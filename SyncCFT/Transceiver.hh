//
//  Transceiver.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Transceiver_hh
#define SyncCFT_Transceiver_hh

#include "Message.hh"

using namespace std;

class Transceiver {
    int mSocket;

    
public:
    Transceiver(string port){}
    ~Transceiver(){}
    
    
    int send(Message msg);
    
    int receive(Message msg);
    
private:
    Transceiver& operator=(Transceiver const& other);
    Transceiver(Transceiver const& other);
};

#endif
