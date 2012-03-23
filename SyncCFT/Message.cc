//
//  Message.cc
//  SyncCFT
//
//  Created by Elo Matias on 27.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Message.hh"



Message::Message() :
    _version(0),
    _type(0),
    _clientID(0),
    _checksum(0),
    _length(0),
    _window(0),
    _seqnum(0),
    _chunk(0),
    _begin(false),
    _end(false), 
    _payload(0) {}

/*
 * Initialize all header values
 */
void Message::initHeader(uint8_t version, uint8_t type, uint8_t clientID, uint8_t checksum, uint16_t length, uint16_t window, uint32_t seqnum, uint32_t chunk) {
    
    srand(time(NULL));     // Use time as seed for random number generator
    
    _version = version;
    _type = type;
    _clientID = clientID;
    _checksum = checksum;
    _length = length;
    _window = window;
    _seqnum = rand();
    _chunk = chunk;
}



int Message::parseFromBytes(char* buffer, int len) {

    if(len < HEADER_SIZE){
        return -1;
    }
    
    _version = ((buffer[0] & 0xE0) >> 5);   //3 MSBs
    _flags = (buffer[0] & 1F);   //5 LSBs
    _type = buffer[1];
    _clientID = buffer[2];
    _checksum = buffer[3];
    _length = (buffer[4] << 8);
    _length |= buffer[5];
    _windowSize = (buffer[6] << 8);
    _windowSize |= buffer[7];
    _seqnum = (buffer[8] << 24);
    _seqnum |= (buffer[9] << 16);
    _seqnum |= (buffer[10] << 8);
    _seqnum |= buffer[11];
    _chunk = (buffer[12] << 24);
    _chunk |= (buffer[13] << 16);
    _chunk |= (buffer[14] << 8);
    _chunk |= buffer[15];
    
}


// Convert message header into binary format
char* Message::parseBytes() {
    memset(_binaryHeader, 0, HEADER_SIZE);
    
    // Version and flags    
	_binaryHeader[0] = ((_version & 0x07) << 5) | ((_begin & 0x01) << 4) | ((_end & 0x01) << 3);
    // Type
	_binaryHeader[1] =  (_type & 0xFF);
    // Client ID
	_binaryHeader[2] =  (_clientID & 0xFF);
    // Checksum
	_binaryHeader[3] =  (_checksum & 0xFF);
    // Payload length
	_binaryHeader[4] =  (_length >> 8) & 0xFF;
	_binaryHeader[5] =  (_length & 0xFF);
    // Window size
	_binaryHeader[6] =  (_window >> 8) & 0xFF;
	_binaryHeader[7] =  (_window & 0xFF);
    // Sequence number
	_binaryHeader[8] =  (_seqnum >> 24) & 0xFF;
	_binaryHeader[9] =  (_seqnum >> 16) & 0xFF;
	_binaryHeader[10] = (_seqnum >> 8) & 0xFF;
	_binaryHeader[11] = (_seqnum & 0xFF);
    // Chunk number
    _binaryHeader[12] = (_chunk >> 24) & 0xFF;
	_binaryHeader[13] = (_chunk >> 16) & 0xFF;
	_binaryHeader[14] = (_chunk >> 8) & 0xFF;
	_binaryHeader[15] = (_chunk & 0xFF);
    
    return _binaryHeader;
}


// Print contents of the header
void Message::print() {
    cout <<"[ ";
    for (int i = 0; i < HEADER_SIZE; i++)
        printf("%02x ", (unsigned char)_binaryHeader[i]);
    cout << "]" << endl;
}
