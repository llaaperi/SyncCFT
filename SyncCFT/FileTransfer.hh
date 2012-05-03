//
//  FileTransfer.hh
//  SyncCFT
//
//  Created by Lauri Lääperi on 30.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_FileTransfer_hh
#define SyncCFT_FileTransfer_hh

#include "networking.hh"
#include "Transceiver.hh"
#include "Message.hh"
#include "Element.hh"

#include <stdexcept>


#define CHUNK_SIZE 10000    //10KB chunks
#define CHUNK_TYPE_BEGIN 0
#define CHUNK_TYPE_END 1
#define FILE_TRANSFER_TYPE_CLIENT 0
#define FILE_TRANSFER_TYPE_SERVER 1



using namespace std;

class FileTransfer{
    
    Element _element;
    Transceiver* _trns;
    
    FILE* _file;
    char* _sendBuffer;
    uint32_t _sendBufferLen;
    char* _recvBuffer;
    uint32_t _recvBufferLen;
    
    list<Message*> _recvList;
    
    uint16_t _window;
    uint32_t _seqBegin;
    uint32_t _seqEnd;
    uint32_t _seqCurrent;
    
    uint32_t _chunkBegin;
    uint32_t _chunkEnd;
    uint32_t _chunkCurrent;
    //uint32_t _chunkAcked;
    
    uint32_t _sendRate;  //Bytes / s
    
public:
    
    FileTransfer(Transceiver* trns, Element file, uint32_t chunkBegin,
                 uint32_t chunkEnd , int seqnum, int type)throw(runtime_error);
    ~FileTransfer();
    
    const Element& getElement(){return _element;}
    void recvChunck();
        
    bool recvFile(const Message* msg);
    bool recvFinish();
    
    /*
     * @param msg Last received message
     */
    void recvTimeout(const Message* msg);
    
    bool sendFile(const Message* msg);
    
private:
    void loadWindow(uint16_t size);
    bool sendWindow(uint16_t size);
    bool sendChunk(const char* chunk, uint16_t len, uint16_t window,
                   uint32_t chunknum, uint32_t seqnum);
    
    void clearRecvList();
    void writeRecvListToFile();
    
    // Rule of three
    FileTransfer(FileTransfer const& other);
    FileTransfer& operator=(FileTransfer const& other);
    
};

#endif
