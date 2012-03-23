//
//  Server.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Server.hh"
#include "networking.hh"


Server::Server(Client* clientHandler, string port) throw(invalid_argument,runtime_error) : _port(port), _running(false){
    
    if(clientHandler != NULL){
        _clientHandler = clientHandler;
    }
    else{
        throw invalid_argument("NULL Client");
    }
    
    _socket = Networking::createUnconnectedSocket(_port);
    if(_socket < 0){
        throw runtime_error("Socket creation failed");
    }
}

Server::~Server(){
    close(_socket);
}


/*
 * Start thread for handling clients
 */
void Server::start(void) {
    _running = true;
    pthread_create(&_thread, NULL, handle, this);
    pthread_detach(_thread);
}


/*
 * Stop thread for server
 */
void Server::stop(void) {
    _running = false;
    pthread_join(_thread, NULL);
    cout << "Server terminated" << endl;
}


/*
 * Main function for server
 */
void* Server::handle(void* arg)
{
    Server* handler = (Server*)arg;
    
    cout << "Server handler" << endl;
    
    struct sockaddr cliAddr;
    
    char recvBuffer[NETWORKING_MTU]; 
    char sendBuffer[NETWORKING_MTU]; 
    
    //Main loop
    while(handler->_running){
        
        cout << "Server: waiting packets..." << endl;
        
        int recvLen = Networking::receivePacket(handler->_socket, recvBuffer, &cliAddr, SERVER_TIMEOUT_RECV);
        
        cout << "Received " << recvLen << " bytes" << endl;
        
        // Parse message headers
        Message msg;
        if (msg.parseFromBytes(recvBuffer, recvLen) < 0)
            continue;
        
        msg.printBytes();
        msg.printInfo();
        
        int bytes;
        switch (msg.getType()) {
            case TYPE_ACK:
                cout << "Received ACK message" << endl;
                break;
            case TYPE_HELLO:
                cout << "Received HELLO message" << endl;
                msg.incrSeqnum();
                msg.setClientID(1);
                msg.setType(TYPE_ACK);
                msg.parseToBytes(sendBuffer);
                bytes = Networking::sendPacket(handler->_socket, sendBuffer, HEADER_SIZE,  &cliAddr, SERVER_TIMEOUT_SEND);
                
                break;
            case TYPE_DESCR:
                cout << "Received DESCR message" << endl;
                break;
            case TYPE_DIFF:
                cout << "Received DIFF message" << endl;
                break;
            case TYPE_GET:
                cout << "Received GET message" << endl;
                break;
            case TYPE_FILE:
                cout << "Received FILE message" << endl;
                break;
            case TYPE_QUIT:
                cout << "Received QUIT message" << endl;
                break;
            case TYPE_NACK:
                cout << "Received NACK message" << endl;
                break;
            default:
                cout << "Unknown message type" << endl;
                break;
        }
    }
    
    return 0;
}
