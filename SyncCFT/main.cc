//
//  main.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>

#include "Metafile.hh"
#include "Message.hh"
#include "Client.hh"
#include "Server.hh"
#include "utilities.hh"

#define HELP "Usage: synccft [-c <client port>] [-s <server port>] [-p <p>] [-q <q>] [-k] [-d <dir>] [-v <version>] <hosts>"


/**
 * Interrupt signal handler
 */
void signalHandler(int signo);
void signalHandler(int signo){
	(void)signo;
    
    // TODO: Free used resources
    
    cout << "SyncCFT closing" << endl;
    exit(0);
}

int main (int argc, const char * argv[])
{
    std::cout << "*** SyncCTF launched ***" << std::endl;
    
    //Default values
    string clientPort = "5063";
    string serverPort = "5062";
    string p = "-1.0";  //Init as not set
    string q = "-1.0";  //Init as not set
    string dir = "./Sync/";
	int version = 2; // Support protocol 2 by default
    list<string> hosts;
    bool newSecret = false;
    
	// Long versions of command line parameters
    static struct option long_options[] = {
		{"cport", 	required_argument, 	0, 'c'},
        {"sport", 	required_argument, 	0, 's'},
        {"success", required_argument, 	0, 'p'},
        {"fail", 	required_argument, 	0, 'q'},
        {"dir",     required_argument, 	0, 'd'},
		{"version", required_argument, 	0, 'v'},
        {"secret", 	no_argument, 		0, 'k'},
        {"help", 	no_argument, 		0, 'h'},
        {0, 0, 0, 0}
    };
	
    int c;
    opterr = 0;
    // Use get_opt to parse command line parameters
	while ((c = getopt_long (argc, (char **)argv, "hkd:c:s:p:q:v:", long_options, NULL)) != -1){
        switch (c)
        {
            case 'c': // Client port
                clientPort = optarg;
                cout << "Client port: " << clientPort << endl;
                break;
            case 's': // Server port
                serverPort = optarg;
                cout << "Server port: " << serverPort << endl;
                break;
            case 'p': // Markov probability
                p = optarg;
                cout << "p: " << p << endl;
                break;
            case 'q': // Markov probability
                q = optarg;
                cout << "q: " << q << endl;
                break;
            case 'd': // Sync directory
                dir = optarg;
                cout << "Directory: " << dir << endl;
                break;
            case 'k': // Generate new secret key
                newSecret = true;
                cout << "Generating new secret key" << endl;
                break;
			case 'v': // Protocol version
				version = atoi(optarg);
				if ((version < 1) || (version > 2) ) {
					version = 2;
				}
                cout << "Protocol version: " << version << endl;
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
    
    // Add signal handler
    signal (SIGINT, signalHandler);
    
    // Load a secret key
    unsigned char secretKey[64];
    const char* fName = NULL;
    if (!newSecret) {
        fName = DEFAULT_KEYFILE;
    }
    if(!Utilities::getSecretKey(secretKey, 64, fName)){
        cout << "New key generated to file <" << DEFAULT_KEYFILE << ">" << endl;
        return 0;
    }
    cout << "[MAIN] Secret key:" << endl;
    Utilities::printBytes(secretKey, 64);
    cout << endl;
    
    //Init markov process
    Utilities::initMarkov(p, q);
    
    // Check if sync directory exists 
    if(dir.at(dir.length()-1) != '/') {
        dir += "/";
    }
    _syncDir = dir;
    if (mkdir(_syncDir.c_str(), S_IRWXU)) {
        cout << "[MAIN] Unable to create dir " << _syncDir << " or it already exists" << endl;
    } else {
        cout << "[MAIN] Created dir " << _syncDir << endl;

    }
    
    //Print folder info
    MetaFile mFile(METAFILE);
    mFile.print();
    
    bool startClient = true;
    bool startServer = true;
    
    // Start client first
    Client* clientHandler = NULL;
    if(startClient){
        try {
            clientHandler = new Client(hosts, clientPort, serverPort, version, secretKey);
            clientHandler->start();
        } catch (...) {
            cout << "Creating client handler failed." << endl;
            return 0;
        }
    }
    
    // Pass reference to client object to the server
    // Start client first
    Server* serverHandler = NULL;
    if(startServer){
        try {
            serverHandler = new Server(clientHandler, serverPort, version, secretKey);
            serverHandler->start();
        } catch (...) {
            cout << "Creating server handler failed." << endl;
            return 0;
        }
    }

    // TODO: Terminate with sigint handler
    while(true){
        sleep(1);
        //break;
    }
    
    serverHandler->stop();
    clientHandler->stop();
    
    cout << "SyncCFT terminated" << endl;
    
    return 0;
}

