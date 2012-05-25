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

#define MESSAGE_MTU 1440 //1500 - IP - UDP - HEADER - MAC
#define DEFAULT_VERSION 2
#define DEFAULT_WINDOW 1
#define MESSAGE_MAC_SIZE 32


using namespace std;

class Message {
    struct sockaddr _addrInfo;
    
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
    bool _hello;
    bool _quit;
    bool _begin;
    bool _end;
    
    //char _binaryHeader[HEADER_SIZE];
    char* _payload;
	unsigned char _mac[MESSAGE_MAC_SIZE];
    
public:
    Message();
    Message(Message const& other);
    ~Message();
    
    uint8_t getVersion() const {return _version;}
    uint8_t getType() const {return _type;}
    uint8_t getClientID() const {return _clientID;}
    uint8_t getChecksum() const {return _checksum;}
    uint16_t getPayloadLength() const {return _payloadLen;}
    uint16_t getWindow() const {return _window;}
    uint32_t getSeqnum() const {return _seqnum;}
    uint32_t getChunk() const {return _chunk;}
    const char* getPayload() const {return _payload;}
    const sockaddr* getAddr() const {return &_addrInfo;}
	const unsigned char* getMAC(){return _mac;}
    
    bool isHello() const {return _hello;}
    bool isQuit() const {return _quit;}
    bool isFirst() const {return _begin;}
    bool isLast() const {return _end;}
    
    void setVersion(uint8_t v){_version = v;}
    void setType(MsgType t){_type = t;}
    void setClientID(uint8_t id){_clientID = id;}
    void setChecksum(uint8_t c){_checksum = c;}
    void setWindow(uint16_t w){_window = w;}
    void setSeqnum(uint32_t s){_seqnum = s;}
    void setChunk(uint32_t c){_chunk = c;}
	
    
    void setPayload(const char* payload, int length);
    
    void setAddr(struct sockaddr addr){_addrInfo = addr;}
    
    void setHello(bool b){_hello = b;}
    void setQuit(bool b){_quit = b;}
    void setFirst(bool b){_begin = b;}
    void setLast(bool b){_end = b;}
    
    void incrSeqnum(){_seqnum++;}
    
    void init(uint8_t version, uint8_t type);
    void initHeader(uint8_t version, uint8_t type);
    void clear();
    void clearPayload();
    void parseToBytes(char* buffer) const;
    
    /*
     * Parse message from byte array.
     * Return true if message was parsed succesfully, otherwise return false
     */
    bool parseFromBytes(const char* buffer, int len);
    
    
    static bool compare_seqnum(Message* msg1, Message* msg2);
    
    void printBytes() const;
    void printInfo() const;

private:
    // Rule of three
    Message& operator=(Message const& other);
};

#endif
