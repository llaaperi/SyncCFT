//
//  Client.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Client_hh
#define SyncCFT_Client_hh

#include "Transceiver.hh"
#include "FileTransfer.hh"
#include "Metafile.hh"
#include "Message.hh"
#include <stdexcept>

//Timeouts in milliseconds
#define CLIENT_TIMEOUT_SEND 5000
#define CLIENT_TIMEOUT_ACK 5000
#define CLIENT_TIMEOUT_HELLO 5000
#define CLIENT_TIMEOUT_HELLOACK 5000
#define CLIENT_TIMEOUT_QUIT 5000
#define CLIENT_TIMEOUT_FILE 500
#define CLIENT_RETRIES 5
#define CLIENT_BACKOFF 2 // In seconds
#define CLIENT_REFRESH 60


using namespace std;

class Client {
    //Transceiver _transceiver;
    pthread_t _thread;
    list<string> _hosts;
    string _cport;
    string _sport;
    bool _running;
    int _socket;
    uint8_t _id;
    Transceiver* _trns;
    FileTransfer* _fFlow;
    
    struct addrinfo* _serverInfo;

    
public:
    
    Client(list<string>& hosts, string cport, string sport) throw(invalid_argument, runtime_error);
    ~Client();
    
    /*
     * Start thread for handling clients
     */
    void start();
    
    /*
     * Stop thread
     */
    void stop();
    
    /*
     * Main function for client thread
     */
    static void* handle(void* arg);
    
    void synchronize(MetaFile file);
    
    sockaddr* getSockAddr(){return _serverInfo->ai_addr;}
    
    
private:
    //Rule of three
    Client(Client const& other);
    Client operator=(Client const& other);
    
    void startSession(sockaddr servAddr);
    void endSession(sockaddr servAddr);
    
    bool handshakeHandler(sockaddr servAddr);
    bool terminateHandler(sockaddr servAddr);
    void metafileHandler(sockaddr servAddr, MetaFile** diff);
    void fileTransfer(sockaddr servAddr, MetaFile* diff);
    bool compliteFileTransfer(Message* msg, bool first);

};

#endif
