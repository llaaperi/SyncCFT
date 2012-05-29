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
    
    char* _payload;
	unsigned char _mac[MESSAGE_MAC_SIZE];
    
public:
    Message();
    
    /**
     * Destructor. Frees the existing payload
     */
    ~Message();
    
    /**
     * Copy constructor. Doesn't copy payload.
     */
    Message(Message const& other);
    
    // Getters
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
    
    // Setters
    void setVersion(uint8_t v){_version = v;}
    void setType(MsgType t){_type = t;}
    void setClientID(uint8_t id){_clientID = id;}
    void setChecksum(uint8_t c){_checksum = c;}
    void setWindow(uint16_t w){_window = w;}
    void setSeqnum(uint32_t s){_seqnum = s;}
    void setChunk(uint32_t c){_chunk = c;}
    void setAddr(struct sockaddr addr){_addrInfo = addr;}
    void setHello(bool b){_hello = b;}
    void setQuit(bool b){_quit = b;}
    void setFirst(bool b){_begin = b;}
    void setLast(bool b){_end = b;}
    
    /**
     * Set message payload. Overwrites existing payload.
     * @param payload New message payload
     * @param length Payload lentgth
     */
    void setPayload(const char* payload, int length);

    bool isHello() const {return _hello;}
    bool isQuit() const {return _quit;}
    bool isFirst() const {return _begin;}
    bool isLast() const {return _end;}
    
    /**
     * Increase sequence number by one
     */
    void incrSeqnum(){_seqnum++;}
    
    /**
     * Initializes the whole message. Existing payload is freed.
     * @param version Protocol version
     * @param type Message type
     */
    void init(uint8_t version, uint8_t type);
    
    /**
     * Initialize all message header values
     * @param version Protocol version
     * @param type Message type
     */
    void initHeader(uint8_t version, uint8_t type);
    
    /**
     * Zero all header values and free allocated memory
     */
    void clear();
    
    /**
     * Clear message payload
     */
    void clearPayload();
    
    /**
     * Convert message header into binary format
     * @param buffer Memory for storing binary message
     */
    void parseToBytes(char* buffer) const;
    
    /**
     * Parses message from byte array. 
     * Memory is allocated for the possible payload and existing payload is freed.
     * @param buffer Stored message in a byte array
     * @param len Length of the message
     */
    bool parseFromBytes(const char* buffer, int len);
    
    /**
     * Compare to message sequence numbers
     * @param msg1 Message 1
     * @param msg2 Message 2
     * @return True if Message 2 sequence number > Message 1 sequence number
     */
    static bool compare_seqnum(Message* msg1, Message* msg2);
    
    /**
     * Print contents of the header in hex format
     */
    void printBytes() const;
    
    /**
     * Print text representation of the message header and payload.
     */
    void printInfo() const;

private:
    // Rule of three
    Message& operator=(Message const& other);
};

#endif
