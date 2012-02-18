//
//  Server.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Server_hh
#define SyncCFT_Server_hh

#include "Transceiver.hh"
#include "Metafile.hh"
#include "Message.hh"

class Server {
    Transceiver mTransceiver;
    
public:
    
    start();
    
    stop();
    
};

#endif
