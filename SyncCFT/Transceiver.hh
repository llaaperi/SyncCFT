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
    int mSocket;

    
public:
    Transceiver(){}
    ~Transceiver(){}
    
    //int send(Message msg);
    //int receive(Message msg);
    
    static bool sendMsg(int socket, Message* msg, struct sockaddr* destAddr, int timeout);
    static bool recvMsg(int socket, Message* msg, struct sockaddr* srcAddr, int timeout);
    
private:
    Transceiver& operator=(Transceiver const& other);
    Transceiver(Transceiver const& other);
};

#endif
