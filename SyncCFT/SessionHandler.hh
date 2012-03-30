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
    
public:
    SessionHandler(int socket, struct sockaddr* cliAddr, uint8_t id, uint32_t seqnum);
    ~SessionHandler();
    
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
