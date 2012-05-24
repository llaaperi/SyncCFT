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
#include "Timer.hh"
#include <stdexcept>

//Timeouts in milliseconds
#define CLIENT_TIMEOUT_SEND 5000
#define CLIENT_TIMEOUT_ACK 5000
#define CLIENT_TIMEOUT_HELLO 5000
#define CLIENT_TIMEOUT_HELLOACK 5000
#define CLIENT_TIMEOUT_QUIT 5000
#define CLIENT_TIMEOUT_FILE 5000
#define CLIENT_CHUNK_RETRIES 5 // Max chunk retries
#define CLIENT_QUIT_RETRIES 3 // Max quit retries
#define CLIENT_BACKOFF 2 // In seconds
#define CLIENT_REFRESH 10


using namespace std;

struct Host {
    string ip;
    string port;
    struct addrinfo* serverInfo;
    bool perm;  //Permanent host (from command line)
};

class Client {
    //Transceiver _transceiver;
    pthread_t _thread;
    list<Host> _hosts;
    string _cport;
    string _sport;
    bool _running;
    bool _finished;
	int _mode;
    int _socket;
    int _version;
    uint8_t _id;
    Transceiver* _trns;
    FileTransfer* _fFlow;
    

    
public:
    /*
	 * Mode defines how many times the synchronization operation is repeated, 0 = infinite
	 */
    Client(list<string> hosts, string cport, string sport, int version, int mode=0) throw(invalid_argument, runtime_error);
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
     *
     */
    bool isRunning(){return _running;}
    
    
    /*
     *
     */
    bool isFinished(){return _finished;}
    
    /*
     * Main function for client thread
     */
    static void* handle(void* arg);
    
    
    void synchronize(MetaFile file);
    
    //sockaddr* getSockAddr(){return _serverInfo->ai_addr;}
    
    //string getHost(){return _hosts.front();}
    void addHost(string addr, string port, bool permanent);
    
private:
    //Rule of three
    Client(Client const& other);
    Client operator=(Client const& other);
    
    void sessionHandler(Host h);
    void startSession(sockaddr servAddr);
    void endSession(sockaddr servAddr);
    
    bool handshakeHandlerV1(sockaddr servAddr);
    bool handshakeHandlerV2(sockaddr servAddr);
    
    
    bool terminateHandler(sockaddr servAddr);
    void metafileHandler(sockaddr servAddr, MetaFile** diff);
    void fileTransfer(sockaddr servAddr, MetaFile* diff);
    bool completeFileTransfer(Message* msg, bool first);

};

#endif
