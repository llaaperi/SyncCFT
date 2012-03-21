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
#include <stdexcept>

using namespace std;

class Client {
    //Transceiver _transceiver;
    pthread_t _thread;
    string _port;
    bool _running;
    int _socket;
    
public:
    
    Client(list<string>& hosts, string port) throw(invalid_argument, runtime_error);
    ~Client() {}
    
    /*
     * Start thread for handling clients
     */
    void start();
    
    /*
     * Stop thread
     */
    void stop();
    
    /*
     * Main function for client thread
     */
    static void* handle(void* arg);
    
    void synchronize(MetaFile file);
    
    
private:
    //Rule of three
    Client(Client const& other);
    Client operator=(Client const& other);
};

#endif
