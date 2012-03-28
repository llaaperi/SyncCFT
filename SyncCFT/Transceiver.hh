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

class Transceiver {
    int _socket;
    struct sockaddr _cliAddr;
    
public:
    Transceiver(int socket, struct sockaddr cliAddr) : _socket(socket), _cliAddr(cliAddr){}
    ~Transceiver(){}
    
    bool send(Message* msg, int timeout);
    bool recv(Message* msg, int timeout);
    
    static bool sendMsg(int socket, Message* msg, struct sockaddr* destAddr, int timeout);
    static bool recvMsg(int socket, Message* msg, struct sockaddr* srcAddr, int timeout);
    
    const sockaddr* getAddr(){return &_cliAddr;}
    
private:
    Transceiver& operator=(Transceiver const& other);
    Transceiver(Transceiver const& other);
};

#endif
