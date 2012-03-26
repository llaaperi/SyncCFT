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
    
    int recvLen = Networking::receivePacket(socket, recvBuffer, srcAddr, timeout);
    
    // Parse message and discard invalid packets
    if(!msg->parseFromBytes(recvBuffer, recvLen)){
        return false;
    }
    
    return true;
}