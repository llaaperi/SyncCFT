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
    list<Message*> _recvList;
    
    FILE* _outFile;
    
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
    /**
     * FileTransfer constructor
     * @param trns Transceiver object used for file transfer
     * @param file Transmitted file
     * @param chunkBegin First chunk
     * @param chunkEnd Last chunk
     * @param seqnum Sequence number to begin from
     * @param type Send or receive
     */
    FileTransfer(Transceiver* trns, Element file, uint32_t chunkBegin,
                 uint32_t chunkEnd , int seqnum, int type)throw(runtime_error);
    
    /**
     * Destructor for file transfer. Frees allocated dynamic memory.
     */
    ~FileTransfer();
    
    const Element& getElement(){return _element;}
    
    /**
     * Handle receiving a file from the server
     * @param msg Received message
     * @return True when the file transfer is completed
     */
    bool recvFile(const Message* msg);
    
    /**
     * Handle receive timeouts. Called when packet(s) are lost.
     * @param msg Last received message
     */
    void recvTimeout(const Message* msg);
    
    /**
     * Handle file sending
     * @param msg Last received message
     * @return True if whole file has been transmitted
     */
    bool sendFile(const Message* msg);
    
private:
    /**
     * Load window size of data to the send buffer
     * @param size Window size
     */
    void loadWindow(uint16_t size);
    
    /**
     * Send a window of data
     * @param size Window size
     */
    bool sendWindow(uint16_t size);
    
    /**
     * Send a single chunk
     * @param chunk Pointer to the data
     * @param len Length of the data
     * @param window Size of the transfer window
     * @param chunknum Chunk number
     * @param seqnum Sequence number
     * @return True when whole chunk has been transmitted
     */
    bool sendChunk(const char* chunk, uint16_t len, uint16_t window,
                   uint32_t chunknum, uint32_t seqnum);
    
    /**
     * Sort received messages and check if some are missing from chunks
     * @param last Pointer to the last message in the last full chunk
     * @return Number of fully received chunks
     */
    int getReceivedChunks(Message** last);
    
    /**
     * Write reception list to the file
     * @param last Final message to be written
     */
    void writeRecvListToFile(const Message* last);
    
    /**
     * Add new message to the reception list
     * @param msg Received message 
     */
    void recvListAdd(Message* msg);
    
    /**
     * Clear reception list content
     */
    void recvListClear();
        
    /**
     * Check that all chunks have been received correctly and reply to the sender.
     * @return True if all chunks have been received
     */
    bool recvFinish();
    
    /**
     * Finish file transfer.Check that the hash matches and rename temp file.
     * @return True if the file was received correctly
     */
    bool fileFinish();
    
    // Rule of three
    FileTransfer(FileTransfer const& other);
    FileTransfer& operator=(FileTransfer const& other);
    
};

#endif
