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
    _payload(0) {
    
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



int Message::parseFromBytes(const char* buffer, int len){

    if(len < HEADER_SIZE){
        return -1;
    }
    
    _version = ((buffer[0] & 0xE0) >> 5);   //3 MSBs
    
    //Flags
    if(buffer[0] & 0x01){
        _begin = true;
    }
    if(buffer[0] & 0x02){
        _end = true;
    }
    
    _type = buffer[1];
    _clientID = buffer[2];
    _checksum = buffer[3];
    _length = ((buffer[4] & 0xFF) << 8);
    _length |= (buffer[5] & 0xFF);
    _window = ((buffer[6] & 0xFF) << 8);
    _window |= (buffer[7] & 0xFF);
    _seqnum = ((buffer[8] & 0xFF) << 24);
    _seqnum |= ((buffer[9] & 0xFF) << 16);
    _seqnum |= ((buffer[10] & 0xFF) << 8);
    _seqnum |= (buffer[11] & 0xFF);
    _chunk = ((buffer[12] & 0xFF) << 24);
    _chunk |= ((buffer[13] & 0xFF) << 16);
    _chunk |= ((buffer[14] & 0xFF) << 8);
    _chunk |= (buffer[15] & 0xFF);
    
    return 0;
}


// Convert message header into binary format
void Message::parseToBytes(char* buffer) {
    
    //Init memory
    memset(buffer, 0, HEADER_SIZE);
    
    // Version and flags    
	buffer[0] = ((_version & 0x07) << 5) | (_begin & 0x01) | (_end & 0x02);
    // Type
	buffer[1] =  (_type & 0xFF);
    // Client ID
	buffer[2] =  (_clientID & 0xFF);
    // Checksum
	buffer[3] =  (_checksum & 0xFF);
    // Payload length
	buffer[4] =  (_length >> 8) & 0xFF;
	buffer[5] =  (_length & 0xFF);
    // Window size
	buffer[6] =  (_window >> 8) & 0xFF;
	buffer[7] =  (_window & 0xFF);
    // Sequence number
	buffer[8] =  (_seqnum >> 24) & 0xFF;
	buffer[9] =  (_seqnum >> 16) & 0xFF;
	buffer[10] = (_seqnum >> 8) & 0xFF;
	buffer[11] = (_seqnum & 0xFF);
    // Chunk number
    buffer[12] = (_chunk >> 24) & 0xFF;
	buffer[13] = (_chunk >> 16) & 0xFF;
	buffer[14] = (_chunk >> 8) & 0xFF;
	buffer[15] = (_chunk & 0xFF);
}


// Print contents of the header
void Message::printBytes() {
    
    char buffer[16];
    parseToBytes(buffer);
    cout <<"[ ";
    for (int i = 0; i < HEADER_SIZE; i++)
        printf("%02x ", (unsigned char)buffer[i]);
    cout << "]" << endl;
}


void Message::printInfo(){
    
    cout << "Message header:" << endl;
    cout << "Version = " << (unsigned int)getVersion() << endl;
    cout << "Type = " << (unsigned int)getType() << endl;
    cout << "ClientID = " << (unsigned int)getClientID() << endl;
    cout << "Checksum = " << (unsigned int)getChecksum() << endl;
    cout << "Length = " << getLength() << endl;
    cout << "WindowSize = " << getWindow() << endl;
    cout << "Seqnum = " << getSeqnum() << endl;
    cout << "Chunk = " << getChunk() << endl;
    
    cout << "Flags: " << endl;
    cout << "Begin = " << (isFirst()?"true":"false") << endl;
    cout << "End = " << (isLast()?"true":"false") << endl;
}
