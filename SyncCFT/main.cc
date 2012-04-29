//
//  main.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <getopt.h>

#include "Metafile.hh"
#include "Message.hh"
#include "Client.hh"
#include "Server.hh"
#include "utilities.hh"

#define HELP "Usage: synccft [-c <client port>] [-s <server port>] [-p <p>] [-q <q>] [-k] <hosts>"

int main (int argc, const char * argv[])
{
    std::cout << "*** SyncCTF launched ***" << std::endl;
    
    //Default values
    string cport = "5063";
    string sport = "5062";
    string p = "-1,0";  //Init as not set
    string q = "-1.0";  //Init as not set
    list<string> hosts;
    bool newSecret = false;
    
	// Long versions of command line parameters
    static struct option long_options[] = {
		{"cport", 	required_argument, 	0, 'c'},
        {"sport", 	required_argument, 	0, 's'},
        {"success", required_argument, 	0, 'p'},
        {"fail", 	required_argument, 	0, 'q'},
        {"secret", 	no_argument, 		0, 'k'},
        {"help", 	no_argument, 		0, 'h'},
        {0, 0, 0, 0}
    };
	
    int c;
    opterr = 0;
    // Use get_opt to parse command line parameters
	while ((c = getopt_long (argc, (char **)argv, "hc:s:p:q:", long_options, NULL)) != -1){
        switch (c)
        {
            case 'c': // Client port
                cport = optarg;
                cout << "Client port: " << cport << endl;
                break;
            case 's': // Server port
                sport = optarg;
                cout << "Server port: " << sport << endl;
                break;
            case 'p': // Markov probability
                p = optarg;
                cout << "p: " << p << endl;
                break;
            case 'q': // Markov probability
                q = optarg;
                cout << "q: " << q << endl;
                break;
            case 'k': // Generate new secret key
                newSecret = true;
                cout << "Generating new secret key" << endl;
                break;
            case 'h': // Help
                cout << HELP << endl;
                break;
            case '?': // Help
                cout << HELP << endl;
                return 0;
            default:
                cout << "Default" << endl;
                return 0;
        }
    }
	for(int index = optind; index < argc; index++) {
        hosts.push_back(argv[index]);
        cout << "Host: " << argv[index] << endl;
    }
    
    //Start in server mode if no hosts are given
    if(hosts.empty()) {
        cout << "No remote hosts defined." << endl;
        cout << "Starting in server mode." << endl;
    }
    
    // Load a secret key
    unsigned char secretKey[512];
    const char* fName = NULL;
    if (!newSecret) {
        fName = DEFAULT_KEYFILE;
    }
    Utilities::getSecretKey(secretKey, 512, fName);
    
    //Init markov process
    Utilities::initMarkov(p, q);
    
    //Print folder info
    MetaFile mFile(METAFILE);
    mFile.print();
    
    //bool startClient = false;
    bool startServer = true;
    
    // Start client first
    Client* clientHandler = NULL;
    if(!hosts.empty()){
        try {
            clientHandler = new Client(hosts, cport, sport);
            clientHandler->start();
        } catch (...) {
            cout << "Creating client handler failed." << endl;
            return 0;
        }
    }
    
    Utilities::isPacketLost();
    
    // Pass reference to client object to the server
    // Start client first
    Server* serverHandler = NULL;
    if(startServer){
        try {
            serverHandler = new Server(clientHandler, sport);
            serverHandler->start();
        } catch (...) {
            cout << "Creating server handler failed." << endl;
            return 0;
        }
    }
        
    while(true){
        sleep(1);
        //break;
    }
    
    serverHandler->stop();
    clientHandler->stop();
    
    cout << "SyncCFT terminated" << endl;
    
    return 0;
}

