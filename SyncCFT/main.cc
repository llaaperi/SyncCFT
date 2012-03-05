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

#define METAFILE ".sync.cft"
#define HELP "Usage: synccft [-t <port>] [-p <p>] [-q <q>] <hosts>"

int main (int argc, const char * argv[])
{
    std::cout << "*** SyncCTF launched ***" << std::endl;
    
    string port = "80";
    string p, q;
    list<string> hosts;
    
	// Long versions of command line parameters
    static struct option long_options[] = {
		{"port", 	required_argument, 	0, 't'},
        {"success", required_argument, 	0, 'p'},
        {"fail", 	required_argument, 	0, 'q'},
        {"help", 	no_argument, 		0, 'h'},
        {0, 0, 0, 0}
    };
	
    int c;
    opterr = 0;
    // Use get_opt to parse command line parameters
	while ((c = getopt_long (argc, (char **)argv, "ht:p:q:", long_options, NULL)) != -1){
        switch (c)
        {
            case 't':
                port = optarg;
                cout << "Port: " << port << endl;
                break;
            case 'p':
                p = optarg;
                cout << "p: " << p << endl;
                break;
            case 'q':
                q = optarg;
                cout << "q: " << q << endl;
                break;
            case 'h':
                cout << HELP << endl;
                break;
            case '?':
                cout << HELP << endl;
                return 0;
            default:
                cout << "Default" << endl;
                return 0;
        }
    }
	for (int index = optind; index < argc; index++) {
        hosts.push_back(argv[index]);
        cout << "Host: " << argv[index] << endl;
    }
    
    // Check that all necessary input parameters are given
    if (hosts.empty()) {
        cout << "Missing host address" << endl;
        cout << HELP << endl;
        return 0;
    }
    
    //Print folder info
    MetaFile mFile(METAFILE);
    mFile.print();
    
    // Start client first
    try {
        Client* clientHandler = new Client(hosts, port);
        clientHandler->start();
    } catch (...) {
        cout << "Creating client handler failed." << endl;
        return 0;
    }
    
    // Pass reference to client object to the server
    // Start client first
    try {
        Server* serverHandler = new Server;
        serverHandler->start();
    } catch (...) {
        cout << "Creating server handler failed." << endl;
        return 0;
    }
    
    return 0;
}

