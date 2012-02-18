//
//  networking.hh
//  RadioStreamer
//

#ifndef RadioStreamer_networking_hh
#define RadioStreamer_networking_hh

#include <string>
#include <list>

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
     * Creates a socket for waiting incoming client connections
     * @param port, Port number to be used
     * @return Socket to be listened
     */
    int createServerSocket(string port);
    
    int createUDPSocket(string port);
	
        // Connect to the shoutcast server
    int connectTCP(string address, string port);

        // Create client socket
    int connectUDP(string address, string port);
    
        // TCP receiving method with timeout
    int receiveTCP(int socketFd, int max_len, char* reply);
    
    /*
     * Receives message from a given socket
     * @param socketFd, socket from which to receive message
     * @param message, Message to be sent
     * @param cliAddr, Structure for storin client address info
     * @return Received message string
     */
    int receiveMessage(int socketFd, string& message, struct sockaddr_in* cliAddr);
    
    /*
     * Send message to given socket
     * @param socketFd, TCP socket to send message
     * @param message, Message to be sent
     * @return Number of bytes sent
     */
    int sendMessage(int socketFd, string message);
}




#endif
