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
#define CLIENT_QUIT_RETRIES 1 // Max quit retries
// In seconds
#define CLIENT_BACKOFF 5    //seconds, wait if hello fails
#define CLIENT_REFRESH 10   //seconds, resync


using namespace std;

/**
 * Stores information about transfer sessions
 */
struct Host {
    string ip;
    string port;
    struct addrinfo* serverInfo;
    bool perm;  //Permanent host (from command line)
    Timer timer;    //Timer from the last resfresh
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
    const unsigned char* _secretKey;
    unsigned char* _sessionKey;

    
public:
    
    /**
     * Constuctor for a client
     * @param hosts List of clients
     * @param cport Client port
     * @param scport Server port 
     * @param version Supported protocol version. 0 supports both 1 and 2
     * @param secretKey Secret key used in authentication
     * @param mode Defines how many times the synchronization operation is repeated, 0 = infinite
     */
    Client(list<string> hosts, string cport, string sport, int version, const unsigned char* secretKey, int mode=0) throw(invalid_argument, runtime_error);
    ~Client();
    
    /**
     * Start thread for handling client messages
     */
    void start();
    
    /**
     * Stop thread
     */
    void stop();
    
    /**
     * Is client running
     * @return True if running
     */
    bool isRunning(){return _running;}
    
    /**
     * Has client finished
     * @return True if finished
     */
    bool isFinished(){return _finished;}
    
    /**
     * Main function for client thread
     */
    static void* handle(void* arg);
    
    /**
     * Add new host to the hosts list
     * @param addr Host address
     * @param port Host port
     * @param permanent Is host stored permanently
     */
    void addHost(string addr, string port, bool permanent);
    
private:
    //Rule of three
    Client(Client const& other);
    Client operator=(Client const& other);
    
    /**
     * Handles a session with a single host
     * @param h Target host
     */
    void sessionHandler(Host h);
    
    /**
     * Function tries to start session with a server.
     * @param servAddr Struct for storing server address info
     */
    void startSession(sockaddr servAddr);
    
    /**
     * Function terminates current session
     * @param servAddr Struct for storing server address info
     */
    void endSession(sockaddr servAddr);
    
    /**
     * HandshakeHandler function handles the HELLO handshake between client and
     * server using protocol version 1.
     * @param servAddr Struct for storing server address info
     * @return True if handshake was successfull and false if it failed.
     */
    bool handshakeHandlerV1(sockaddr servAddr);
    
    /**
     * HandshakeHandler function handles the HELLO handshake between client and
     * server using protocol version 2.
     * @param servAddr Struct for storing server address info
     * @return True if handshake was successfull and false if it failed.
     */
    bool handshakeHandlerV2(sockaddr servAddr);
    
    /**
     * Handles termination of the session
     * @param servAddr Struct for storing server address info
     * @return True if termination was successfull and false if it failed.
     */
    bool terminateHandler(sockaddr servAddr);
    
    /**
     * Perform metafile comparison between the client and the server
     * @param servAddr Struct for storing server address info
     * @param diff Difference between the two Metafiles
     */
    void metafileHandler(sockaddr servAddr, MetaFile** diff);
    
    /**
     * Perform file download from the server
     * //TODO: servAddr parameter not used anywhere
     * @param servAddr Struct for storing server address info
     * @param diff Metafile containing the files to transfer
     */
    void fileTransfer(sockaddr servAddr, MetaFile* diff);
    
    /**
     * Complete single file transfer
     * @msg Message to begin the transfer session from
     * @first Is the first message type FILE
     * @return Success status of the file transfer
     */
    bool completeFileTransfer(Message* msg, bool first);

};

#endif
