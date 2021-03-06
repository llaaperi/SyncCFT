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
     * @param addr1 First address
     * @param Second address
     * @return true if addresses match, false if not
     */
    bool cmpAddr(const sockaddr* addr1, const sockaddr* addr2);
    
    /**
     * Compare two IPv4 address
     * @param addr1 First address
     * @param addr2 Second address
     * @return true if addresses match, false if not
     */
    bool cmpIPv4Addr(const sockaddr_in* addr1, const sockaddr_in* addr2);
    
    /**
     * Compare two IPv6 address
     * @param addr1 First address
     * @param addr2 Second address
     * @return true if addresses match, false if not
     */
    bool cmpIPv6Addr(const sockaddr_in6* addr1, const sockaddr_in6* addr2);
    
    /**
     * Find out IP address type (IPv4 or IPv6)
     * @param address Struct where address information is stored
     * @return Correct address for IP type
     */
    void* getAddr(struct sockaddr* address);
    
    /**
     * Creates an UDP socket for incoming client connections
     * @param port Port number to be used
     * @return Socket to be listened
     */
    int createUnconnectedSocket(string port);
    
    /**
     * Creates an UDP socket to a remote server
     * @param address Server address to connect
     * @param port Server port number
     * @return File descriptor to the created socket
     */
    int createConnectedSocket(string address, string port);
    
    /**
     * Receive message from a given socket
     * @param socketFd socket from which to receive message
     * @param buffer Data buffer for storing received packets
     * @param cliAddr Structure for storing received address info
     * @param timeout Timeout value in milliseconds
     * @return Number of bytes received
     */
    int receivePacket(int socketFd, char* buffer, struct sockaddr* cliAddr, unsigned int timeout);
    
    /**
     * Send message to a given socket
     * @param socketFd socket to which message is sent
     * @param data Message to be sent
     * @param length Size of the message
     * @param cliAddr Structure for storing target address info
     * @param timeout Timeout value in milliseconds
     * @return Number of bytes sent
     */
    int sendPacket(int socketFd, char* data,int length, struct sockaddr* cliAddr, unsigned int timeout);
    
    /**
     * Convert address to a string format
     * @param addr Sockaddr struct containing the address
     * @return The address as a string
     */
    string getAddrStr(const struct sockaddr* addr);
    
    /**
     * Convert port number number to a string format
     * @param addr Sockaddr struct containing the port
     * @return The port as a string
     */
    string getPortStr(const struct sockaddr* addr);
    
    /**
     * Print address in format: <IP:Port (127.0.0.1:5062)>
     * @param addr Address to be printed
     */
    void printAddress(const struct sockaddr* addr);
    
    /**
     * Print IPv4 address in format: <IP:Port (127.0.0.1:5062)>
     * @param addr Address to be printed
     */
    void printIPv4Address(struct sockaddr_in* addr);

}
    
#endif
