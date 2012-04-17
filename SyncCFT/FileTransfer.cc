//
//  FileTransfer.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 30.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "FileTransfer.hh"

/*
 * FileTransfer constructor
 */
FileTransfer::FileTransfer(Transceiver* trns, Element file, int seqnum) : _trns(trns), _element(file), _seqnum(seqnum){
    cout << "[FILE] Transfer created" << endl;
}

FileTransfer::~FileTransfer(){
    cout << "[FILE] Transfer destroyed" << endl;
}


bool FileTransfer::transferFile(const Message* msg){
    
    //cout << "[TRANSFER] received: " << msg->getPayload() << endl;
    //cout << "[TRANSFER] received type: " << msg->getType() << endl;
    msg->printInfo();
    
    static bool init = false;
    if(!init && (msg->getType() == TYPE_GET)){
        init = true;    //Make shure that this function is called only once
        
        //Parse begin and end chunks
        string line(msg->getPayload());
        vector<string> parts; 
        int size = Utilities::split(line, ";", parts);
        if(size < 2){
            return false;
        }
        
        sscanf(parts[1].c_str(), "%u-%u", &_chunkBegin, &_chunkEnd);
        _chunkCurrent = _chunkBegin;
        //cout << "[TRANSFER] chunkB: " << _chunkBegin << ", chunkE: " << _chunkEnd << endl;
        
        //Open FILE
        _file = fopen(_element.getName().c_str(), "r");
        if(_file == NULL){
            cout << "[TRANSFER] File could not be opened" << endl;
            return true;    //File is transferred (Could be a better solution)
        }
        
        //Allocate window size amount of send buffer
        _sendBufferLen = msg->getWindow() * CHUNK_SIZE;
        _sendBuffer = (char*)malloc(_sendBufferLen);
        
        
        sendWindow(msg->getWindow());
        return false;
    }
    
    if(init && (msg->getType() == TYPE_ACK)){
        
        return false;
    }
    
    //Invalid packet received here
    
    
    return false;
}

/*
 * Send window.
 * @param size  window size
 */
bool FileTransfer::sendWindow(int size){
    
    //Load file fo buffer
    unsigned int bytes = 0;
    bytes = (unsigned int)fread(_sendBuffer, 1, size * CHUNK_SIZE, _file);
    _sendBufferLen = bytes;
    
    int chunks = ceil(bytes / CHUNK_SIZE);  //Number of read chunks
    
    for(int i = 0; i < chunks; i++){
        sendChunk(i);
    }
    return false;
}


bool FileTransfer::sendChunk(int offset){
    
    
    
    
    return true;
}
