//
//  networking.cc
//  SyncCFT
//
//  Created by Elo Matias on 18.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>


#include <string>
#include <sstream>
#include <list>
#include <iostream>

#include "networking.hh"


/**
 * Compare two network address and ports
 * @param addr1 First address
 * @param Second address
 * @return true if addresses match, false if not
 */
bool Networking::cmpAddr(const sockaddr* addr1, const sockaddr* addr2){
    
    //Check for NULL pointers
    if((addr1 == NULL) || (addr2 == NULL)){
        return false;
    }
    //Check if both are IPv4 or IPv6
    if(addr1->sa_family != addr2->sa_family){
        return false;
    }
    //Compare IPv4 addresses
    if(addr1->sa_family == AF_INET){
        return cmpIPv4Addr((sockaddr_in*)addr1, (sockaddr_in*)addr2);
    }//Compare IPv6 addresses
    else{
        return cmpIPv6Addr((sockaddr_in6*)addr1, (sockaddr_in6*)addr2);
    }
}


/**
 * Compare two IPv4 address
 * @param addr1 First address
 * @param addr2 Second address
 * @return true if addresses match, false if not
 */
bool Networking::cmpIPv4Addr(const sockaddr_in* addr1, const sockaddr_in* addr2){
    
    //Check for NULL pointers
    if((addr1 == NULL) || (addr2 == NULL)){
        return false;
    }
    //Compare address
    if(addr1->sin_addr.s_addr != addr2->sin_addr.s_addr){
        return false;
    }
    //Compare port
    if(addr1->sin_port != addr2->sin_port){
        return false;
    }
    return true;
}


/**
 * Compare two IPv6 address
 * @param addr1 First address
 * @param addr2 Second address
 * @return true if addresses match, false if not
 */
bool Networking::cmpIPv6Addr(const sockaddr_in6* addr1, const sockaddr_in6* addr2){
    
    //Check for NULL pointers
    if((addr1 == NULL) || (addr2 == NULL)){
        return false;
    }
    //Compare address
    if(!memcmp(addr1->sin6_addr.s6_addr, addr2->sin6_addr.s6_addr, 16)){
        return false;
    }
    //Compare port
    if(addr1->sin6_port != addr2->sin6_port){
        return false;
    }
    return true; 
}


/**
 * Find out IP address type (IPv4 or IPv6)
 * @param address Struct where address information is stored
 * @return Correct address for IP type
 */
void* Networking::getAddr(struct sockaddr *address) {
    if (address->sa_family == AF_INET)
        return &(((struct sockaddr_in*)address)->sin_addr); // IPv4 address
    return &(((struct sockaddr_in6*)address)->sin6_addr);   // IPv6 address
}


/**
 * Creates an UDP socket for incoming client connections
 * @param port Port number to be used
 * @return Socket to be listened
 */
int createUnconnectedSocket(string port);
int Networking::createUnconnectedSocket(string port) {
    int listenFd;
    int yes=1;
    struct addrinfo hints, *serverInfo, *tempInfo;
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE; // Allow clients from any network address
    
    // Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    int temp;
    temp = getaddrinfo(NULL, port.c_str(), &hints, &serverInfo);
    if (temp != 0) {
        perror("Running getaddrinfo failed.");
        return -1;
    }
    
    // Loop through all the results and bind the first
    for(tempInfo = serverInfo; tempInfo != NULL; tempInfo = tempInfo->ai_next) {
        if ((listenFd = socket(tempInfo->ai_family, tempInfo->ai_socktype, tempInfo->ai_protocol)) == -1)
            continue; // Skip this one
                      // Set socket options to allow re-binding
        if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("Setting socket options failed.");
            return -1;
        }
        // Bind the socket
        if (bind(listenFd, tempInfo->ai_addr, tempInfo->ai_addrlen) == -1) {
            close(listenFd);
            perror("Binding socket failed");
            continue;
        }
        break; // Done
    }
    if (tempInfo == NULL)  {
        perror("Binding client socket failed.");
        return -1;
    }
    freeaddrinfo(serverInfo); // Free used struct
    
    return listenFd;
}


/**
 * Creates an UDP socket to a remote server
 * @param address Server address to connect
 * @param port Server port number
 * @return File descriptor to the created socket
 */
int Networking::createConnectedSocket(std::string address, std::string port) {
    
    int socketFd;
    struct addrinfo hints, *serverInfo, *tempInfo; // Structs for storing address info
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
        // Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    int temp;
    if((temp = getaddrinfo(address.c_str(), port.c_str(), &hints, &serverInfo)) != 0) {
        perror("Getaddrinfo failed.");
		return -1;
    }
    
        // Loop through all the results and connect to the first possible
    for(tempInfo = serverInfo; tempInfo != NULL; tempInfo = tempInfo->ai_next) {
        if ((socketFd = socket(tempInfo->ai_family, tempInfo->ai_socktype, tempInfo->ai_protocol)) == -1)
            continue; // Try next one
        
            // Try to connect
        if (connect(socketFd, tempInfo->ai_addr, tempInfo->ai_addrlen) == -1) {
            close(socketFd);
            continue; // Try next one
        }
        break;
    }
    
    if (tempInfo == NULL) { // Check connect results
        perror("Failed to connect to the server");
		return -1;
    }
    freeaddrinfo(serverInfo); // Free used struct
    return socketFd;
}


/**
 * Receive message from a given socket
 * @param socketFd socket from which to receive message
 * @param buffer Data buffer for storing received packets
 * @param cliAddr Structure for storing received address info
 * @param timeout Timeout value in milliseconds
 * @return Number of bytes received
 */
int Networking::receivePacket(int socketFd, char* buffer, struct sockaddr* cliAddr, unsigned int timeout) {
     int j, numBytes, maxFd;
     
     // Struct for setting responce timeout
     struct timeval tOut; 
     tOut.tv_sec = timeout / 1000; // Sets server timeout to 5 sec  
     tOut.tv_usec = (int)(timeout - (tOut.tv_sec * 1000)) * 1000;
     
     fd_set readSet; // Set for storing socket information
     
     FD_ZERO(&readSet); // Zero all values
     FD_SET(socketFd, &readSet); // Add fd to the main set
     maxFd = socketFd + 1;
     
     // Wait until there is data to receive or connection times out
     if ((j = select(maxFd, &readSet, NULL, NULL, &tOut)) == -1) // Triggered by incoming data or timeout
         perror("Receive select error");
     
     // Handling of timeout
     else if (j == 0) {
         //perror("Connection timed out.");
         return 0;
     }
     if (FD_ISSET(socketFd, &readSet)) { // Check if fd is active         
         int flags = 0;
         int cliLen = sizeof(*cliAddr);
         if ((numBytes = (int)recvfrom(socketFd, buffer, (size_t)NETWORKING_MTU, flags, cliAddr, (socklen_t*)&cliLen)) <= 0) {
             perror("Receiving error.");
             return -1;
         }
         return numBytes;
     }
     return -1;
 }


/**
 * Send message to a given socket
 * @param socketFd socket to which message is sent
 * @param data Message to be sent
 * @param length Size of the message
 * @param cliAddr Structure for storing target address info
 * @param timeout Timeout value in milliseconds
 * @return Number of bytes sent
 */
int Networking::sendPacket(int socketFd, char* data,int length, struct sockaddr* cliAddr, unsigned int timeout) {
    
    int j, maxFd;
    
    // Struct for setting responce timeout
    struct timeval tOut; 
    tOut.tv_sec = timeout / 1000; // Sets server timeout to 5 sec  
    tOut.tv_usec = (int)(timeout - (tOut.tv_sec * 1000)) * 1000;
    
    fd_set writeSet; // Set for storing socket information
    
    FD_ZERO(&writeSet); // Zero all values
    FD_SET(socketFd, &writeSet); // Add fd to the main set
    maxFd = socketFd + 1;
    
    // Wait until you are able to write to socket
    if ((j = select(maxFd, NULL, &writeSet, NULL, &tOut)) == -1) // Triggered by available socket or timeout
        perror("Select error");
    
    // Handling of timeout
    else if (j == 0) {
        perror("Connection timed out.");
        return -1;
    }
    
    if (FD_ISSET(socketFd, &writeSet)) { // Check if fd is active
        // Send request to the server
        return (int)sendto(socketFd, data, length, 0, cliAddr, sizeof(*cliAddr));
    }
    return -1;
}


/**
 * Convert address to string
 * @param addr Address to be converted
 * @return Address as a string
 */
string Networking::getAddrStr(const struct sockaddr* addr){

    //Check for NULL pointer
    if(addr == NULL){
        return "";
    }
    //Print IPv4 addresses
    if(addr->sa_family == AF_INET){
        
        long ip = ((sockaddr_in*)addr)->sin_addr.s_addr;
        stringstream addrStrm;
        addrStrm << (ip & 0xFF) << "." << ((ip >> 8) & 0xFF) << "." << ((ip >> 16) & 0xFF) << "." << ((ip >> 24) & 0xFF);
        return addrStrm.str();
        
    }//Print IPv6 addresses
    else{
        cout << "Not implemented IPv6 getStr." << endl;
    }
    return "";
}


/**
 * Convert port number number to a string format
 * @param addr Sockaddr struct containing the port
 * @return The port as a string
 */
string Networking::getPortStr(const struct sockaddr* addr){
    
    //Check for NULL pointer
    if(addr == NULL){
        return "";
    }
    //Print IPv4 addresses
    if(addr->sa_family == AF_INET){
        
        sockaddr_in* addr_in = (sockaddr_in*)addr;
        unsigned int port = ((addr_in->sin_port & 0xFF) << 8) | ((addr_in->sin_port) >> 8);
        stringstream portStrm;
        portStrm << port;
        return portStrm.str();
        
    }//Print IPv6 addresses
    else{
        cout << "Not implemented IPv6 getStr." << endl;
    }
    return "";
}


/**
 * Print address in format: <IP:Port (127.0.0.1:5062)>
 * @param addr Address to be printed
 */
void Networking::printAddress(const struct sockaddr* addr){
    
    //Check for NULL pointer
    if(addr == NULL){
        return;
    }
    //Print IPv4 addresses
    if(addr->sa_family == AF_INET){
        printIPv4Address((sockaddr_in*)addr);
    }//Print IPv6 addresses
    else{
        cout << "Not implemented IPv6 print." << endl;
    }
}


/**
 * Print IPv4 address in format: <IP:Port (127.0.0.1:5062)>
 * @param addr Address to be printed
 */
void Networking::printIPv4Address(struct sockaddr_in* addr){
    
    long ip = addr->sin_addr.s_addr;
    unsigned int port = ((addr->sin_port & 0xFF) << 8) | ((addr->sin_port) >> 8);
    
    //Print ip (hton) and port
    cout << (ip & 0xFF) << "." << ((ip >> 8) & 0xFF) << "." << ((ip >> 16) & 0xFF) << "." << ((ip >> 24) & 0xFF) << ":" << port;
}
