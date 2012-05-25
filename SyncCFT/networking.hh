//
//  networking.hh
//  SyncCFT
//
//  Created by Elo Matias on 18.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef RadioStreamer_networking_hh
#define RadioStreamer_networking_hh

#include <string>
#include <list>

#include <netinet/in.h>
#include <netdb.h>

#define NETWORKING_MTU 1500
#define BACKLOG 50

using namespace std;

namespace Networking {
    
    
    /**
     * Compare two network address and ports
     * @addr1 First address
     * @addr2 Second address
     * @return true if addresses match, false if not
     **/
    bool cmpAddr(const sockaddr* addr1, const sockaddr* addr2);
    
    
    
    /**
     * Compare two IPv4 address
     * @addr1 First address
     * @addr2 Second address
     * @return true if addresses match, false if not
     **/
    bool cmpIPv4Addr(const sockaddr_in* addr1, const sockaddr_in* addr2);
    
    
    
    /**
     * Compare two IPv6 address
     * @addr1 First address
     * @addr2 Second address
     * @return true if addresses match, false if not
     **/
    bool cmpIPv6Addr(const sockaddr_in6* addr1, const sockaddr_in6* addr2);
    
    
    
    /**
     * Find out IP address type (IPv4 or IPv6)
     * @address Struct where address information is stored
     * @return Correct address for IP type
     **/
    void* getAddr(struct sockaddr* address);
    
    
    
    /**
     * Creates an UDP socket for incoming client connections
     * @port Port number to be used
     * @return Socket to be listened
     **/
    int createUnconnectedSocket(string port);

    
    
    /**
     * Creates an UDP socket to a remote server
     * @address Server address to connect
     * @port Server port number
     * @return File descriptor to the created socket
     **/
    int createConnectedSocket(string address, string port);
    
    
    
    /**
     * Receive message from a given socket
     * @socketFd socket from which to receive message
     * @buffer Data buffer for storing received packets
     * @cliAddr Structure for storing received address info
     * @timeout Timeout value in seconds
     * @return Number of bytes received
     **/
    int receivePacket(int socketFd, char* buffer, struct sockaddr* cliAddr, unsigned int timeout);
    
    
    
    /**
     * Send message to a given socket
     * @socketFd socket to which message is sent
     * @data Message to be sent
     * @length Size of the message
     * @cliAddr Structure for storing target address info
     * @timeout Timeout value in seconds
     * @return Number of bytes sent
     **/
    int sendPacket(int socketFd, char* data,int length, struct sockaddr* cliAddr, unsigned int timeout);
    
    
    /*
     *
     */
    string getAddrStr(const struct sockaddr* addr);
    
    
    /*
     *
     */
    string getPortStr(const struct sockaddr* addr);
    
    
    /**
     * Print address IP:Port (127.0.0.1:5062)
     * @addr Address to be printed
     **/
    void printAddress(const struct sockaddr* addr);

}
    

#endif
