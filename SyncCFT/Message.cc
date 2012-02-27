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




// Convert message header into binary format
void Message::parseBytes() {
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
}


// Print contents of the header
void Message::print() {
    cout <<"[ ";
    for (int i = 0; i < HEADER_SIZE; i++)
        printf("%02x ", (unsigned char)mBinaryHeader[i]);
    cout << "]" << endl;
}
