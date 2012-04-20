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
#include "Server.hh"

/*
 * FileTransfer constructor
 */
FileTransfer::FileTransfer(Transceiver* trns, Element file, int seqnum) : _trns(trns), _element(file), _seqnum(seqnum), _chunkBegin(0), _chunkEnd(0), _chunkCurrent(0), _chunkAcked(0), _sendBuffer(NULL), _sendBufferLen(0), _recvBuffer(NULL), _recvBufferLen(0), _file(NULL){
    cout << "[FILE] Transfer created" << endl;
}


/*
 *
 */
FileTransfer::~FileTransfer(){
    cout << "[FILE] Transfer destroyed" << endl;
    if(_file != NULL){
        fclose(_file);
    }
    if(_sendBuffer != NULL){
        free(_sendBuffer);
    }
    if(_recvBuffer != NULL){
        free(_recvBuffer);
    }
}


/*
 *
 */
bool FileTransfer::initRecv(uint32_t chunkBegin, uint32_t chunkEnd){
    
    //Init chunk numbers
    _chunkBegin = chunkBegin;
    _chunkEnd = chunkEnd;
    _chunkCurrent = _chunkBegin;
    if(_chunkEnd == 0){
        _chunkEnd = ceil((double)_element.getSize() / CHUNK_SIZE);  //Set end to the end of the file
    }
    
    cout << "[TRANSFER] chunkB: " << _chunkBegin << ", chunkE: " << _chunkEnd << endl;
    cout << "[TRANSFER] file name: " << _element.getName() << " size: " << _element.getSize() << endl;
    
    //Open temp file
    string fName = _element.getName() + ".tmp";
    _file = fopen(fName.c_str(), "w");  //w or a
    
    if(_file == NULL){
        cout << "[TRANSFER] File could not be opened" << endl;
        return false;    //File is transferred (Could be a better solution)
    }
    return true;
}


/*
 *
 */
bool FileTransfer::recvFile(const Message* msg){
    
    if(msg->getType() != TYPE_FILE){
        return false;
    }
    
    //Allocate window size amount of recv buffer
    if((msg->getWindow() * CHUNK_SIZE) > _recvBufferLen){
        _recvBufferLen = msg->getWindow() * CHUNK_SIZE;
        _recvBuffer = (char*)realloc(_recvBuffer, _recvBufferLen);
    }
    
    //Receive window
    //recvWindow(msg->getWindow());
    
    
    if(msg->isLast()){
        Message reply(*msg);
        reply.setType(TYPE_ACK);
        reply.setLast(false);
        _trns->send(&reply, CLIENT_TIMEOUT_SEND);
        
        cout << "[TRANSFER] Chunk " << msg->getChunk() << " received" << endl;
        
        if(msg->getChunk() == _chunkEnd){
            cout << "[TRANSFER] File received" << endl;
            return true;
        }
    }
    
    return false;
}


/*
 *
 */
//bool FileTransfer::recvWindow(int size){}



/*
 *
 */
bool FileTransfer::initSend(uint32_t chunkBegin, uint32_t chunkEnd){
    
    //Init chunk numbers
    _chunkBegin = chunkBegin;
    if(_chunkBegin == 0){   //If begin is 0 change it to 1 (first chunk)
        _chunkBegin = 1;
    }
    _chunkEnd = chunkEnd;
    if(_chunkEnd == 0){
        _chunkEnd = ceil((double)_element.getSize() / CHUNK_SIZE);  //Set end to the end of the file
    }
    _chunkCurrent = _chunkBegin;
    _chunkAcked =_chunkCurrent;
    
    cout << "[TRANSFER] chunkB: " << _chunkBegin << ", chunkE: " << _chunkEnd << endl;
    cout << "[TRANSFER] file name: " << _element.getName() << " size: " << _element.getSize() << endl;
    
    //Open FILE
    _file = fopen(_element.getName().c_str(), "r");
    if(_file == NULL){
        cout << "[TRANSFER] File could not be opened" << endl;
        return false;    //File is transferred (Could be a better solution)
    }
    
    return true;
}

/*
 * Return true when finished
 */
bool FileTransfer::sendFile(const Message* msg){
    
    //cout << "[TRANSFER] received: " << msg->getPayload() << endl;
    //cout << "[TRANSFER] received type: " << msg->getType() << endl;
    //msg->printInfo();
    
    //Allocate window size amount of send buffer
    if((msg->getWindow() * CHUNK_SIZE) > _sendBufferLen){
        _sendBufferLen = msg->getWindow() * CHUNK_SIZE;
        _sendBuffer = (char*)realloc(_sendBuffer, _sendBufferLen);
    }
    
    //Received GET (Duplicates discarded before this)
    if(msg->getType() == TYPE_GET){
        
        cout << "[TRANSFER] Send first window after GET" << endl;
        loadWindow(msg->getWindow());
        sendWindow(msg->getWindow());
    }
    
    //Handle FILE ACK's
    if(msg->getType() == TYPE_ACK){
        
        cout << "[TRANSFER] Client ACKed chunk " << msg->getChunk() << endl;
        
        //Check if whole file has been trasmitted
        if(msg->getChunk() >= _chunkEnd){
            return true;
        }
        
        _chunkAcked = msg->getChunk();
        
        //Packet is lost
        if(msg->getChunk() < (_chunkCurrent - 1)){
            long int offset = ((_chunkCurrent - 1) - msg->getChunk()) * CHUNK_SIZE; //((curr - 1) - ack) * CSIZE 
            fseek(_file, offset, SEEK_CUR); //Rewind file pointer to the acked position + 1
            loadWindow(msg->getWindow());
            return false;
        }
        
        //No packets lost
        if(msg->getChunk() == (_chunkCurrent - 1)){
            loadWindow(msg->getWindow());
        }
        
        sendWindow(msg->getWindow());
    }
    return false;
}


/*
 * Load window size amount of data to send buffer
 */
void FileTransfer::loadWindow(int size){
    
    //cout << "[TRANSFER] Loading window data" << endl;
    //Load chunks fo buffer
    unsigned int bytes = 0;
    bytes = (unsigned int)fread(_sendBuffer, 1, size * CHUNK_SIZE, _file);
    _sendBufferLen = bytes;
}


/*
 * Send window.
 * @param size  window size
 */
bool FileTransfer::sendWindow(int size){
    
    cout << "[TRANSFER] Sending window of size " << size << endl;
    
    int chunks = ceil((double)_sendBufferLen / CHUNK_SIZE);  //Number of read chunks
    for(int i = 0; i < chunks; i++){
        sendChunk(_sendBuffer + (i * CHUNK_SIZE), _sendBufferLen, _chunkCurrent);
    }
    return true;
}


/*
 *
 */
bool FileTransfer::sendChunk(const char* chunk, uint16_t len, uint32_t chunk_num){
    
    Message msg;
    msg.init(TYPE_FILE);
    msg.setChunk(chunk_num);
    msg.setSeqnum(0);
    
    //TODO
    //Flow id
    
    //
    msg.setFirst(true);
    int i;
    for(i = 0; i < len; i += MESSAGE_MTU){
        
        cout << "[TRANSFER] message " << i << " sent from chunk " << chunk_num << endl;
        
        msg.incrSeqnum();
        msg.setPayload(chunk + i, MESSAGE_MTU);
        _trns->send(&msg, SERVER_TIMEOUT_SEND);
        
        msg.setFirst(false);
    }
    //Send last message
    msg.incrSeqnum();
    msg.setLast(true);
    msg.setPayload(chunk + i, len - (i - MESSAGE_MTU));
    _trns->send(&msg, SERVER_TIMEOUT_SEND);
    
    cout << "[TRANSFER] message " << i << " sent from chunk " << chunk_num << endl;
    cout << "[TRANSFER] last payload length " << msg.getPayloadLength() << endl;
    
    ++_chunkCurrent;    //Increment current chunk
    return true;
}
