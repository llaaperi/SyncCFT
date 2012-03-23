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
#include "Message.hh"

using namespace std;

class SessionHandler {
    
    uint8_t _id;
    struct sockaddr _cliAddr;
    
public:
    SessionHandler(uint8_t id, sockaddr cliAddr);
    ~SessionHandler(){};
    
    void newMessage(Message* msg);
    
private:
    // Rule of three
    SessionHandler(SessionHandler const& other);
    SessionHandler& operator=(SessionHandler const& other);
    
    bool isValidSource();
};

#endif
