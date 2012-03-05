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
    //Transceiver mTransceiver;
    pthread_t _thread;

public:
    
    Server(){}
    ~Server(){}
    
    /*
     * Start thread for handling server messages
     */
    void start();
    
    /*
     * Stop thread
     */
    void stop();
    
    /*
     * Main function for server thread
     */
    static void* handle(void* arg);

private:
    //Rule of three
    Server(Server const& other);
    Server operator=(Server const& other);
};

#endif
