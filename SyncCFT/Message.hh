//
//  Message.hh
//  SyncCFT
//
//  Created by Elo Matias on 2/18/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Message_hh
#define SyncCFT_Message_hh

#include "utilities.hh"

#include <arpa/inet.h>
#include <netdb.h>

#define HEADER_SIZE 16
enum MsgType {TYPE_ACK, TYPE_HELLO, TYPE_DESCR, TYPE_DIFF, TYPE_GET, TYPE_FILE, TYPE_QUIT, TYPE_NACK};

using namespace std;

class Message {
    struct sockaddr_in _addrInfo;
    
    // Header
    uint8_t _version;
    uint8_t _type;
    uint8_t _clientID;
    uint8_t _checksum;
    uint16_t _length;
    uint16_t _window;
    uint32_t _seqnum;
    uint32_t _chunk;
    // Flags
    bool _begin;
    bool _end;
    char* _payload;
    
    char _binaryHeader[HEADER_SIZE];
        
public:
    Message();
    ~Message(){}
    
    void initHeader(uint8_t version, uint8_t type, uint8_t clientID, uint8_t checksum, uint16_t length, uint16_t window, uint32_t seqnum, uint32_t chunk);
    
    char* getPayload() const {return _payload;}
    void setPayload(char* payload, int length) {_payload = payload; _length = length;}
    
    char* parseBytes();
    
    void print();

private:
    // Rule of three
    Message(Message const& other);
    Message& operator=(Message const& other);
};

#endif
