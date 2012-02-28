//
//  Message.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Message_hh
#define SyncCFT_Message_hh

#include <arpa/inet.h>
#include <netdb.h>

#define HEADER_SIZE 16
enum MsgType {TYPE_ACK, TYPE_HELLO, TYPE_DESCR, TYPE_DIFF, TYPE_GET, TYPE_FILE, TYPE_QUIT, TYPE_NACK};

using namespace std;

class Message {
    struct sockaddr_in mAddrInfo;
    
    // Header
    uint8_t mVersion;
    uint8_t mType;
    uint8_t mClientID;
    uint8_t mChecksum;
    uint16_t mLength;
    uint16_t mWindow;
    uint32_t mSeqnum;
    uint32_t mChunk;
    // Flags
    bool mBegin;
    bool mEnd;
    char* mPayload;
    
    char mBinaryHeader[HEADER_SIZE];
        
public:
    Message();
    ~Message(){}
    
    void initHeader(uint8_t version, uint8_t type, uint8_t clientID, uint8_t checksum, uint16_t length, uint16_t window, uint32_t seqnum, uint32_t chunk);
    
    char* getPayload() const {return mPayload;}
    void setPayload(char* payload, int length) {mPayload = payload; mLength = length;}
    
    char* parseBytes();
    
    void print();

private:
    // Rule of three
    Message(Message const& other);
    Message& operator=(Message const& other);
};

#endif
