//
//  SessionHandler.hh
//  SyncCFT
//
//  Created by Elo Matias on 3/23/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_SessionHandler_hh
#define SyncCFT_SessionHandler_hh

#include "utilities.hh"
#include "networking.hh"
#include "Transceiver.hh"
#include "FileTransfer.hh"
#include "Message.hh"
#include "Timer.hh"
#define SESSION_TIMEOUT 10 // Seconds
#define SESSIONHANDLER_MAX_TRANSFERS 1

using namespace std;

// Forward declaration of class Server
class Server;

class SessionHandler {
    
    Server* _server;
    uint8_t _id;
    uint32_t _seqnum;
    Transceiver* _trns;
    FileTransfer* _fFlows[SESSIONHANDLER_MAX_TRANSFERS];
    Timer _timer;
    
    unsigned char* _sessionKey; //Allocated from server and freed here
    
public:
    /**
     * Constuctor
     * @param server Pointer to Server object
     * @param trns Pointer to a Transceiver object
     * @param id Session ID
     * @param seqnum Sequence number
     * @param sessionKey Session key
     */
    SessionHandler(Server* server, Transceiver* trns, uint8_t id, uint32_t seqnum, unsigned char* sessionKey);
    ~SessionHandler();
    
    const unsigned char* getSessionKey() {return _sessionKey;}
    /**
     * Creates a 32-byte session key from two nonces and the secret key
     * @param nonce1 First 16-byte random nonce
     * @param nonce2 Second 16-byte random nonce
     * @param secretKey Stored 64-byte secret key
     */ 
    void createSessionKey(unsigned char* nonce1, unsigned char* nonce2, unsigned char* secretKey);

    /**
     * Handle received message
     * @param msg Pointer to the received message
     * @return True if message is valid
     */
    bool newMessage(const Message* msg);
    
    /**
     * Check if connection has been idle too long
     * @return True if expired
     */
    bool isExpired();
    
private:
    // Rule of three
    SessionHandler(SessionHandler const& other);
    SessionHandler& operator=(SessionHandler const& other);
    
    /**
     * Function for checking if source is valid for this session
     * @param msg Message to be checked
     * @return True if message source is valid
     */
    bool isValidSource(const Message* msg);
    
    /**
     * Function for checking if Message is valid for this session
     * @param msg Message to be checked
     * @return True if message is valid
     */
    bool isValidMessage(const Message* msg);
    
    /**
     * Check if Element file is being transferred already
     * @param file Pointer to the element
     * @return True if file is being transferred
     */
    bool isTransferring(Element* file);
    
    /**
     * Handle DESCR message
     * @param msg Pointer to the message
     */
    void descrHandler(const Message* msg);
    
    /**
     * Handle received GET message
     * @param msg Received message
     */
    void getHandler(const Message* msg);
    
    /**
     * Handle file transfer
     * @param msg Received message
     */
    void fileHandler(const Message* msg);
    
    /**
     * Handler for session termination
     * @param msg Received message
     * @return True if the session was properly terminated
     */
    bool quitHandler(const Message* msg);
    
    /**
     * Send reply ACK to a message
     * @param file Pointer to the received message
     */
    void sendAck(const Message* msg);

    /**
     * Send reply NACK to a message
     * @param file Pointer to the received message
     */
    void sendNack(const Message* msg);
    
    /**
     * Function for requesting next free client id
     * @return Free ID or -1 if no free IDs are available
     */
    int getFreeFlow();
    
    /**
     * Parse a GET message to request chunks
     * @param msg Received GET message
     * @param file Pointer to the requested element
     * @param chunkBegin First chunk
     * @param chunkEnd Last Chunk
     * @return True if requested file was found
     */
    bool parseGet(const Message* msg, Element* file, uint32_t* chunkBegin, uint32_t* chunkEnd);
};

#endif
