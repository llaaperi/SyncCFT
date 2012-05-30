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

#define SERVER_SESSION_HANDLERS 10 //Number of handlers can be defined. Maximum value is 256.

extern list<string> _serverClients; //ip:port

using namespace std;

/**
 * Struct for storing information during the client handshake
 */
struct PendingClient{
    unsigned char cNonce[16];
    unsigned char sNonce[16];
    sockaddr addr;
};

class Server {
    //Transceiver mTransceiver;
    Client* _client;
    pthread_t _thread;
    bool _running;
    string _port;
    int _socket;
    int _version;
	const unsigned char* _secretKey;
    
    list<PendingClient*> _pendingClients;
    SessionHandler* _sessionHandlers[SERVER_SESSION_HANDLERS];

public:
    
    /**
     * Constuctor for a server
     * @param clientHandler Pointer to a client handler object
     * @param port Server port 
     * @param version Supported protocol version. 0 supports both 1 and 2.
     * @param secretKey Secret key used in authentication
     */
    Server(Client* clientHandler, string port, int version, const unsigned char* secretKey) throw(invalid_argument,runtime_error);
    ~Server();
    
    /**
     * Start thread for handling server messages
     */
    void start();
    
    /**
     * Stop thread
     */
    void stop();
    
    /**
     * Main function for server thread
     * @param arg Pointer to server object
     */
    static void* handle(void* arg);
    
    /**
     * Add new sync source to the client hosts
     * @param addr Source address
     * @param port Source port
     */
    void addSource(string addr, string port);

private:
    // The rule of three
    Server(Server const& other);
    Server operator=(Server const& other);
    
    /**
     * Send a NACK message to the client
     * @param msg Message containing the required header information
     * @param clieAddr Address info to send NACK message
     */
    void replyNACK(Message* msg, sockaddr cliAddr);
    
    /**
     * Create a new session for communicating with a client
     * @param clientID Allocated client ID
     * @param cliAddr Client address info
     * @param seqnum Sequence number from which to continue communication
     * @param sessionKey Used session key
     * @param version Protocol version
     */
    void createNewSession(int clientID, sockaddr cliAddr, uint32_t seqnum, unsigned char* sessionKey, int version);
    
    /**
     * Function for requesting next free client id
     * @return Free id or -1 if no free ID's are available
     */
    int getFreeID();
    
    /**
     * Get PendingClient if handshake is already in progress
     * @param cliAddr Struct containing the client address and port
     * @return Pointer to a pending client or NULL if no handshake is in progress
     *  with this client
     */
    PendingClient* getPendingClient(sockaddr cliAddr);
    
    /**
     * Add new PendingClient
     * @param cliAddr Struct containing the client address and port
     * @return Pointer to the pending client
     */
    PendingClient* addPendingClient(sockaddr cliAddr);
    
    /**
     * Remove PendingClient
     * @param Pointer to the PendingClient being removed
     */
    void removePendingClient(PendingClient* client);
    
    /**
     * Handshake handler (VERSION 1) funcion handles new connection requests and allocates new
     * handler when sucessfull. 
     * NOT ROBUST YET, ALLOCATES RESOURCES WITHOUT CHECKING
     * @param msg Message received from client
     * @param cliAddr Struct storing the client address and port
     */
    void handshakeHandlerV1(Message* msg, sockaddr cliAddr);
    
    /**
     * Handshake handler (VERSION 2) funcion handles new connection requests and
     * allocates new handler when sucessfull. 
     * @param msg Message received from client
     * @param cliAddr Struct storing the client address and port
     */
    void handshakeHandlerV2(Message* msg, sockaddr cliAddr);
    
    /**
     * Handle HELLO messages for protocol version 2
     * @param msg Message received from client
     * @param cliAddr Struct storing the client address and port
     */
    void handshakeHandlerV2Hello(Message* msg, sockaddr cliAddr);
    
    /**
     * Handle ACK messages for protocol version 2
     * @param msg Message received from client
     * @param cliAddr Struct storing the client address and port
     */
    void handshakeHandlerV2Ack(Message* msg, sockaddr cliAddr);
    
    // DEPRECATED
    //void terminateHandler(Message* msg, sockaddr cliAddr);
};

#endif
