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

/**
 * Constructor
 * @param socket Used UDP socket
 * @param cliAddr Struct containing required address info
 * @param key Session key for authentication
 * @param version Protocol version
 */
Transceiver::Transceiver(int socket, struct sockaddr cliAddr, const unsigned char* key, int version) : _socket(socket), _cliAddr(cliAddr), _key(key){
    
    if(version <= 0){
        _version = 2;
    }else{
        _version = version;
    }

}


/**
 * Function for sending messages
 * @param msg Message to be sent
 * @param timeout Time in milliseconds
 * @return True if message was sent successfully, 
 *  false if send failed or timeouted
 */
bool Transceiver::send(Message* msg, int timeout){
    
    //Add valid MAC for the message is version 2 is used
    if(_version == 2 && !msg->isHello()){
        
        char buffer[NETWORKING_MTU];
        memset(buffer, 0, NETWORKING_MTU);
        
        msg->parseToBytes(buffer);
        int pktLen = HEADER_SIZE + msg->getPayloadLength();
        memcpy(&buffer[pktLen], _key, MESSAGE_MAC_SIZE);    //Copy key to the mac
        
        unsigned char hash[HASH_LENGTH];
        Utilities::SHA256Hash(hash, (unsigned char*)buffer, pktLen + MESSAGE_MAC_SIZE);
        msg->setMac(hash);
		//msg->printInfo();
    }
     
    return sendMsg(_socket, msg, &_cliAddr, timeout);
}


/**
 * Function for receiving messages
 * @param msg Save received message here
 * @param timeout Time in milliseconds
 * @return  True if valid message was received, 
 *  false if invalid message or wrong source.
 */
bool Transceiver::recv(Message* msg, int timeout){
    
    struct sockaddr srcAddr;
    
    //Receive valid message
    if(!recvMsg(_socket, msg, &srcAddr, timeout)){
        cout << "[TRANS] Invalid message" << endl;
        return false;
    }
    //Check source
    if(!Networking::cmpAddr(&srcAddr, &_cliAddr)){
        cout << "[TRANS] Invalid source" << endl;
        return false;
    }
    
    //Check MAC
    if(_version == 2 && !msg->isHello()){
        
        char buffer[NETWORKING_MTU];
        memset(buffer, 0, NETWORKING_MTU);
        
        msg->parseToBytes(buffer);
        int pktLen = HEADER_SIZE + msg->getPayloadLength();
        memcpy(&buffer[pktLen], _key, MESSAGE_MAC_SIZE);    //Copy key to the mac
        
        unsigned char hash[HASH_LENGTH];
        Utilities::SHA256Hash(hash, (unsigned char*)buffer, pktLen + MESSAGE_MAC_SIZE);
        
        if(memcmp(hash, msg->getMAC(), MESSAGE_MAC_SIZE)){
            cout << "[TRANS] Invalid MAC" << endl;
			return false;
        }
    }
    
    return true;
}


/**
 * Static version of send message
 * @param socket UDP socket to be used
 * @param msg Message to be sent
 * @param destAddr Destination address
 * @param timeout Time in milliseconds
 * @return True if message was sent successfully, 
 *  false if send failed or timeouted
 */
bool Transceiver::sendMsg(int socket, Message* msg, struct sockaddr* destAddr, int timeout){
    
    char sendBuffer[NETWORKING_MTU];
    
    if(msg == NULL){
        return false;
    }
    
    msg->parseToBytes(sendBuffer);
    int pktLen = HEADER_SIZE + msg->getPayloadLength();
    if(msg->getVersion() == 2){
        pktLen += MESSAGE_MAC_SIZE;
    }
    
    //msg->printInfo();
    
    int sentBytes = Networking::sendPacket(socket, sendBuffer, pktLen,  destAddr, timeout);
    
    if(sentBytes < pktLen){
        return false;
    }
    
    return true;
}


/**
 * Static version of receive message
 * @param socket UDP socket to be used
 * @param msg Save received message here
 * @param srcAddr Source address
 * @param timeout Time in milliseconds
 * @return  True if valid message was received, 
 *  false if invalid message or wrong source.
 */
bool Transceiver::recvMsg(int socket, Message* msg, struct sockaddr* srcAddr, int timeout){
    
    char recvBuffer[NETWORKING_MTU];
    
    if(msg == NULL){
        return false;
    }
    
    bool first = true;
    int recvLen = 0;
    do{
        
        if(!first){
            cout << "Markov process packet loss" << endl;
        }
        first = false;
        
        recvLen = Networking::receivePacket(socket, recvBuffer, srcAddr, timeout);
        
        if(recvLen <= 0){
            return false;
        }
    
    }while(Utilities::isPacketLost());
    
    msg->setAddr(*srcAddr);
    
    // Parse message and discard invalid packets
    if(!msg->parseFromBytes(recvBuffer, recvLen)){
        cout << "[TRANSCEIVER] ParseFromBytes failed." << endl;
        return false;
    }
    
    return true;
}
