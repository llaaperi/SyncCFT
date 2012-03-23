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


/*
 * Constructor
 */
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
    
    //Initialize session handler pointers to NULL
    memset(_sessionHandlers, 0, SERVER_SESSION_HANDLERS * sizeof(SessionHandler*));
}



/*
 * Destructor
 */
Server::~Server(){
    close(_socket);
}



/*
 * Start thread for handling clients
 */
void Server::start(void){
    _running = true;
    pthread_create(&_thread, NULL, handle, this);
    pthread_detach(_thread);
}



/*
 * Stop thread for server
 */
void Server::stop(void){
    _running = false;
    pthread_join(_thread, NULL);
    cout << "Server terminated" << endl;
}



/*
 * Main function for server
 */
void* Server::handle(void* arg){
    Server* handler = (Server*)arg;
    
    cout << "Server handler" << endl;
    
    struct sockaddr cliAddr;
    
    char recvBuffer[NETWORKING_MTU]; 
    char sendBuffer[NETWORKING_MTU]; 
    
    //Main loop
    while(handler->_running){
        
        //Wait incoming packets
        cout << "Server: waiting packets..." << endl;
        int recvLen = Networking::receivePacket(handler->_socket, recvBuffer, &cliAddr, SERVER_TIMEOUT_RECV);
        
        cout << "Received " << recvLen << " bytes" << endl;
        
        // Parse message headers and discard invalid packets
        Message msg;
        if(msg.parseFromBytes(recvBuffer, recvLen) < 0){
            continue;
        }
        
        //msg.printBytes();
        //msg.printInfo();
        
        //Handle new handshake requests
        if(msg.getType() == TYPE_HELLO){
            handler->handshakeHandler(&msg, cliAddr);
        }else{
            
            //Forward existing connections to corresponding handler
            if(handler->_sessionHandlers[msg.getClientID()] != NULL){
                handler->_sessionHandlers[msg.getClientID()]->newMessage(&msg);
            }
        }
    }
    
    return 0;
}



/*
 * Function for requesting next free client id
 * Return: -1 if no free ID's are available or free id
 */
int Server::getFreeID(){
    
    for(int i = 0; i < SERVER_SESSION_HANDLERS; i++){
        if(_sessionHandlers[i] == NULL){
            return i;
        }
    }
    return -1;
}



/*
 * Handshake handler funcion handles new connection requests and allocates ne handler when sucessfull
 * NOT ROBUST YET, ALLOCATES RESOURCES WITHOUT CHECKING
 */
void Server::handshakeHandler(Message* msg, sockaddr cliAddr){
    
    cout << "Handshake handler started" << endl;
    
    int bytes;
    char sendBuffer[HEADER_SIZE];
    
    //Check that there are free client ID's
    int id = getFreeID();
    if(id < 0){
        cout << "Connection refused: ClientID's depleted" << endl;
        
        //Send NACK if ID's are depleted
        msg->incrSeqnum();
        msg->setType(TYPE_NACK);
        Networking::sendPacket(_socket, sendBuffer, HEADER_SIZE,  &cliAddr, SERVER_TIMEOUT_SEND);
        return;
    }
    
    //Allocate resources and send ACK
    _sessionHandlers[id] = new SessionHandler(id, cliAddr);
    msg->incrSeqnum();
    msg->setClientID(id);
    msg->setType(TYPE_ACK);
    msg->parseToBytes(sendBuffer);
    bytes = Networking::sendPacket(_socket, sendBuffer, HEADER_SIZE,  &cliAddr, SERVER_TIMEOUT_SEND);
}
