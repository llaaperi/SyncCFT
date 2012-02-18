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
    bool mBeing;
    bool mEnd;
    char* mPayload;
        
public:
    Message(){}
    ~Message(){}
    
    void initHeader(char* data, int length);
    
    char* getPayload() const {return mPayload;}
    void setPayload(char* payload, int length);

private:
    // Rule of three
    Message(Message const& other);
    Message& operator=(Message const& other);
};

#endif
