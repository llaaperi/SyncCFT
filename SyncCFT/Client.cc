//
//  Client.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Client.hh"


/*
 * Start thread for handling clients
 */
void Client::start(void)
{
    pthread_create(&_thread, NULL, handle, this);
    pthread_detach(_thread);
}

/*
 * Main function for client thread
 */
void* Client::handle(void* arg)
{
    Client* handler = (Client*)arg;
    
    cout << "Client handler" << endl;
}
