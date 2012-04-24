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
#include <list>

#include "FileTransfer.hh"
#include "Server.hh"

/*
 * FileTransfer constructor
 */
FileTransfer::FileTransfer(Transceiver* trns, Element file, int seqnum) : _trns(trns), _element(file), _seqCurrent(seqnum), _chunkBegin(0), _chunkEnd(0), _chunkCurrent(0), _chunkAcked(0), _sendBuffer(NULL), _sendBufferLen(0), _recvBuffer(NULL), _recvBufferLen(0), _file(NULL){
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
    if(_chunkBegin == 0){   //If begin is 0 change it to 1 (first chunk)
        _chunkBegin = 1;
    }
    _chunkEnd = chunkEnd;
    if(_chunkEnd == 0){
        _chunkEnd = ceil((double)_element.getSize() / CHUNK_SIZE);  //Set end to the end of the file
    }
    _chunkCurrent = _chunkBegin;
    _chunkAcked =_chunkCurrent;
    
    _seqBegin = _seqCurrent;
    
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
    
    Message *recvMsg = new Message(*msg);
    recvMsg->setPayload(msg->getPayload(), msg->getPayloadLength());
    
    _recvList.push_back(recvMsg);
    
    uint16_t lastChunknum = _chunkCurrent + msg->getWindow() - 1;
    if (lastChunknum > _chunkEnd) {
        lastChunknum = _chunkEnd;
    }
    static bool lastChunkReceived = false;
    
    cout << "lastChunkReceived: " << lastChunkReceived << " chunkCurrent: " << _chunkCurrent << ", lastChunk: " << lastChunknum << ", isLast: " << recvMsg->isLast() << ", Chunk: " << recvMsg->getChunk() << endl;
    
    if((recvMsg->getChunk() >= lastChunknum) && recvMsg->isLast()){
        
        lastChunkReceived = true;
    }
    
    if(lastChunkReceived && recvFinish()){
            
        cout << "[TRANSFER] Window completed" << endl;
        
        // Write data to a temp file
        for(Message* ptr: _recvList) {
            fwrite(ptr->getPayload(), 1, ptr->getPayloadLength(), _file);
        }
        
        //Whole file is received
        if(_chunkCurrent == _chunkEnd){
            cout << "[TRANSFER] Complete file received" << endl;
            lastChunkReceived = false;
            return true;
        }
        
        //Continue reception of subsequent windows
        ++_chunkCurrent;
        ++_seqCurrent;
        lastChunkReceived = false;
        _recvList.clear();  //Remove all messages from the reception queue
    }

    return false;
}


/*
 *
 */
bool FileTransfer::recvFinish(){
    
    cout << "[TRANSFER] Window finished " << endl;
    
    //Sort reveice list according to sequence number
    _recvList.sort(Message::compare_seqnum);
    
    //Check that rest of the messages are received 
    cout << "[TRANSFER] Recv List: (size " << _recvList.size() << ")" << endl;
    uint32_t currentSeq = _seqCurrent;
    for(Message *msg : _recvList){
        
        cout << "seqnum=" << msg->getSeqnum() << "(" << currentSeq << "), chunk=" << msg->getChunk() << endl;
        if(msg->getSeqnum() != currentSeq++){
            cout << "Packet missing" << endl;
            return false;
        }
    }
    cout << "All packets found" << endl;

    Message reply(*_recvList.back());
    reply.setType(TYPE_ACK);
    reply.setLast(false);
    reply.setSeqnum(_recvList.back()->getSeqnum());
    reply.setChunk(_recvList.back()->getChunk());
    _trns->send(&reply, CLIENT_TIMEOUT_SEND);
    
    _chunkCurrent = _recvList.back()->getChunk();
    _seqCurrent = _recvList.back()->getSeqnum();
    
    return true;
}


/*
 *
 */
/*
bool FileTransfer::recvWindow(uint16_t size){
    
    uint16_t packets = size * ceil((double)CHUNK_SIZE / MESSAGE_MTU);
    uint8_t recvChunk[packets];
    
    cout << "[TRANSFER] Prepare to receive " << packets << " packets" << endl;
    
    for(int i = 0; i < packets; i++){}
    
    Message msg;
    list<Message> recvMsg;
    
    bool finished = false;
    while(finished){
        
        _trns->recv(&msg, CLIENT_TIMEOUT_FILE);
        
        if
    
    }
    
    
}
*/

//bool FileTransfer::recvChunk(uint32_t chunknum, uint32_t seqnum){}


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
    
    //Parse message info
    _window = msg->getWindow();
    
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
            cout << "[TRASFER] All chunks ACKed: ACK=" << msg->getChunk() << " Current=" << _chunkCurrent << " End=" << _chunkEnd << endl;
            return true;
        }
        
        _chunkAcked = msg->getChunk();
        
        //Packet is lost
        if(msg->getChunk() < (_chunkCurrent - 1)){
            cout << "[TRANSFER] Packet lost: ACK=" << msg->getChunk() << " Current=" << _chunkCurrent << " End=" << _chunkEnd << endl;
            //long int offset = ((_chunkCurrent - 1) - msg->getChunk()) * CHUNK_SIZE; //((curr - 1) - ack) * CSIZE 
            //fseek(_file, offset, SEEK_CUR); //Rewind file pointer to the acked position + 1
            //_chunkCurrent = msg->getChunk() + 1;    //
            //return false;   //MUST BE REMOVED
        }
        
        //No packets lost
        if(msg->getChunk() == (_chunkCurrent - 1)){
            cout << "[TRANSFER] Send next window: ACK=" << msg->getChunk() << " Current=" << _chunkCurrent << " End=" << _chunkEnd << endl;
        }
        
        loadWindow(msg->getWindow());
        sendWindow(msg->getWindow());
    }
    return false;
}


/*
 * Load window size amount of data to send buffer
 */
void FileTransfer::loadWindow(uint16_t size){
    
    //cout << "[TRANSFER] Loading window data" << endl;
    //Load chunks fo buffer
    unsigned int bytes = 0;
    bytes = (unsigned int)fread(_sendBuffer, 1, size * CHUNK_SIZE, _file);
    _sendBufferLen = bytes;
    cout << "[TRANSFER] Loaded " << bytes << " bytes of data" << endl;
}


/*
 * Send window.
 * @param size  window size
 */
bool FileTransfer::sendWindow(uint16_t size){
    
    cout << "[TRANSFER] Sending window of size " << size << endl;
    
    int chunks = ceil((double)_sendBufferLen / CHUNK_SIZE);  //Number of read chunks
    for(int i = 0; i < chunks; i++){
        
        char* ptr = _sendBuffer + (i * CHUNK_SIZE); //Chunk begin
        uint16_t len = _sendBufferLen - (i * CHUNK_SIZE);   //Calculate chunk size
        if(len > CHUNK_SIZE){
            len = CHUNK_SIZE;
        }
        
        //Send chunk
        if(sendChunk(ptr, len, _window, _chunkCurrent, _seqCurrent)){
            ++_chunkCurrent;
        }
    }
    return true;
}


/*
 *
 */
bool FileTransfer::sendChunk(const char* chunk, uint16_t len, uint16_t window, uint32_t chunknum, uint32_t seqnum){
    
    Message msg;
    msg.init(TYPE_FILE);
    msg.setWindow(window);
    msg.setChunk(chunknum);
    msg.setSeqnum(seqnum);
    
    cout << "[TRANSFER] Sending chunk " << chunknum << " of size " << len << endl;
    
    //TODO
    //Flow id
    
    msg.setFirst(true);
    while(len > 0){
    
        cout << "[TRANSFER] message " << _seqCurrent << " sent from chunk " << chunknum << endl;
        
        //Send full packet
        if(len > MESSAGE_MTU){
            
            msg.setPayload(chunk, MESSAGE_MTU);
            _trns->send(&msg, SERVER_TIMEOUT_SEND);
            
            msg.setFirst(false);
            
            len -= MESSAGE_MTU; //Decrement remaining length
            chunk += MESSAGE_MTU;   //Increment payload pointer
        }else{  //Last packet
            
            msg.setLast(true);
            msg.setPayload(chunk, len);
            _trns->send(&msg, SERVER_TIMEOUT_SEND);
            ++_seqCurrent;
            break;
        }
        
        msg.incrSeqnum();   //Increment seqnum for the next packet
        ++_seqCurrent;
    }
    
    cout << "[TRANSFER] last payload length " << msg.getPayloadLength() << endl;
    return true;
}
