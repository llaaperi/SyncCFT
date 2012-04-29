//
//  Client.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "Client.hh"
#include "networking.hh"
#include "FileTransfer.hh"

Client::Client(list<string>& hosts, string cport, string sport) throw(invalid_argument, runtime_error) : _hosts(hosts), _cport(cport), _sport(sport){
    
    if(true){
        
    }
    else{
        throw invalid_argument("[CLIENT] Invalid arguments");
    }
    
    _socket = Networking::createUnconnectedSocket(_cport);
    if(_socket < 0){
        throw runtime_error("[CLIENT] Socket creation failed");
    }
    
    // Find out server address
    // TODO: Currently supports only one server
    struct addrinfo hints;
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    //Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    if(getaddrinfo(_hosts.front().c_str(), _sport.c_str(), &hints, &_serverInfo)) { //getaddrinfo returns 0 on success
        throw runtime_error("[CLIENT] Running getaddrinfo failed.");
    }
    
    // Create new tranceiver
    _trns = new Transceiver(_socket, *getSockAddr());

}


/**
 *
 * Destructor for Client class
 *
 **/
Client::~Client() {
    
    // Close open socket
    if (_socket < 0) {
        close(_socket);  

    }
    // TODO: Free rest of the resources
    
}


/*
 * Start thread for handling clients
 */
void Client::start(void)
{
    _running = true;
    pthread_create(&_thread, NULL, handle, this);
    pthread_detach(_thread);
}



/*
 * Stop thread for handling clients
 */
void Client::stop(void) {
    _running = false;
    pthread_join(_thread, NULL);
    cout << "[CLIENT] Client terminated" << endl;
}



/*
 * Main function for client thread
 */
void* Client::handle(void* arg)
{
    Client* handler = (Client*)arg;
    
    cout << "[CLIENT] Client handler" << endl;
    
    
    //TEMP SLEEP
    sleep(2);
    
    while(handler->_running){
        sockaddr* sockAddr = handler->getSockAddr();
        
        //Try HELLO handshake
        handler->startSession(*sockAddr);
        
        //Metafile handler
        MetaFile* diff;
        handler->metafileHandler(*sockAddr, &diff);
        
        //File transfers
        if(diff != NULL){
            handler->fileTransfer(*sockAddr, diff);
        }
        
        
        //Terminate session
        handler->endSession(*sockAddr);
        
        sleep(CLIENT_BACKOFF); //TEMPORARILY HERE
    }
    return 0;
}


/*
 *
 */
void Client::metafileHandler(sockaddr servAddr, MetaFile** diff){
    
    cout << "[CLIENT] Obtaining metafile from ";
    Networking::printAddress(&servAddr);
    cout << endl;
    
    Message msg;
    //TODO sequence numbers
    msg.init(TYPE_DESCR);
    msg.setClientID(_id);
    
    MetaFile mFile(METAFILE);
    
    //TODO add metafile to payload
    string payload = mFile.getDescr();
    //cout << "[CLIENT] Own descriptio:" << endl;
    //cout << payload << endl;
    msg.setPayload(payload.c_str(), (int)payload.length());
    if(!_trns->send(&msg, CLIENT_TIMEOUT_SEND)){
        return;
    }
    
    //Receive DIFF from the server
    if(!_trns->recv(&msg, CLIENT_TIMEOUT_HELLO)){
        return;
    }
    
    //Check that received DIFF message
    if(msg.getType() != TYPE_DIFF){
        return;
    }
    
    //msg.printInfo();
    //TODO Possible refactoring to MetaFile constructior to throw invalid argument exception
    if(msg.getPayloadLength() > 0){
        *diff = new MetaFile(msg.getPayload(), msg.getPayloadLength());
        cout << "[CLIENT] Received DIFF:" << endl;
        (*diff)->print();
    }else{
        *diff = NULL;
        cout << "[CLIENT] Received empty DIFF" << endl;
    }
    
}


/*
 * //TODO: servAddr parameter not used anywhere
 */
void Client::fileTransfer(sockaddr servAddr, MetaFile* diff){
    
    cout << "[CLIENT] File transfer started" << endl;
    
    Message msg;
    
    list<Element> elements = diff->getData();
    
    for(Element e : elements){
        cout << "[CLIENT] Request file: " << e.getName() << endl;
        
        msg.initHeader(TYPE_GET);
		msg.setWindow(2);
        
        char buf[NETWORKING_MTU];
        sprintf(buf, "%s;%d-%d", e.getName().c_str(), 0, 0);
        msg.setPayload(buf, (int)strlen(buf));
        
        if(!_trns->send(&msg, CLIENT_TIMEOUT_SEND)){
            cout << "[CLIENT] Unable to send GET" << endl;
            return;
        }
        
        int tries = 0;
        // Send GET message
        while(!_trns->recv(&msg, CLIENT_TIMEOUT_ACK)) {
            // Send new GET    
            _trns->send(&msg, CLIENT_TIMEOUT_SEND);
            if (++tries > CLIENT_RETRIES) {
                cout << "[CLIENT] Unable to receive reply to GET" << endl;
                return;
            }
        }
        
        // Handle reply to GET message
        bool firstFile = false;
        cout << "[CLIENT] received first message after GET" << endl;
        if (msg.getType() == TYPE_NACK) {
            cout << "[CLIENT] Received NACK message after GET" << endl;
            continue;
        } else if (msg.getType() == TYPE_ACK) {
            cout << "[CLIENT] Received ACK message after GET" << endl;
        } else if (msg.getType() == TYPE_FILE) {
            cout << "[CLIENT] Received FILE message after GET" << endl;
            firstFile = true;
        }

        try {
            _fFlow = new FileTransfer(_trns, e, 0, 0, 0, FILE_TRANSFER_TYPE_CLIENT);
        } catch (...) {
            cout << "[CLIENT] File could not be opened" << endl;
            continue;
        }
        
        if (compliteFileTransfer(&msg, firstFile)) {
            cout << "[CLIENT] Completed file" << endl;
        } else {
            cout << "[CLIENT] Failed to complete file" << endl;
        }
        
        delete(_fFlow);
    }
}

/**
 * Complite single file transfer
 * @msg Message to begin the transfer session from
 * @first Is the first message type FILE
 * @return Success status of the file transfer
 **/
bool Client::compliteFileTransfer(Message* msg, bool first) {
    
    bool ready = false;
    int tries = 0;
    while(!ready){
        if(!first && !_trns->recv(msg, CLIENT_TIMEOUT_ACK)) {
            cout << "[CLIENT] Wait FILE timeout" << endl;
            if (tries++ > CLIENT_RETRIES) {
                cout << "[CLIENT] Too many retries" << endl;
                return false;
            }
            _fFlow->recvTimeout(msg);
            continue;
        }
        tries = 0;
        cout << "[CLIENT] Received file message from Chunk: " << msg->getChunk() << ", Seqnum: " << msg->getSeqnum() << ", Size: " << msg->getPayloadLength() << ", Window size: " << msg->getWindow() << endl;
        ready = _fFlow->recvFile(msg);
        first = false;
    }
    return true;
}


/**
 * Function tries to start session with a server.
 **/
void Client::startSession(sockaddr servAddr){
    
    bool started = false;
    do{
        cout << "[CLIENT] Trying to start session ..." << endl;
        started = handshakeHandler(servAddr);
        if(started){
            cout << "[CLIENT] Session started succesfully" << endl;
        }else{
            cout << "[CLIENT] Session start failed, retrying in " << CLIENT_BACKOFF << " seconds" << endl;
            sleep(CLIENT_BACKOFF);
        }
    }while(!started);
}



/*
 * Function terminates current session
 */
void Client::endSession(sockaddr servAddr){
    
    bool terminated = false;
    do{
        cout << "[CLIENT] Trying to terminate session ..." << endl;
        terminated = terminateHandler(servAddr);
        if(terminated){
            cout << "[CLIENT] Session terminated succesfully" << endl;
        }else{
            cout << "[CLIENT] Session termination failed, retrying in " << CLIENT_BACKOFF << " seconds" << endl;
            sleep(CLIENT_BACKOFF);
        }
    }while(!terminated);
}



/*
 * HandshakeHandler function handles the HELLO handshake between client and server.
 * Returns true if handshake was successfull and false if it failed.
 */
bool Client::handshakeHandler(sockaddr servAddr){
    
    Message msg;
    
    //Send HELLO message
    msg.initHeader(TYPE_HELLO);
    /*if(!Transceiver::sendMsg(_socket, &msg, &servAddr, CLIENT_TIMEOUT_SEND)){
        return false;
    }*/
    if(!_trns->send(&msg, CLIENT_TIMEOUT_SEND)){
        return false;
    }
    
    //Receive reply from the server
    if(!_trns->recv(&msg, CLIENT_TIMEOUT_HELLO)){
        return false;
    }
    
    //Check that received HELLOACK
    if(msg.getType() != TYPE_ACK){
        return false;
    }
    
    //Save id
    _id = msg.getClientID();
    
    //Reply with final HELLOACK
    msg.incrSeqnum();
    msg.setPayload(NULL, 0);
    msg.setHello(true);
    _trns->send(&msg, CLIENT_TIMEOUT_HELLO);
    
    return true;
}



/*
 * Handleds termination of the session
 */
bool Client::terminateHandler(sockaddr servAddr){
    
    Message msg;
    
    //Send QUIT message
    msg.initHeader(TYPE_QUIT);
    if(!_trns->send(&msg, CLIENT_TIMEOUT_SEND)){
        return false;
    }
    
    //Receive reply from the server
    if(!_trns->recv(&msg, CLIENT_TIMEOUT_QUIT)){
        return false;
    }
    
    //Check that received ACK
    if(msg.getType() != TYPE_ACK){
        return false;
    }
    
    //Remove id
    _id = 0;
    
    //Reply with final QUITACK
    //msg.incrSeqnum();
    //msg.clearPayload();
    //msg.setQuit(true);
    //_trns->send(&msg, CLIENT_TIMEOUT_HELLO);
    
    return true;
}
