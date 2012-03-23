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

#define NETWORKING_MTU 1500
#define TIMEOUT 5
#define BACKLOG 50

using namespace std;

namespace Networking {
    
    /*
     * Find out IP address type (IPv4 or IPv6)
     * @param address, Struct where address information is stored
     * @return Correct address for IP type
     */
    void* getAddr(struct sockaddr* address);
    
    /*
     * Creates an UDP socket for incoming client connections
     * @param port Port number to be used
     * @return Socket to be listened
     */
    int createUnconnectedSocket(string port);

    /*
     * Creates an UDP socket to a remote server
     * @param address Server address to connect
     * @param port Server port number
     * @return File descriptor to the created socket
     */
    int createConnectedSocket(string address, string port);
    
    /*
     * Receive message from a given socket
     * @param socketFd socket from which to receive message
     * @param buffer Data buffer for storing received packets
     * @param cliAddr Structure for storing received address info
     * @return Number of bytes received
     */
    int receivePacket(int socketFd, char* buffer, struct sockaddr* cliAddr);
    
    /*
     * Send message to a given socket
     * @param socketFd socket to which message is sent
     * @param data Message to be sent
     * @param length Size of the message
     * @param cliAddr Structure for storing target address info
     * @return Number of bytes sent
     */
    int sendPacket(int socketFd, char* data,int length, struct sockaddr* cliAddr);

}
    

#endif
