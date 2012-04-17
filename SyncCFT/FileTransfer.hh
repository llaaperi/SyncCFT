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

using namespace std;

class FileTransfer{
    
    Element _file;
    Transceiver* _trns;
    int _seqnum;
    
public:
    
    FileTransfer(Transceiver* trns, Element file, int seqnum);
    ~FileTransfer();
    
    const Element& getElement(){return _file;}
    void recvChunck();
    
    bool transferFile(Message* msg);
    
private:
    
    bool sendChunk(unsigned long chunk);
    
    // Rule of three
    FileTransfer(FileTransfer const& other);
    FileTransfer& operator=(FileTransfer const& other);
    
};

#endif
