//
//  Client.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Client_hh
#define SyncCFT_Client_hh

#include "Transceiver.hh"
#include "Metafile.hh"
#include "Message.hh"

class Client {
    Transceiver mTransceiver;
    
public:
    
    start();
    
    stop();
    
    synchronize(Metafile file);
    
};

#endif
