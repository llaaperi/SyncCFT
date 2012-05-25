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


list<string> _serverClients;


/*
 * Constructor
 */
Server::Server(Client* clientHandler, string port, int version, const unsigned char* secretKey) throw(invalid_argument,runtime_error) : _client(clientHandler), _port(port), _version(version), _secretKey(secretKey), _running(false){
    
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
 * Add new sync source to the client hosts
 */
void Server::addSource(string addr, string port){
    
    //cout << "[SERVER] Add new source, ip: " << addr << ", port: " << port << endl;
    if(_client != NULL){
        _client->addHost(addr, port, false); 
    }else{
        cout << "[SERVER] Server does not accept new sources" << endl;
    }
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
        //while(!Transceiver::recvMsg(handler->_socket, &msg, &cliAddr, SERVER_TIMEOUT_RECV));        
        
        if(Transceiver::recvMsg(handler->_socket, &msg, &cliAddr, SERVER_TIMEOUT_RECV)){
        
            //msg.printBytes();
            //msg.printInfo();
            
            //Server handles HELLO and QUIT messages
            if(msg.isHello()){  //Handle new handshake requests
                
                //Check request version number
                if(msg.getVersion() == 1){
                    handler->handshakeHandlerV1(&msg, cliAddr);
                    continue;
                }
                if(msg.getVersion() == 2){
                    handler->handshakeHandlerV2(&msg, cliAddr);
                    continue;
                }
            }
            
            //Forward existing connections to corresponding handler
            if((msg.getClientID() < SERVER_SESSION_HANDLERS) && (handler->_sessionHandlers[msg.getClientID()] != NULL)){
                bool stop = !handler->_sessionHandlers[msg.getClientID()]->newMessage(&msg);
                
                //Release resources if client quits
                if(stop){
                    delete(handler->_sessionHandlers[msg.getClientID()]);
                    handler->_sessionHandlers[msg.getClientID()] = NULL;
                }
                
            }else{
                cout << "[SERVER] Packet discarded" << endl;
                //msg.printInfo();
            }
        }
        
        cout << "[SERVER] Received invalid message" << endl;
        
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
        }else
        if(_sessionHandlers[i]->isExpired()){
            delete(_sessionHandlers[i]);    //Free existing session handler if it has timed out
            _sessionHandlers[i] = NULL;
            return i;
        }
    }
    return -1;
}


/*
 *
 */
PendingClient* Server::getPendingClient(sockaddr cliAddr){
    
    for(PendingClient* client : _pendingClients){
        if(Networking::cmpAddr(&cliAddr, &client->addr)){
            return client;
        }
    }
    return NULL;
}


/*
 *
 */
PendingClient* Server::addPendingClient(sockaddr cliAddr){
    
    //Check that there is free client ids available
    if(getFreeID() < 0){
        return NULL;
    }
    
    PendingClient* newClient = new PendingClient();
    newClient->addr = cliAddr;
    _pendingClients.push_back(newClient);
    return newClient;
}

/*
 *
 */
void Server::removePendingClient(PendingClient* client){
    
    list<PendingClient*>::iterator iter;
    for(iter = _pendingClients.begin(); iter != _pendingClients.end(); iter++){
        if(*iter == client){
            
            cout << "[SERVER] Pending client ";
            Networking::printAddress(&client->addr);
            cout << " erased" << endl;
            
            delete(client);
            _pendingClients.erase(iter);
            return;
        }
    }
}


/*
 * Handshake handler funcion handles new connection requests and allocates ne handler when sucessfull
 * NOT ROBUST YET, ALLOCATES RESOURCES WITHOUT CHECKING
 */
void Server::handshakeHandlerV1(Message* msg, sockaddr cliAddr){
    
    cout << "[SERVER] Handshake handler version 1 started" << endl;
    static int clientID = 0;
    
    if(_version > 1){
        //Send NACK if version 1 is not supported by the server
        cout << "[SERVER] Connection refused: Version 1 not supported" << endl;
        msg->incrSeqnum();
        msg->setType(TYPE_NACK);
        msg->clearPayload();
        Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND);
        return;
    }
    
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
            createNewSession(clientID, cliAddr, msg->getSeqnum(), NULL);
        }
        return;
    }
}


/*
 *
 */
void Server::replyNACK(Message* msg, sockaddr cliAddr){
    msg->incrSeqnum();
    msg->setType(TYPE_NACK);
    msg->clearPayload();
    Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND);
}


/*
 *
 */
void Server::createNewSession(int clientID, sockaddr cliAddr, uint32_t seqnum, unsigned char* sessionKey){
    Transceiver* trns = new Transceiver(_socket, cliAddr, sessionKey);
    _sessionHandlers[clientID] = new SessionHandler(this, trns, clientID, seqnum, sessionKey);
}


/*
 *
 */
void Server::handshakeHandlerV2(Message* msg, sockaddr cliAddr){
    
    cout << "[SERVER] Handshake handler version 2 started" << endl;    
    
    //Hanshake is initiated
    if(msg->getType() == TYPE_HELLO){
        handshakeHandlerV2Hello(msg, cliAddr);
        return;
    }
    
    //Check client hash
    if(msg->getType() == TYPE_ACK){
        handshakeHandlerV2Ack(msg, cliAddr);
        return;
    }
}


/*
 *
 */
void Server::handshakeHandlerV2Hello(Message* msg, sockaddr cliAddr){
    
    cout << "[SERVER] HELLO received from ";
    Networking::printAddress(&cliAddr);
    cout << endl;
    
    //Get pending client for overwriting if it exists
    PendingClient* client = getPendingClient(cliAddr);
    if(client == NULL){ //Create new pending client if needed
        client = addPendingClient(cliAddr);
    }
    
    if(client == NULL){
        //Send NACK if ID's are depleted
        cout << "[SERVER] Connection refused: ClientID's depleted" << endl;
        replyNACK(msg, cliAddr);
        return;
    }
    
    //Check nonce
    if(msg->getPayloadLength() < 16){
        cout << "[SERVER] Connection refused: Invalid nonce" << endl;
        replyNACK(msg, cliAddr);
        return;
    }
    
    //Create server nonce and save with client nonce to pending client
    Utilities::randomBytes(client->sNonce, 16);
    memcpy(client->cNonce, msg->getPayload(), 16);
    
    //Reply with ACK and clientID
    msg->incrSeqnum();
    msg->setClientID(0);
    msg->setType(TYPE_ACK);
    msg->setPayload((char*)(client->sNonce), 16);
    
    if(!Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND)){
        return;
    }
    
    //msg->printInfo();
    
    cout << "[SERVER] Server nonce: ";
    Utilities::printBytes(client->sNonce, 16);
    cout << endl;
    cout << "[SERVER] Client nonce: ";
    Utilities::printBytes(client->cNonce, 16);
    cout << endl;
    
    return;
}


/*
 *
 */
void Server::handshakeHandlerV2Ack(Message* msg, sockaddr cliAddr){
    
    cout << "[SERVER] HELLOACK received from ";
    Networking::printAddress(&cliAddr);
    cout << endl;
    
    //msg->printInfo();
    PendingClient* client = getPendingClient(cliAddr);
    
    //Terminate if unknown source
    if(client == NULL){
        cout << "[SERVER] HELLOACK from unknown source" << endl;
        replyNACK(msg, cliAddr);
        return;
    }
    
    int clientID = getFreeID();
    if(clientID < 0){
        //Send NACK if ID's are depleted
        cout << "[SERVER] Connection refused: ClientID's depleted" << endl;
        replyNACK(msg, cliAddr);
        removePendingClient(client);
        return;
    }
    
    if(msg->getPayloadLength() < HASH_LENGTH){
        cout << "[SERVER] Connection refused: Invalid hash" << endl;
        replyNACK(msg, cliAddr);
        removePendingClient(client);
        return;
    }
    
    unsigned char hash[HASH_LENGTH];
    Utilities::nonceHash(hash, client->sNonce, _secretKey);
    
    cout << "Server hash: " << endl;
    Utilities::printBytes(hash, HASH_LENGTH);
    cout << endl;
    
    //Utilities::printBytes((unsigned char*)msg->getPayload(), 256);
    
    cout << "[SERVER] Server nonce: ";
    Utilities::printBytes(client->sNonce, 16);
    cout << endl;
    cout << "[SERVER] Client nonce: ";
    Utilities::printBytes(client->cNonce, 16);
    cout << endl;
    
    //Check that client hash is correct
    if(memcmp(hash, msg->getPayload(), HASH_LENGTH)){
        cout << "[SERVER] Connection refused: Invalid hash value" << endl;
        replyNACK(msg, cliAddr);
        removePendingClient(client);
        return;
    }
    
    //Reply with ACK containing hash
    Utilities::nonceHash(hash, client->cNonce, _secretKey);
    msg->incrSeqnum();
    msg->setPayload((char*)hash, HASH_LENGTH);
    msg->setHello(true);
    if(!Transceiver::sendMsg(_socket, msg, &cliAddr, SERVER_TIMEOUT_SEND)){
        removePendingClient(client);
        return;
    }
    
    //Create sesion key
    unsigned char* sKey = Utilities::sessionKey(client->sNonce, client->cNonce, _secretKey);
    
    //Allocate resources when client ACKs the handshake
    if(msg->getClientID() == clientID){
        createNewSession(clientID, cliAddr, msg->getSeqnum(), sKey);
    }
    removePendingClient(client);
    
    cout << "[SERVER] Handshake finished: session key=";
    Utilities::printBytes(sKey, HASH_LENGTH);
    cout << endl;
    
    return;
}


/*
 * Handler for session termination
 */
/*
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
        
        //Release resources when client ACKs the handshake
        if(msg->getClientID() == clientID){
            
            if(!_sessionHandlers[clientID]){
                delete(_sessionHandlers[clientID]);
                _sessionHandlers[clientID] = NULL;
            }
        }
        
        //Reply with final QUITACK
        msg->setType(TYPE_ACK);
        msg->setQuit(true);
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
*/
