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


#define DEFAULT_VERSION 0
#define DEFAULT_WINDOW 1


using namespace std;

class Message {
    struct sockaddr_in _addrInfo;
    
    // Header
    uint8_t _version;
    uint8_t _type;
    uint8_t _clientID;
    uint8_t _checksum;
    uint16_t _payloadLen;
    uint16_t _window;
    uint32_t _seqnum;
    uint32_t _chunk;
    // Flags
    bool _begin;
    bool _end;
    
    //char _binaryHeader[HEADER_SIZE];
    const char* _payload;
    
public:
    Message();
    ~Message(){}
    
    uint8_t getVersion(){return _version;}
    uint8_t getType(){return _type;}
    uint8_t getClientID(){return _clientID;}
    uint8_t getChecksum(){return _checksum;}
    uint16_t getPayloadLength(){return _payloadLen;}
    uint16_t getWindow(){return _window;}
    uint32_t getSeqnum(){return _seqnum;}
    uint32_t getChunk(){return _chunk;}
    const char* getPayload() const {return _payload;}
    bool isFirst(){return _begin;}
    bool isLast(){return _end;}
    
    void setVersion(uint8_t v){_version = v;}
    void setType(MsgType t){_type = t;}
    void setClientID(uint8_t id){_clientID = id;}
    void setChecksum(uint8_t c){_checksum = c;}
    void setWindow(uint16_t w){_window = w;}
    void setSeqnum(uint32_t s){_seqnum = s;}
    void setChunk(uint32_t c){_chunk = c;};
    void setFirst(){_begin = true;}
    void setLast(){_end = true;}
    void setPayload(char* payload, int length) {_payload = payload; _payloadLen = length;}
    
    void incrSeqnum(){_seqnum++;}
    
    
    void initHeader(uint8_t type);
    void parseToBytes(char* buffer);
    
    /*
     * Parse message from byte array.
     * Return true if message was parsed succesfully, otherwise return false
     */
    bool parseFromBytes(const char* buffer, int len);
    
    void printBytes();
    void printInfo();

private:
    // Rule of three
    Message(Message const& other);
    Message& operator=(Message const& other);
};

#endif
