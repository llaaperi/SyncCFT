//
//  Transceiver.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Transceiver_hh
#define SyncCFT_Transceiver_hh

#include <list>

#include "Message.hh"

using namespace std;

/**
 * TODO: - Class has to support multiple parallel sessions
 *       - Class has to perform message authentication
 *
 * IDEAS: - Create an individual receive buffer for each session (client ID,
 *          struct sockaddr) -> if received packet doesn't belong to own
 *          session, save it to the correct receive buffer
 *
 **/

class Transceiver {
    int _socket;
    struct sockaddr _cliAddr;
    const unsigned char* _key;
    int _version;
    
public:
    Transceiver(int socket, struct sockaddr cliAddr, const unsigned char* key, int version);
    ~Transceiver(){}
    
    bool send(Message* msg, int timeout);
    bool recv(Message* msg, int timeout);
    
    static bool sendMsg(int socket, Message* msg, struct sockaddr* destAddr, int timeout);
    static bool recvMsg(int socket, Message* msg, struct sockaddr* srcAddr, int timeout);
    
    const sockaddr* getAddr(){return &_cliAddr;}
    
    int getVersion(){return _version;}
    
private:
    Transceiver& operator=(Transceiver const& other);
    Transceiver(Transceiver const& other);
};

#endif
