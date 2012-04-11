//
//  SessionHandler.hh
//  SyncCFT
//
//  Created by Elo Matias on 3/23/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_SessionHandler_hh
#define SyncCFT_SessionHandler_hh

#include "networking.hh"
#include "Transceiver.hh"
#include "FileTransfer.hh"
#include "Message.hh"


#define SESSIONHANDLER_MAX_TRANSFERS 1

using namespace std;

class SessionHandler {
    
    uint8_t _id;
    uint32_t _seqnum;
    Transceiver* _trns;
    FileTransfer* _fFlows[SESSIONHANDLER_MAX_TRANSFERS];
    
    unsigned char _sessionKey[32];
    
public:
    SessionHandler(){};
    SessionHandler(int socket, struct sockaddr* cliAddr, uint8_t id, uint32_t seqnum);
    ~SessionHandler();
    
    /*
     * Creates a session key from two nonces and the secret key
     * @param nonce1 First 16-byte random nonce
     * @param nonce2 Second 16-byte random nonce
     * @param secretKey Stored  64-byte secret key
     */ 
    void createSessionKey(unsigned char* nonce1, unsigned char* nonce2, unsigned char* secretKey);
    const unsigned char* getSessionKey() {return _sessionKey;}

    void newMessage(Message* msg);
    
private:
    // Rule of three
    SessionHandler(SessionHandler const& other);
    SessionHandler& operator=(SessionHandler const& other);
    
    bool isValidSource(Message* msg);
    bool isValidMessage(Message* msg);
    
    void descrHandler(Message* msg);
    void getHandler(Message* msg);
    void fileTransfer(Message* msg);
    
    int getFreeFlow();
};

#endif
