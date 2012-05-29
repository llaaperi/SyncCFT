//
//  Transceiver.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Transceiver_hh
#define SyncCFT_Transceiver_hh

#include <list>

#include "Message.hh"

using namespace std;


class Transceiver {
    int _socket;
    struct sockaddr _cliAddr;
    const unsigned char* _key;
    int _version;
    
public:
    /**
     * Constructor
     * @param socket Used UDP socket
     * @param cliAddr Struct containing required address info
     * @param key Session key for authentication
     * @param version Protocol version
     */
    Transceiver(int socket, struct sockaddr cliAddr, const unsigned char* key, int version);
    ~Transceiver(){}
    
    /**
     * Function for sending messages
     * @param msg Message to be sent
     * @param timeout Time in milliseconds
     * @return True if message was sent successfully, 
     *  false if send failed or timeouted
     */
    bool send(Message* msg, int timeout);
    
    /**
     * Function for receiving messages
     * @param msg Save received message here
     * @param timeout Time in milliseconds
     * @return  True if valid message was received, 
     *  false if invalid message or wrong source.
     */
    bool recv(Message* msg, int timeout);
    
    /**
     * Static version of send message
     * @param socket UDP socket to be used
     * @param msg Message to be sent
     * @param destAddr Destination address
     * @param timeout Time in milliseconds
     * @return True if message was sent successfully, 
     *  false if send failed or timeouted
     */
    static bool sendMsg(int socket, Message* msg, struct sockaddr* destAddr, int timeout);
    
    /**
     * Static version of receive message
     * @param socket UDP socket to be used
     * @param msg Save received message here
     * @param srcAddr Source address
     * @param timeout Time in milliseconds
     * @return  True if valid message was received, 
     *  false if invalid message or wrong source.
     */
    static bool recvMsg(int socket, Message* msg, struct sockaddr* srcAddr, int timeout);
    
    const sockaddr* getAddr(){return &_cliAddr;}
    int getVersion(){return _version;}
    
private:
    Transceiver& operator=(Transceiver const& other);
    Transceiver(Transceiver const& other);
};

#endif
