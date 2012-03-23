//
//  Message.cc
//  SyncCFT
//
//  Created by Elo Matias on 27.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Message.hh"



Message::Message() : _window(1), _chunk(0), _begin(false), _end(false)  {
    
    srand(time(NULL));     // Use time as seed for random number generator
    _seqnum = rand();

}
/*
 * Initialize all header values
 */
void Message::initHeader(uint8_t version, uint8_t type, uint8_t clientID, uint8_t checksum, uint16_t length, uint16_t window, uint32_t seqnum, uint32_t chunk) {
    _version = version;
    _type = type;
    _clientID = clientID;
    _checksum = checksum;
    _length = length;
    _window = window;
    _seqnum = seqnum;
    _chunk = chunk;
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
