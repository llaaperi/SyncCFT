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


Client::Client(list<string> hosts, string cport, string sport, int version, const unsigned char* secretKey, int mode) throw(invalid_argument, runtime_error) : _cport(cport), _sport(sport), _version(version), _secretKey(secretKey), _mode(mode), _running(false), _finished(false){
    
	//cout << "[CLIENT] Client constructor with mode: " << mode << endl;
    
    _socket = Networking::createUnconnectedSocket(_cport);
    if(_socket < 0){
        throw runtime_error("[CLIENT] Socket creation failed");
    }
    
    //Add hosts
    for(string host : hosts){
        addHost(host, sport, true);
    }
}


/**
 *
 * Destructor for Client class
 *
 **/
Client::~Client() {
    
    cout << "[CLIENT] Destructor" << endl;
    
    // Close open socket
    if(_socket >= 0) {
        cout << "[CLIENT] Closing socket" << endl;
        close(_socket);
    }
    
    if(_trns != NULL){
        delete(_trns);
    }
    
    if(_fFlow != NULL){
        delete(_fFlow);
    }
    
    if(_sessionKey != NULL){
        free(_sessionKey);
    }
}


/*
 *
 */
void Client::addHost(string addr, string port, bool permanent){
    
    cout << "[CLIENT] Adding new host, ip: " << addr << ", port: " << port << endl;
    
    //Check duplicates
    for(Host h : _hosts){
        if((h.ip == addr) && (h.port == port)){
            cout << "[CLIENT] Duplicate host" << endl;
            return;
        }
    }
    
    Host newHost;
    newHost.perm = permanent;   //Set host type
    newHost.ip = addr;
    newHost.port = port;
    
    // Find out server address
    // TODO: Currently supports only one server
    struct addrinfo hints;
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    //Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    if(getaddrinfo(addr.c_str(), port.c_str(), &hints, &newHost.serverInfo)) { //getaddrinfo returns 0 on success
        throw runtime_error("[CLIENT] Running getaddrinfo failed.");
    }
    
     _hosts.push_back(newHost);
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
    
    while(handler->_running){
        
        list<Host>::iterator iter;
        for(iter = handler->_hosts.begin(); iter != handler->_hosts.end(); iter++){
            handler->sessionHandler(*iter);
            
            //Remove temporary hosts
            if(!(*iter).perm){
                cout << "[CLIENT] Removing temporary host " << (*iter).ip << endl;
                handler->_hosts.erase(iter);
            }
        }
		
        
        
        sleep(CLIENT_REFRESH);
    }
    handler->_running = false;
    handler->_finished = true;
		
    return 0;
}


/*
 *
 */
void Client::sessionHandler(Host h){
    
    cout << "[CLIENT] New session with host " << h.ip << endl;
    
    sockaddr* sockAddr = h.serverInfo->ai_addr;
    
    Transceiver trans(_socket, *sockAddr);
    _trns = &trans;
    
    //Try HELLO handshake
    startSession(*sockAddr);
    
    //Metafile handler
    MetaFile* diff;
    metafileHandler(*sockAddr, &diff);
    
    //File transfers
    if(diff != NULL){
        fileTransfer(*sockAddr, diff);
    }
    
    //Terminate session
    endSession(*sockAddr);
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
    delete(diff);   //Free previously allocated MetaFile
    
    for(Element e : elements){
        cout << "[CLIENT] Request file: " << e.getName() << endl;
        
        msg.initHeader(TYPE_GET);   //Create GET message
		msg.setWindow(1);   //Init window size to 1
        
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
            if (++tries > CLIENT_CHUNK_RETRIES) {
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
        
        if (completeFileTransfer(&msg, firstFile)) {
            cout << "[CLIENT] Completed file" << endl;
        } else {
            cout << "[CLIENT] Failed to complete file" << endl;
            delete(_fFlow);
            _fFlow = NULL;
            return; // Terminate session
        }
        
        delete(_fFlow);
        _fFlow = NULL;
    }
}

/**
 * Complete single file transfer
 * @msg Message to begin the transfer session from
 * @first Is the first message type FILE
 * @return Success status of the file transfer
 **/
bool Client::completeFileTransfer(Message* msg, bool first) {
    
    Timer tmr;
    tmr.start();
    long timeout = CLIENT_TIMEOUT_FILE;
    
    bool ready = false;
    int tries = 0;
    while(!ready){
        
        if(!first && !_trns->recv(msg, (int)timeout)) {
            cout << "[CLIENT] Wait FILE timeout" << endl;
            if (tries++ > CLIENT_CHUNK_RETRIES) {
                cout << "[CLIENT] Too many retries" << endl;
                return false;
            }
            _fFlow->recvTimeout(msg);
            continue;
        }
        tries = 0;
        
        long ms = tmr.elapsed_ms();
        timeout = ms * 3;   //TODO moving average
        if (timeout < 100) { // Set minimum timeout value
            timeout = 100;
        }
        cout << "[CLIENT] Inter-arrival time (ms): " << ms << endl;
        
        tmr.start();
        
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
        if(_version == 1){
            started = handshakeHandlerV1(servAddr);
        }else{
            started = handshakeHandlerV2(servAddr);
        }
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
    int retries = CLIENT_QUIT_RETRIES;
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
        if ((retries--) <= 0) { // Retry quit CLIENT_QUIT_RETRIES times
            return;
        }
    }while(!terminated);
}



/*
 * HandshakeHandler function handles the HELLO handshake between client and server.
 * Returns true if handshake was successfull and false if it failed.
 */
bool Client::handshakeHandlerV1(sockaddr servAddr){
    
    Message msg;
    
    cout << "[CLIENT] Version 1 handshake handler" << endl;
    
    //Send HELLO message
    msg.initHeader(TYPE_HELLO);
    msg.setVersion(1);
    
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
 * HandshakeHandler function handles the HELLO handshake between client and server.
 * Returns true if handshake was successfull and false if it failed.
 */
bool Client::handshakeHandlerV2(sockaddr servAddr){
    
    unsigned char cNonce[16];
    unsigned char sNonce[16];
    Message msg;
    
    cout << "[CLIENT] Version 2 handshake handler" << endl;
    
    //**********************Send HELLO message *************************
    msg.initHeader(TYPE_HELLO);
    msg.setVersion(2);
    
    Utilities::randomBytes(cNonce, 16);
    msg.setPayload((char*)cNonce, 16);
    if(!_trns->send(&msg, CLIENT_TIMEOUT_SEND)){
        return false;
    }
    
    //******************Receive reply from the server *************************
    if(!_trns->recv(&msg, CLIENT_TIMEOUT_HELLO)){
        return false;
    }
    
    //Check that received HELLOACK with nonce
    if((msg.getType() != TYPE_ACK) || (msg.getPayloadLength() < 16)){
        return false;
    }
    
    //Get servers nonce
    memcpy(sNonce, msg.getPayload(), 16);
    cout << "[CLIENT] Server nonce: ";
    Utilities::printBytes(sNonce, 16);
    cout << endl;
    cout << "[CLIENT] Client nonce: ";
    Utilities::printBytes(cNonce, 16);
    cout << endl;
    
    //*********************Send key hash *****************
    
    unsigned char hash[HASH_LENGTH];
    
	Utilities::nonceHash(hash, sNonce, _secretKey);
    
    cout << "[CLIENT] Client hash: " << endl;
    Utilities::printBytes(hash, HASH_LENGTH);
    cout << endl;
    
    //Reply with ACK containing hash
	msg.setType(TYPE_ACK);
    msg.incrSeqnum();
    msg.setPayload((char*)hash, HASH_LENGTH);
    msg.setHello(true);
    if(!_trns->send(&msg, CLIENT_TIMEOUT_SEND)){
        return false;
    }
	//msg.printInfo();
    
    //************** check server responce ****************
    if(!_trns->recv(&msg, CLIENT_TIMEOUT_HELLO)){
        return false;
    }
    
    Utilities::nonceHash(hash, cNonce, _secretKey);
    
    cout << "[CLIENT] Server hash: " << endl;
    Utilities::printBytes(hash, HASH_LENGTH);
    cout << endl;
    
    //Utilities::printBytes((unsigned char*)msg->getPayload(), 256);
    
    //Check that server hash is correct
    if(memcmp(hash, msg.getPayload(), HASH_LENGTH)){
        cout << "[CLIENT] Handshake failed: Invalid server hash value" << endl;
        return false;
    }
    
    _sessionKey = Utilities::sessionKey(sNonce, cNonce, _secretKey);
    
    //Save id
    _id = msg.getClientID();
    
    cout << "[CLIENT] Handshake finished: session key=";
    Utilities::printBytes(_sessionKey, HASH_LENGTH);
    cout << endl;
    
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
    
    return true;
}
