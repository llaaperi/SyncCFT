//
//  Server.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Server_hh
#define SyncCFT_Server_hh

#include "Transceiver.hh"
#include "SessionHandler.hh" 
#include "Metafile.hh"
#include "Message.hh"
#include "Client.hh"
#include "stdexcept"

//Timeouts in milliseconds
#define SERVER_TIMEOUT_RECV 1000000
#define SERVER_TIMEOUT_SEND 5000
#define SERVER_TIMEOUT_HELLOACK 5000

#define SERVER_SESSION_HANDLERS 3 //Number of handlers can be defined. Maximum value is 256.

extern list<string> _serverClients; //ip:port

using namespace std;

class Server {
    //Transceiver mTransceiver;
    Client* _client;
    pthread_t _thread;
    bool _running;
    string _port;
    int _socket;
    int _version;
    
    SessionHandler* _sessionHandlers[SERVER_SESSION_HANDLERS];

public:
    
    Server(Client* clientHandler, string port, int version) throw(invalid_argument,runtime_error);
    ~Server();
    
    /*
     * Start thread for handling server messages
     */
    void start();
    
    /*
     * Stop thread
     */
    void stop();
    
    /*
     * Main function for server thread
     */
    static void* handle(void* arg);
    
    void addSource(string addr, string port);

private:
    //Rule of three
    Server(Server const& other);
    Server operator=(Server const& other);
    
    int getFreeID();
    void sourceHandler();
    void handshakeHandlerV1(Message* msg, sockaddr cliAddr);
    void handshakeHandlerV2(Message* msg, sockaddr cliAddr);
    //void terminateHandler(Message* msg, sockaddr cliAddr);
};

#endif
