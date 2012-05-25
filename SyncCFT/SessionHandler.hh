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
#define SESSION_TIMEOUT 60 // Seconds
#define SESSIONHANDLER_MAX_TRANSFERS 1

using namespace std;

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
    //SessionHandler(){};
    SessionHandler(Server* server, Transceiver* trns, uint8_t id, uint32_t seqnum, unsigned char* sessionKey);
    ~SessionHandler();
    
    
    /*
     * Creates a session key from two nonces and the secret key
     * @param nonce1 First 16-byte random nonce
     * @param nonce2 Second 16-byte random nonce
     * @param secretKey Stored  64-byte secret key
     */ 
    void createSessionKey(unsigned char* nonce1, unsigned char* nonce2, unsigned char* secretKey);
    const unsigned char* getSessionKey() {return _sessionKey;}

    bool newMessage(const Message* msg);
    
    /*
     * Check if connection has been idle too long
     * @return True if expired
     */
    bool isExpired();
    
private:
    // Rule of three
    SessionHandler(SessionHandler const& other);
    SessionHandler& operator=(SessionHandler const& other);
    
    bool isValidSource(const Message* msg);
    bool isValidMessage(const Message* msg);
    bool isTransferring(Element* file);
    
    void descrHandler(const Message* msg);
    void getHandler(const Message* msg);
    void fileHandler(const Message* msg);
    bool quitHandler(const Message* msg);
    
    void sendNack(const Message* msg);
    void sendAck(const Message* msg);
    
    int getFreeFlow();
    
    bool parseGet(const Message* msg, Element* file, uint32_t* chunkBegin, uint32_t* chunkEnd);
};

#endif
