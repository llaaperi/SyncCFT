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

/**
 * FileTransfer constructor
 * @param trns Transceiver object used for file transfer
 * @param file Transmitted file
 * @param chunkBegin First chunk
 * @param chunkEnd Last chunk
 * @param seqnum Sequence number to begin from
 * @param type Send or receive
 */
FileTransfer::FileTransfer(Transceiver* trns, Element file, uint32_t chunkBegin,
                           uint32_t chunkEnd , int seqnum, int type)
                            throw(std::runtime_error) : 
                            _trns(trns), _element(file), _window(1), _seqCurrent(seqnum),
                            _seqBegin(seqnum), _chunkBegin(chunkBegin),
                            _chunkEnd(chunkEnd), _chunkCurrent(chunkBegin),
                            _sendBuffer(NULL), _sendBufferLen(0),
                            _file(NULL), _sendRate(1000000) {
                                
    cout << "[FILE] Transfer created" << endl;
    
    //Init chunk numbers
    if(_chunkBegin == 0){   //If begin is 0 change it to 1 (first chunk)
        _chunkBegin = 1;
    }
    _chunkCurrent = _chunkBegin - 1;    //Reset current to the one before next receive or send chunk
                                
    if(_chunkEnd == 0){
        _chunkEnd = ceil((double)_element.getSize() / CHUNK_SIZE);  //Set end to the end of the file
    }
    
    cout << "[TRANSFER] file name: " << _element.getName() << " size: " << _element.getSize() << endl;
    cout << "[TRANSFER] chunkB: " << _chunkBegin << ", chunkE: " << _chunkEnd << endl;
    
    if (type == FILE_TRANSFER_TYPE_CLIENT) {
        //Open temp file for writing
        string fName = _element.getName() + ".tmp";
        _file = fopen((_syncDir + fName).c_str(), "w");  //w or a
    } else {
        // Open file for reading
        _file = fopen((_syncDir + _element.getName()).c_str(), "r");
        //string fName = _element.getName() + ".tmp";
        //_outFile = fopen(fName.c_str(), "w");
    }
    
    if(_file == NULL){
        cout << "[TRANSFER] File could not be opened" << endl;
        throw runtime_error("File could not be opened");
    }
}


/**
 * Destructor for file transfer. Frees allocated dynamic memory.
 */
FileTransfer::~FileTransfer(){
    cout << "[FILE] Transfer destroyed" << endl;
    if(_file != NULL){
        fclose(_file);
    }
    if(_sendBuffer != NULL){    //Free send buffer
        free(_sendBuffer);
    }
    recvListClear();    //Clear receive list
}


/**
 * Clear reception list content
 */
void FileTransfer::recvListClear(){
    for(Message* ptr: _recvList) {
        delete(ptr);    //Free message
    }
    _recvList.clear();  //Remove all messages from the reception queue
}


/**
 * Write reception list to the file
 * @param last Final message to be written
 */
void FileTransfer::writeRecvListToFile(const Message* last){
    
    //cout << "[TRANSFER] Writing received chunks to a file" << endl;
    
    // Write data to a temp file and free message
    for(Message* ptr : _recvList) {
        
        cout << "[TRANSFER] Writing chunk " << ptr->getChunk() << " with seqnum of " << ptr->getSeqnum() << " to a file." << endl;
        
        fwrite(ptr->getPayload(), 1, ptr->getPayloadLength(), _file);   //Write message to the file
        
        if(last == ptr){
            break;
        }
        
    }
    recvListClear();    //Clear list
}


/**
 * Add new message to the reception list
 * @param msg Received message 
 */
void FileTransfer::recvListAdd(Message* msg){
    
    //Sort reveice list according to sequence number
    _recvList.sort(Message::compare_seqnum);
    
    for(Message* ptr : _recvList){
    
        if(msg->getSeqnum() == ptr->getSeqnum()){   //Discard duplicates
            delete(msg);
            return;
        }
        
        if(msg->getSeqnum() < ptr->getSeqnum()){    //Skip rest of the list bcause message should be before ptr
            break;
        }
    
    }
    
    _recvList.push_back(msg);
    _recvList.sort(Message::compare_seqnum);
}


/**
 * Handle receiving a file from the server
 * @param msg Received message
 * @return True when the file transfer is completed
 */
bool FileTransfer::recvFile(const Message* msg){
    
    //Accept only FILE messages
    // TODO: Check that value is within the window (also over)
    if((msg->getType() != TYPE_FILE) || (msg->getChunk() <= _chunkCurrent)){
        return false;
    }
    
    //Set current window
    _window = msg->getWindow();
    
    //Copy received message and put it in reception queue
    Message *recvMsg = new Message(*msg);
    recvMsg->setPayload(msg->getPayload(), msg->getPayloadLength());
    recvListAdd(recvMsg);
    
    cout << " chunkCurrent: " << _chunkCurrent << ", isFirst: " << recvMsg->isFirst() << ", isLast: " << recvMsg->isLast() << ", Chunk: " << recvMsg->getChunk() << endl;
    
    //Check if last message of the window is received and the window is received correctly
    if(recvFinish()){
            
        cout << "[TRANSFER] Window completed" << endl;
                
        //Whole file is received
        if(_chunkCurrent == _chunkEnd){
            
            //Finish file ransfer by checking file validity and renaming it correctly (without .tmp suffix)
            if(!fileFinish()){
                cout << "[TRANSFER] Invalid file " << _element.getName() << " received " << endl;
            }else{
                cout << "[TRANSFER] File " << _element.getName() << " received successfully" << endl;
            }
            return true;    //Return true whether or not the file was received successfully
        }
    }
    return false;
}



/**
 * Finish file transfer.Check that the hash matches and rename temp file.
 * @return True if the file was received correctly
 */
bool FileTransfer::fileFinish(){
    fclose(_file); // Close output file
    string fileHash;
    string tmpName = _element.getName() + ".tmp"; 
    if(!Utilities::MD5Hash(_syncDir + tmpName, fileHash)){
        cout << "[TRANSFER] File " << tmpName << " not found" << endl;
        return false;
    }
    
    if(_element.getHash() != fileHash){
        cout << "[TRANSFER] File " << tmpName << " has invalid MD5 hash" << endl;
        return false;
    }
    
    if(rename((_syncDir + tmpName).c_str(), (_syncDir + _element.getName()).c_str())){
        cout << "[TRANSFER] Renaming file " << tmpName << " failed" << endl;
        return false;
    }
    return true;
}


/**
 * Sort received messages and check if some are missing from chunks
 * @param last Pointer to the last message in the last full chunk
 * @return Number of fully received chunks
 */
int FileTransfer::getReceivedChunks(Message** last){

    //Sort reveice list according to sequence number
    _recvList.sort(Message::compare_seqnum);

    cout << "[TRANSFER] Recv List: (size " << _recvList.size() << ")" << endl;
    
    //Check that rest of the messages are received 
    uint32_t currentSeq = _seqCurrent + 1;
    uint16_t receivedChunks = 0;
    for(Message *msg : _recvList){
        
        cout << "seqnum=" << msg->getSeqnum() << "(" << currentSeq << "), chunk=" << msg->getChunk() << endl;
        
        if(msg->getSeqnum() != currentSeq++){
            cout << "[TRANSFER] Packet missing from window" << endl;
            return receivedChunks;
        }
        if(msg->isLast()) {
            ++receivedChunks;
            *last = msg;
        }
    }
    return receivedChunks;
}


/**
 * Check that all chunks have been received correctly and reply to the sender.
 * @return True if all chunks have been received
 */
bool FileTransfer::recvFinish(){
    
    Message* last;
    int receivedChunks = getReceivedChunks(&last);
    cout << "[TRANSFER] Received chunks: " << receivedChunks << ", Window: " << _window << endl;
    
    //Check that some chunks are received
    if(receivedChunks == 0){
        return false;
    }
    
    // Check that all chunks have been received
    if((receivedChunks < _window) && ((_chunkCurrent + receivedChunks) != _chunkEnd)) {
        return false;
    }
    
    cout << "[TRANSFER] All packets found" << endl;
    writeRecvListToFile(last);  //Write completed chunks to file

    Message reply(*last);
    reply.setType(TYPE_ACK);
    reply.setLast(false);
    reply.setWindow(_window + 1);    //Increment window size after successfull window reception
    reply.setSeqnum(last->getSeqnum());
    reply.setChunk(last->getChunk());
    _trns->send(&reply, CLIENT_TIMEOUT_SEND);
    cout << "[TRANSFER] Client ACK: Chunk: " << reply.getChunk() << ", Seqnum: " << reply.getSeqnum() << endl; 

    
    _chunkCurrent = last->getChunk();   //Set _chunkCurrent to last received chunk
    _seqCurrent = last->getSeqnum();    //Set _seqCurrent to last received seqnum
    
    return true;
}


/**
 * Handle receive timeouts. Called when packet(s) are lost.
 * @param msg Last received message
 */
void FileTransfer::recvTimeout(const Message *msg){
    
    Message* last;
    int receivedChunks = getReceivedChunks(&last);
    // Check that all chunks have been received
    if(receivedChunks > 0){
        cout << "[TRANSFER] Last correct packet seq: " << last->getSeqnum() << ", chunk: " << last->getChunk() << endl;
        _seqCurrent = last->getSeqnum();
        _chunkCurrent = last->getChunk();
        writeRecvListToFile(last);  //Write completed chunks to file
    }
    
    Message reply(*msg);
    reply.setType(TYPE_ACK);
    reply.setFirst(false);
    reply.setLast(false);
    reply.setSeqnum(_seqCurrent);
    reply.setChunk(_chunkCurrent);
    
    //Reduce window size if packet(s) are lost
    uint16_t window = msg->getWindow() / 2;
    if(window == 0){
        window = 1;
    }
    reply.setWindow(window);
    
    _trns->send(&reply, CLIENT_TIMEOUT_SEND);
    cout << "[TRANSFER] Client ACK: Chunk: " << reply.getChunk() << ", Seqnum: " << reply.getSeqnum() << endl; 
}


/**
 * Handle file sending
 * @param msg Last received message
 * @return True if whole file has been transmitted
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
        loadWindow(msg->getWindow());   //Send first window after GET
        sendWindow(msg->getWindow());
        return false;
    }
    
    //Handle FILE ACK's
    if(msg->getType() == TYPE_ACK){
        
        cout << "[TRANSFER] Client ACKed chunk " << msg->getChunk() << " Seqnum=" << msg->getSeqnum() << endl;
        
        //Check if whole file has been trasmitted
        if(msg->getChunk() >= _chunkEnd){
            cout << "[TRASFER] All chunks ACKed: ACK=" << msg->getChunk() << " Current=" << _chunkCurrent << " End=" << _chunkEnd << " Seqnum=" << msg->getSeqnum() << endl;
            return true;
        }
                
        //Packet is lost
        if(msg->getChunk() < _chunkCurrent){
            cout << "[TRANSFER] Packet lost: ACK=" << msg->getChunk() << " Current=" << _chunkCurrent << " End=" << _chunkEnd << " Seqnum=" << msg->getSeqnum() << endl;
            //long int offset = ((_chunkCurrent - 1) - msg->getChunk()) * CHUNK_SIZE; //((curr - 1) - ack) * CSIZE 
            long int offset = msg->getChunk() * CHUNK_SIZE;
            
            cout << "[TRANSFER] File pointer set to " << offset << endl;
            if(fseek(_file, offset, SEEK_SET)){     //Rewind file pointer to the acked position
                cout << "[TRANSFER] fseek failed" << endl;
            }
            _chunkCurrent = msg->getChunk();    //Reset _chunkCurrent
            _seqCurrent = msg->getSeqnum();     //Reset _seqCurrent
        }
        
        //Send window
        loadWindow(msg->getWindow());
        sendWindow(msg->getWindow());
    }
    return false;
}


/**
 * Load window size of data to the send buffer
 * @param size Window size
 */
void FileTransfer::loadWindow(uint16_t size){
    
    //cout << "[TRANSFER] Loading window data" << endl;
    //Load chunks fo buffer
    unsigned int bytes = 0;
    bytes = (unsigned int)fread(_sendBuffer, 1, size * CHUNK_SIZE, _file);
    _sendBufferLen = bytes;
    cout << "[TRANSFER] Loaded " << bytes << " bytes of data" << endl;
}


/**
 * Send a window of data
 * @param size Window size
 * @return True when window has been sent
 */
bool FileTransfer::sendWindow(uint16_t size){
    
    cout << "[TRANSFER] Sending window of size " << size << endl;
    
    int chunks = ceil((double)_sendBufferLen / CHUNK_SIZE);  //Number of read chunks
    for(int i = 0; i < chunks; i++){
        
        char* ptr = _sendBuffer + (i * CHUNK_SIZE); //Chunk begin
        uint32_t len = _sendBufferLen - (i * CHUNK_SIZE);   //Calculate chunk size
        if(len > CHUNK_SIZE){
            len = CHUNK_SIZE;
        }
        
        //Send chunk
        if(sendChunk(ptr, len, _window, ++_chunkCurrent, ++_seqCurrent)){
            //++_chunkCurrent;
        }
    }
    return true;
}


/**
 * Send a single chunk
 * @param chunk Pointer to the data
 * @param len Length of the data
 * @param window Size of the transfer window
 * @param chunknum Chunk number
 * @param seqnum Sequence number
 * @return True when whole chunk has been transmitted
 */
bool FileTransfer::sendChunk(const char* chunk, uint16_t len, uint16_t window, uint32_t chunknum, uint32_t seqnum){
    
    Message msg;
    msg.init(_trns->getVersion(), TYPE_FILE);
    msg.setWindow(window);
    msg.setChunk(chunknum);
    msg.setSeqnum(seqnum);
    
    cout << "[TRANSFER] Sending chunk " << chunknum << " of size " << len << endl;
    
    //TODO
    //Flow id
    
    msg.setFirst(true);
    while(len > 0){
        
        //Send full packet
        if(len > MESSAGE_MTU){
            
            msg.setPayload(chunk, MESSAGE_MTU);
            _trns->send(&msg, SERVER_TIMEOUT_SEND);
            cout << "[TRANSFER] message " << msg.getSeqnum() << " sent from chunk " << msg.getChunk() << " First = " << msg.isFirst() << ", Last = " << msg.isLast() << endl;
            msg.incrSeqnum();   //Increment seqnum for the next packet
            msg.setFirst(false);
            
            len -= MESSAGE_MTU; //Decrement remaining length
            chunk += MESSAGE_MTU;   //Increment payload pointer
        }else{  //Last packet
            
            msg.setLast(true);
            msg.setPayload(chunk, len);
            _trns->send(&msg, SERVER_TIMEOUT_SEND);
            cout << "[TRANSFER] message " << msg.getSeqnum() << " sent from chunk " << msg.getChunk() << " First = " << msg.isFirst() << ", Last = " << msg.isLast() << endl;
            break;
        }
    }
    _seqCurrent = msg.getSeqnum();
    
    double t = (double)CHUNK_SIZE / _sendRate;   //Time needed to send chunk
    uint32_t delay = t * 1000000;
    //cout << "[TRANSFER] Send delay (us): " << delay << endl;
    usleep(delay);
    
    //cout << "[TRANSFER] last payload length " << msg.getPayloadLength() << endl;
    return true;
}
