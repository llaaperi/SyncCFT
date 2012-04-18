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


#define CHUNK_SIZE 10000    //10KB chunks
#define CHUNK_TYPE_BEGIN 0
#define CHUNK_TYPE_END 1

using namespace std;

class FileTransfer{
    
    Element _element;
    Transceiver* _trns;
    int _seqnum;
    
    FILE* _file;
    char* _sendBuffer;
    unsigned long _sendBufferLen;
    
    uint32_t _chunkBegin;
    uint32_t _chunkEnd;
    uint32_t _chunkCurrent;
    
public:
    
    FileTransfer(Transceiver* trns, Element file, int seqnum);
    ~FileTransfer();
    
    const Element& getElement(){return _element;}
    void recvChunck();
    
    bool transferFile(const Message* msg);
    
private:
    bool sendWindow(int size);
    bool sendChunk(const char* chunk, uint16_t len, uint32_t chunk_num);
    
    // Rule of three
    FileTransfer(FileTransfer const& other);
    FileTransfer& operator=(FileTransfer const& other);
    
};

#endif
