//
//  Client.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Client.hh"
#include "networking.hh"

Client::Client(list<string>& hosts, string cport, string sport) throw(invalid_argument, runtime_error) : _cport(cport), _sport(sport){
    
    if(true){
        
    }
    else{
        throw invalid_argument("[CLIENT] Invalid arguments");
    }
    
    _socket = Networking::createUnconnectedSocket(_cport);
    if(_socket < 0){
        throw runtime_error("[CLIENT] Socket creation failed");
    }
    
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
    
    //char sendBuffer[NETWORKING_MTU];
    //char recvBuffer[NETWORKING_MTU];
    
    struct addrinfo hints, *serverInfo;
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    //Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    if(getaddrinfo("127.0.0.1", handler->_sport.c_str(), &hints, &serverInfo)) { //getaddrinfo returns 0 on success
        perror("[CLIENT] Running getaddrinfo failed.");
        return 0;
    }
    
    //TEMP SLEEP
    sleep(2);
    
    while(handler->_running){
        
        //Try HELLO handshake
        handler->startSession(*serverInfo->ai_addr);
        
        //Metafile handler
        //TODO
        
        //File transfers
        //TODO
        
        //Terminate session
        handler->endSession(*serverInfo->ai_addr);
        
        sleep(CLIENT_TIMEOUT_BACKOFF); //TEMPORARILY HERE
    }
    return 0;
}



/*
 * Function tries to start session with a server.
 */
void Client::startSession(sockaddr servAddr){
    
    bool started = false;
    do{
        cout << "[CLIENT] Trying to start session ..." << endl;
        started = handshakeHandler(servAddr);
        if(started){
            cout << "[CLIENT] Session started succesfully" << endl;
        }else{
            cout << "[CLIENT] Session start failed" << endl;
            sleep(CLIENT_TIMEOUT_BACKOFF);
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
            cout << "[CLIENT] Session termination failed" << endl;
            sleep(CLIENT_TIMEOUT_BACKOFF);
        }
    }while(!terminated);
}



/*
 * HandshakeHandler function handles the HELLO handshake between client and server.
 * Returns true if handshake was successfull and false if it failed.
 */
bool Client::handshakeHandler(sockaddr servAddr){
    
    int bytes = 0;
    char sendBuffer[NETWORKING_MTU];
    char recvBuffer[NETWORKING_MTU];
    
    //Create HELLO message
    Message msg;
    msg.initHeader(TYPE_HELLO);
    msg.parseToBytes(sendBuffer);
    
    //Send HELLO message
    bytes = Networking::sendPacket(_socket, sendBuffer, HEADER_SIZE, &servAddr, CLIENT_TIMEOUT_SEND);
    
    //If send timeouted
    if(bytes < 0){
        return false;
    }
    
    //Receive reply from the server
    bytes = Networking::receivePacket(_socket, recvBuffer, &servAddr, CLIENT_TIMEOUT_HELLO);
    
    //If recv HELLOACK timeouted or packet was invalid
    if((bytes < 0) || !msg.parseFromBytes(recvBuffer, bytes)){
        return false;
    }
    
    //Check that received HELLOACK
    if(msg.getType() != TYPE_ACK){
        return false;
    }
    
    //Reply with final HELLOACK
    msg.incrSeqnum();
    msg.setPayload(NULL, 0);
    msg.parseToBytes(sendBuffer);
    bytes = Networking::sendPacket(_socket, sendBuffer, HEADER_SIZE, &servAddr, CLIENT_TIMEOUT_SEND);
    
    return true;
}



/*
 * Handleds termination of the session
 */
bool Client::terminateHandler(sockaddr servAddr){
    
    int bytes = 0;
    char sendBuffer[NETWORKING_MTU];
    char recvBuffer[NETWORKING_MTU];
    
    //Create QUIT message
    Message msg;
    msg.initHeader(TYPE_QUIT);
    msg.parseToBytes(sendBuffer);
    
    //Send QUIT message
    bytes = Networking::sendPacket(_socket, sendBuffer, HEADER_SIZE, &servAddr, CLIENT_TIMEOUT_SEND);
    
    //If send timeouted
    if(bytes < 0){
        return false;
    }
    
    //Receive reply from the server
    bytes = Networking::receivePacket(_socket, recvBuffer, &servAddr, CLIENT_TIMEOUT_QUIT);
    
    //If recv ACK timeouted or packet was invalid
    if((bytes < 0) || !msg.parseFromBytes(recvBuffer, bytes)){
        return false;
    }
    
    //Check that received ACK
    if(msg.getType() != TYPE_ACK){
        return false;
    }
    return true;
}
