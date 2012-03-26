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
#include "Message.hh"

using namespace std;

class SessionHandler {
    
    uint8_t _id;
    Transceiver* _trns;
    
public:
    SessionHandler(int socket, struct sockaddr* cliAddr, uint8_t id);
    ~SessionHandler();
    
    void newMessage(Message* msg);
    
private:
    // Rule of three
    SessionHandler(SessionHandler const& other);
    SessionHandler& operator=(SessionHandler const& other);
    
    bool isValidSource();
    void descrHandler(Message* msg);
};

#endif
