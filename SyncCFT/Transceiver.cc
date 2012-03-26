//
//  Transceiver.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 26.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Transceiver.hh"
#include "networking.hh"


/*
 * Function sends the message to the client.
 * @return  true if message was sent successfully, 
 *          false if send failed or timeouted
 */
bool Transceiver::send(Message* msg, int timeout){
    return sendMsg(_socket, msg, &_cliAddr, timeout);
}


/*
 * Function receives message from the client.
 * @return  true if valid message was received from the client, 
 *          false if ivalid message or wrong source.
 */
bool Transceiver::recv(Message* msg, int timeout){
    
    struct sockaddr srcAddr;
    
    //Receive valid message
    if(!recvMsg(_socket, msg, &srcAddr, timeout)){
        return false;
    }
    
    //Check source
    if(!Networking::cmpAddr(&srcAddr, &_cliAddr)){
        return false;
    }
    return true;
}


/*
 *
 */
bool Transceiver::sendMsg(int socket, Message* msg, struct sockaddr* destAddr, int timeout){
    
    char sendBuffer[NETWORKING_MTU];
    
    if(msg == NULL){
        return false;
    }
    
    msg->parseToBytes(sendBuffer);
    int sentBytes = Networking::sendPacket(socket, sendBuffer, HEADER_SIZE,  destAddr, timeout);
    
    if(sentBytes < (HEADER_SIZE + msg->getPayloadLength())){
        return false;
    }
    
    return true;
}


/*
 *
 */
bool Transceiver::recvMsg(int socket, Message* msg, struct sockaddr* srcAddr, int timeout){

    char recvBuffer[NETWORKING_MTU];
    
    if(msg == NULL){
        return false;
    }
    
    int recvLen = Networking::receivePacket(socket, recvBuffer, srcAddr, timeout);
    
    // Parse message and discard invalid packets
    if(!msg->parseFromBytes(recvBuffer, recvLen)){
        return false;
    }
    
    return true;
}