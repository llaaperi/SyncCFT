//
//  Server.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Server.hh"


/*
 * Start thread for handling clients
 */
void Server::start(void)
{
    pthread_create(&_thread, NULL, handle, this);
    pthread_detach(_thread);
}

/*
 * Main function for client thread
 */
void* Server::handle(void* arg)
{
    Server* handler = (Server*)arg;
    
    cout << "Server handler" << endl;
}
