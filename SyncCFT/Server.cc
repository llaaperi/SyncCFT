//
//  Server.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Server.hh"
#include "Transceiver.hh"
#include "networking.hh"


/*
 * Constructor
 */
Server::Server(Client* clientHandler, string port) throw(invalid_argument,runtime_error) : _port(port), _running(false){
    
    if(clientHandler != NULL){
        _clientHandler = clientHandler;
    }
    else{
        throw invalid_argument("[SERVER] NULL Client");
    }
    
    _socket = Networking::createUnconnectedSocket(_port);
    if(_socket < 0){
        throw runtime_error("[SERVER] Socket creation failed");
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
    cout << "[SERVER] Server terminated" << endl;
}



/*
 * Main function for server
 */
void* Server::handle(void* arg){
    Server* handler = (Server*)arg;
    
    cout << "[SERVER] Server handler" << endl;
    
    struct sockaddr cliAddr;
    Message msg;
    
    //Main loop
    while(handler->_running){
        
        //Wait incoming packets
        cout << "[SERVER] Waiting packets..." << endl;
        while(!Transceiver::recvMsg(handler->_socket, &msg, &cliAddr, SERVER_TIMEOUT_RECV));        
        
        //msg.printBytes();
        //msg.printInfo();
        
        //Server handles HELLO and QUIT messages
        if(msg.isHello()){  //Handle new handshake requests
            handler->handshakeHandler(&msg, cliAddr);
            continue;
        }
        if(msg.isQuit()){   //Handle termination requests
            handler->terminateHandler(&msg, cliAddr);
            continue;
        }
        
        //Forward existing connections to corresponding handler
        if((msg.getClientID() < SERVER_SESSION_HANDLERS) && (handler->_sessionHandlers[msg.getClientID()] != NULL)){
            handler->_sessionHandlers[msg.getClientID()]->newMessage(&msg);
        }else{
            cout << "[SERVER] Packet discarded" << endl;
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
    
    cout << "[SERVER] Handshake handler started" << endl;
    static int clientID = 0;
    
    //Hanshake is initiated
    if(msg->getType() == TYPE_HELLO){
        
        cout << "[SERVER] HELLO received from ";
        Networking::printAddress(&cliAddr);
        cout << endl;
        
        //Check that there are free client ID's
        int id = getFreeID();
        if(id < 0){
            //Send NACK if ID's are depleted
            cout << "[SERVER] Connection refused: ClientID's depleted" << endl;
            msg->incrSeqnum();
            msg->setType(TYPE_NACK);
            msg->clearPayload();
            Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND);
            return;
        }
        
        //Reply with ACK and clientID
        msg->incrSeqnum();
        msg->setClientID(id);
        msg->setType(TYPE_ACK);
        msg->clearPayload();    //Clear payload if if exists
        if(Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND)){
            clientID = id; //Save id if packet was sent succesfully
        }
        
        return;
    }
    
    //Handshake is completed
    if(msg->getType() == TYPE_ACK){
        
        cout << "[SERVER] HELLOACK received from ";
        Networking::printAddress(&cliAddr);
        cout << endl;
        
        //Allocate resources when client ACKs the handshake
        if(msg->getClientID() == clientID){
            _sessionHandlers[clientID] = new SessionHandler(clientID, cliAddr);
        }
        return;
    }
}



/*
 * Handler for session termination
 */
void Server::terminateHandler(Message* msg, sockaddr cliAddr){

    cout << "[SERVER] Terminate handler started" << endl;
    
    static int clientID = 0;
    
    //Termination is initiated
    if(msg->getType() == TYPE_QUIT){
        
        cout << "[SERVER] QUIT received from ";
        Networking::printAddress(&cliAddr);
        cout << endl;
        
        int id = msg->getClientID();
        
        //Check source from session handler
        //TODO
        
        //Reply with final QUITACK
        msg->setType(TYPE_ACK);
        msg->incrSeqnum();
        msg->clearPayload();
        if(Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND)){
            clientID = id; //Save id if packet was sent succesfully
        }
        return;
    }
    
    //Termination is confirmed and compeleted
    if(msg->getType() == TYPE_ACK){
        
        cout << "[SERVER] QUITACK received from ";
        Networking::printAddress(&cliAddr);
        cout << endl;
        
        //Allocate resources when client ACKs the handshake
        if(msg->getClientID() == clientID){
            delete(_sessionHandlers[clientID]);
            _sessionHandlers[clientID] = NULL;
        }
    }
}
