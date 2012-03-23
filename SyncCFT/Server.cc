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
    
    struct sockaddr_in cliAddr;
    
    char recvBuffer[NETWORKING_MTU]; 
    
    //Main loop
    while(handler->_running){
        
        cout << "Server: waiting packets..." << endl;
        
        int recvLen = Networking::receivePacket(handler->_socket, recvBuffer, &cliAddr);
        
        cout << "Received " << recvLen << " bytes" << endl;
        
        
        
        
    }
    
    return 0;
}
