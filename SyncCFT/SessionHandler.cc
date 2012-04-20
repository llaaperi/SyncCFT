//
//  SessionHandler.cc
//  SyncCFT
//
//  Created by Elo Matias on 3/23/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "SessionHandler.hh"
#include "Transceiver.hh"
#include "Metafile.hh"
#include "Server.hh"
#include "utilities.hh"


/*
 * Constructor
 */
SessionHandler::SessionHandler(int socket, struct sockaddr* cliAddr, uint8_t id, uint32_t seqnum) : _id(id), _seqnum(seqnum){
    cout << "[SESSION] New session with id " << (unsigned int)id << " accepted"<< endl;
    //Create new tranceiver for this session
    _trns = new Transceiver(socket, *cliAddr);
    memset(_fFlows, 0, SESSIONHANDLER_MAX_TRANSFERS * sizeof(FileTransfer*));
}


/*
 * Destructor
 */
SessionHandler::~SessionHandler(){
    // Commented to enable unit testing
    cout << "[SESSION] Session " << (unsigned int)_id << " terminated"<< endl;
    delete(_trns);
}


void SessionHandler::newMessage(const Message* msg){

    switch (msg->getType()) {
        case TYPE_ACK:
            cout << "[SESSION] Received ACK message" << endl;
            fileHandler(msg);
            break;
        case TYPE_HELLO:
            cout << "[SESSION] Received HELLO message" << endl;
            break;
        case TYPE_DESCR:
            cout << "[SESSION] Received DESCR message" << endl;
            descrHandler(msg);
            break;
        case TYPE_DIFF:
            cout << "[SESSION] Received DIFF message" << endl;
            break;
        case TYPE_GET:
            cout << "[SESSION] Received GET message" << endl;
            getHandler(msg);
            break;
        case TYPE_FILE:
            cout << "[SESSION] Received FILE message" << endl;
            break;
        case TYPE_QUIT:
            cout << "[SESSION] Received QUIT message" << endl;
            break;
        case TYPE_NACK:
            cout << "[SESSION] Received NACK message" << endl;
            break;
        default:
            cout << "[SESSION] Unknown message type" << endl;
            break;
    }
}


/*
 * Function for checking if source is valid for this session
 */
bool SessionHandler::isValidSource(const Message *msg){
    
    //Compare message address to the Transceiver address
    if(!Networking::cmpAddr(msg->getAddr(), _trns->getAddr())){
        return false;
    }
    return true;
}


/*
 * 
 */
bool SessionHandler::isValidMessage(const Message *msg){
    
    //Compare source address
    if(!isValidSource(msg)){
        cout << "Invalid source address" << endl;
        return false;
    }
    
    //Compare sequence number
    if((msg->getSeqnum() < _seqnum) && (msg->getSeqnum() > (_seqnum + 7))){
        cout << "Invalid sequence number" << endl;
        return false;
    }
    
    return true;
}


/*
 * Handles DESCR messages
 */
void SessionHandler::descrHandler(const Message* msg){
    
    cout << "[SESSION] Description handler started" << endl;
    
    MetaFile mFile(METAFILE);
    //mFile.print();
    
    Message reply(*msg);
    reply.setType(TYPE_DIFF);
    reply.setFirst(true);
    reply.setLast(true);
    
    MetaFile clientFile(msg->getPayload(), msg->getPayloadLength());
    string diff = mFile.getDiff(clientFile);
    
    //cout << "[SESSION] Diff file:" << endl << diff << endl;
    reply.setPayload(diff.c_str(), (int)diff.length());
    
    //msg->printInfo();
    
    //Send diff
    _trns->send(&reply, SERVER_TIMEOUT_SEND);
    cout << "[SESSION] Description handler finished" << endl;
}



/*
 * Function for requesting next free client id
 * Return: -1 if no free ID's are available or free id
 */
int SessionHandler::getFreeFlow(){
    
    for(int i = 0; i < SESSIONHANDLER_MAX_TRANSFERS; i++){
        if(_fFlows[i] == NULL){
            return i;
        }
    }
    return -1;
}


/*
 * Check if Element file is being transferred already
 * @return true if file is being transferred
 */
bool SessionHandler::isTransferring(Element* file){

    //If already in transfer
    for(int i = 0; i < SESSIONHANDLER_MAX_TRANSFERS; i++){
        
        FileTransfer* ft = _fFlows[i];
        if((ft != NULL) && !file->compare(ft->getElement())){
            return true;
        }
    }
    return false;
}


/*
 * Send reply NACK to the message
 */
void SessionHandler::sendNack(const Message* msg){
    Message reply(*msg);
    reply.setType(TYPE_NACK);
    _trns->send(&reply, SERVER_TIMEOUT_SEND);
}


/*
 * Send reply ACK to the message
 */
void SessionHandler::sendAck(const Message* msg){
    Message reply(*msg);
    reply.setType(TYPE_ACK);
    _trns->send(&reply, SERVER_TIMEOUT_SEND);
}


/*
 *
 */
bool SessionHandler::parseGet(const Message* msg, Element* file, uint32_t* chunkBegin, uint32_t* chunkEnd){
    
    //Check message validity
    if(!isValidMessage(msg)){
        cout << "[SESSION] Invalid message" << endl;
        return false;
    }
    
    //Parse GET message
    string line(msg->getPayload());
    vector<string> parts; 
    int size = Utilities::split(line, ";", parts);
    
    //Validity check (file1.txt;0-0)
    if(size != 2){
        cout << "[SESSION] Invalid GET message" << endl;
        return false;
    }
    
    //Get file
    MetaFile mFile(METAFILE);
    bool found = false;
    Element fileTmp = mFile.find(parts[0], found);
    
    //Reply wih NACK if file is not found
    if(!found){
        cout << "[SESSION] Requested non-existing file" << endl;
        return false;
    }
    
    //Parse chunk numbers
    uint32_t chunkBeginTmp = 0;
    uint32_t chunkEndTmp = 0;
    sscanf(parts[1].c_str(), "%u-%u", &chunkBeginTmp, &chunkEndTmp);
    
    //TODO check chunk numbers
    
    *chunkBegin = chunkBeginTmp;
    *chunkEnd = chunkEndTmp;
    *file = fileTmp; //Reference to the pointer
    return true;
}


/*
 *
 */
void SessionHandler::getHandler(const Message* msg){
    
    cout << "[SESSION] Get handler started: " << msg->getPayload() << endl;
    uint32_t chunkBegin = 0;
    uint32_t chunkEnd = 0;
    Element file;
    
    //Parse GET message
    if(!parseGet(msg, &file, &chunkBegin, &chunkEnd)){
        cout << "[SESSION] Get parse failed" << endl;
        return sendNack(msg);
    }
    
    //Chek if file is being transferred
    if(isTransferring(&file)){
        cout << "[SESSION] Requested file alredy being transferred" << endl;
        return; //This is probably a duplicate GET -> discard
    }
    
    //Create new FileTransfer object
    int flowIdx = getFreeFlow();
    _fFlows[flowIdx] = new FileTransfer(_trns, file, 0);
    _fFlows[flowIdx]->initSend(chunkBegin, chunkEnd);
    
    //Send ACK
    //cout << "[SESSION] Requested file " << file.getName() << endl;
    sendAck(msg);
    
    //Send first window
    _fFlows[flowIdx]->sendFile(msg);
}


/*
 *
 */
void SessionHandler::fileHandler(const Message* msg){
    
    cout << "[SESSION] File handler started" << endl;
    
    int flowID = 0;
    //Chose file transfer from messages flow index
    //TODO
    FileTransfer* flow = _fFlows[flowID];
    
    //Check that flow is valid and active
    if(flow == NULL){
        return;
    }
    
    //Give chunk to the file transfer object
    if(flow->sendFile(msg)){    //File is transferred successfully
        delete(flow);
        _fFlows[flowID] = NULL;
    }
}


/*
 * Creates a 256-bit session key from two nonces and the secret key
 * @param nonce1 First 16-byte random nonce
 * @param nonce2 Second 16-byte random nonce
 * @param secretKey Stored  64-byte secret key
 */ 
void SessionHandler::createSessionKey(unsigned char* nonce1, unsigned char* nonce2, unsigned char* secretKey)
{
    unsigned char data[96]; // 16+16+64
    unsigned char* dataPointer = data;
    memset(data, 0, 96);
    
    // Combine key paramters
    memcpy(dataPointer, nonce1, 16);
    dataPointer += 16;
    memcpy(dataPointer, nonce2, 16);
    dataPointer += 16;
    memcpy(dataPointer, secretKey, 16);

    Utilities::SHA256Hash(_sessionKey, data, 96);
}
