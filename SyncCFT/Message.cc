//
//  Message.cc
//  SyncCFT
//
//  Created by Elo Matias on 27.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Message.hh"



Message::Message() : mWindow(1), mChunk(0),mBegin(false), mEnd(false)  {
    
    srand(time(NULL));     // Use time as seed for random number generator
    mSeqnum = rand();

}


Message::Message(char* buffer){
    
    
    
}


/*
 * Initialize all header values
 */
void Message::initHeader(uint8_t version, uint8_t type, uint8_t clientID, uint8_t checksum, uint16_t length, uint16_t window, uint32_t seqnum, uint32_t chunk) {
    mVersion = version;
    mType = type;
    mClientID = clientID;
    mChecksum = checksum;
    mLength = length;
    mWindow = window;
    mSeqnum = seqnum;
    mChunk = chunk;
}



int Message::parseFromBytes(char* buffer, int len){

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
    memset(mBinaryHeader, 0, HEADER_SIZE);
    
    // Version and flags    
	mBinaryHeader[0] = ((mVersion & 0x07) << 5) | ((mBegin & 0x01) << 4) | ((mEnd & 0x01) << 3);
    // Type
	mBinaryHeader[1] =  (mType & 0xFF);
    // Client ID
	mBinaryHeader[2] =  (mClientID & 0xFF);
    // Checksum
	mBinaryHeader[3] =  (mChecksum & 0xFF);
    // Payload length
	mBinaryHeader[4] =  (mLength >> 8) & 0xFF;
	mBinaryHeader[5] =  (mLength & 0xFF);
    // Window size
	mBinaryHeader[6] =  (mWindow >> 8) & 0xFF;
	mBinaryHeader[7] =  (mWindow & 0xFF);
    // Sequence number
	mBinaryHeader[8] =  (mSeqnum >> 24) & 0xFF;
	mBinaryHeader[9] =  (mSeqnum >> 16) & 0xFF;
	mBinaryHeader[10] = (mSeqnum >> 8) & 0xFF;
	mBinaryHeader[11] = (mSeqnum & 0xFF);
    // Chunk number
    mBinaryHeader[12] = (mChunk >> 24) & 0xFF;
	mBinaryHeader[13] = (mChunk >> 16) & 0xFF;
	mBinaryHeader[14] = (mChunk >> 8) & 0xFF;
	mBinaryHeader[15] = (mChunk & 0xFF);
    
    return mBinaryHeader;
}


// Print contents of the header
void Message::print() {
    cout <<"[ ";
    for (int i = 0; i < HEADER_SIZE; i++)
        printf("%02x ", (unsigned char)mBinaryHeader[i]);
    cout << "]" << endl;
}
