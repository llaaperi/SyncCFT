//
//  utilities.cc
//  RadioStreamer
//

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <string>
#include <sstream>
#include <list>
#include <iostream>

#include "networking.hh"



/*
 * Find out IP address type (IPv4 or IPv6)
 * @param address, Struct where address information is stored
 * @return Correct address for IP type
 */
void* Networking::getAddr(struct sockaddr *address) {
    if (address->sa_family == AF_INET)
        return &(((struct sockaddr_in*)address)->sin_addr); // IPv4 address
    return &(((struct sockaddr_in6*)address)->sin6_addr);   // IPv6 address
}


/*
 * Creates a socket for waiting incoming client connections
 * @param port, Port number to be used
 * @return Socket to be listened
 */
int Networking::createServerSocket(string port) {
    int listenFd;
    int yes=1;
    struct addrinfo hints, *serverInfo, *tempInfo;
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket
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
    
    // Start listening to the port
    if (listen(listenFd, BACKLOG) == -1) {
        perror("Client listen failed.");
        return -1;
    }
    return listenFd;
}


/*
 * Creates a socket for waiting incoming client connections
 * @param port, Port number to be used
 * @return Socket to be listened
 */
int Networking::createUDPSocket(string port) {
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


    // Create client TCP socket
int Networking::connectTCP(std::string address, std::string port) {
    
    int socketFd;
    struct addrinfo hints, *serverInfo, *tempInfo; // Structs for storing address info
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;
    
        // Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    int temp;
    if((temp = getaddrinfo(address.c_str(), port.c_str(), &hints, &serverInfo)) != 0) {
        perror("Getaddrinfo failed.");
		return -1;
    }
    
        // Loop through all the results and connect to the first possible
    for(tempInfo = serverInfo; tempInfo != NULL; tempInfo = tempInfo->ai_next) {
        if ((socketFd = socket(tempInfo->ai_family, tempInfo->ai_socktype, tempInfo->ai_protocol)) == -1)
            continue; // Skip this one
        
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
        //exit(1);
    }
    freeaddrinfo(serverInfo); // Free used struct
    return socketFd;
}


    // Create client socket
int Networking::connectUDP(std::string address, std::string port) {
    
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


    // Own receiving method with timeout
int Networking::receiveTCP(int socketFd, int max_len, char* reply) {
	
    int j, numBytes, maxFd;
    char buffer[NETWORKING_MTU]; // Buffer for receiving messages
    
    
        // Struct for setting responce timeout
    struct timeval timeout; 
	timeout.tv_sec = TIMEOUT; // Sets server timeout to 5 sec  
    timeout.tv_usec = 0;
    
    fd_set readSet; // Set for storing socket information
    
    FD_ZERO(&readSet); // Zero all values
    FD_SET(socketFd, &readSet); // Add fd to the main set
    maxFd = socketFd + 1;
    
        // Wait until there is data to receive or connection times out
    if ((j = select(maxFd, &readSet, NULL, NULL, &timeout)) == -1) // Triggered by incoming data or timeout
        perror("Receive select error");
    
        // Handling of timeout
    else if (j == 0) {
            //perror("Connection timed out.");
        return -1;
    }
    if (FD_ISSET(socketFd, &readSet)) { // Check if fd is active
                                        // Receive MAX_LENGTH bytes from the server
        if ((numBytes = (int)recv(socketFd, buffer, NETWORKING_MTU, 0)) <= 0) {
            perror("Receiving error.");
            return -1;
        }
		
		//fprintf(stderr, "Bytes received: %d\n", numBytes);
		
        buffer[numBytes] = '\0'; // End character to the string
		/*
        if ((reply = (char *)malloc(numBytes + 1)) == NULL) {
            perror("Malloc error.");
            return NULL;
        }
		 */
        //strcpy(reply, buffer);          
        memcpy(reply, buffer, numBytes);
		return numBytes;
    }
    return -1;
}


/*
 * Receives message from a given socket
 * @param socketFd, TCP socket from which to receive message
 * @return Received message string
 */int Networking::receiveMessage(int socketFd, string& message, struct sockaddr_in* cliAddr) {
     int j, numBytes, maxFd;
     char buffer[NETWORKING_MTU]; // Buffer for receiving messages
     message.clear();
     
     // Struct for setting responce timeout
     struct timeval timeout; 
     timeout.tv_sec = TIMEOUT; // Sets server timeout to 5 sec  
     timeout.tv_usec = 0;
     
     fd_set readSet; // Set for storing socket information
     
     FD_ZERO(&readSet); // Zero all values
     FD_SET(socketFd, &readSet); // Add fd to the main set
     maxFd = socketFd + 1;
     
     // Wait until there is data to receive or connection times out
     if ((j = select(maxFd, &readSet, NULL, NULL, &timeout)) == -1) // Triggered by incoming data or timeout
         perror("Receive select error");
     
     // Handling of timeout
     else if (j == 0) {
         perror("Connection timed out.");
         return 0;
     }
     if (FD_ISSET(socketFd, &readSet)) { // Check if fd is active
         // Receive MAX_LENGTH bytes from the server
         /*
          if ((numBytes = (int)recv(socketFd, buffer, NETWORKING_MTU, 0)) <= 0) {
          perror("Receiving error.");
          return -1;
          }
          */
         
         int flags = 0;
         int cliLen = sizeof(*cliAddr);
         if ((numBytes = (int)recvfrom(socketFd, buffer, (size_t)NETWORKING_MTU, flags, (struct sockaddr *)cliAddr, (socklen_t*)&cliLen)) <= 0) {
             perror("Receiving error.");
             return -1;
         }
         
         //buffer[numBytes] = '\0'; // End character to the string
         string returnValue(buffer, numBytes);
         message = returnValue;
         return numBytes;
     }
     return -1;
 }


/*
 * Send message to given socket
 * @param socketFd, TCP socket to send message
 * @param message, Message to be sent
 * @return Number of bytes sent
 */int Networking::sendMessage(int socketFd, string message) {
     
     int j, maxFd;
     
     // Struct for setting responce timeout
     struct timeval timeout; 
     timeout.tv_sec = 5; // Sets server timeout to 5 sec
     timeout.tv_usec = 0;
     
     fd_set writeSet; // Set for storing socket information
     
     FD_ZERO(&writeSet); // Zero all values
     FD_SET(socketFd, &writeSet); // Add fd to the main set
     maxFd = socketFd + 1;
     
     // Wait until you are able to write to socket
     if ((j = select(maxFd, NULL, &writeSet, NULL, &timeout)) == -1) // Triggered by available socket or timeout
         perror("Select error");
     
     // Handling of timeout
     else if (j == 0) {
         perror("Connection timed out.");
         return -1;
     }
     
     if (FD_ISSET(socketFd, &writeSet)) { // Check if fd is active
                                          // Send request to the server
         return (int)send(socketFd, message.c_str(), strlen(message.c_str()), 0);
     }
     return -1;
 }
