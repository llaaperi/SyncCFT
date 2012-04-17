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
            fileHandler(msg);
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
 *
 */
void SessionHandler::getHandler(const Message* msg){
    
    cout << "[SESSION] Get handler started" << endl;
    
    //Check message validity
    if(!isValidMessage(msg)){
        return; //Discard invalid packets
    }
    
    //const char* payload = msg->getPayload();
    string line(msg->getPayload());
    vector<string> parts; 
    int size = Utilities::split(line, ";", parts);
    
    //Validity check (file1.txt;0-0)
    if(size != 2){
        return;
    }
    
    MetaFile mFile(METAFILE);
    bool found = false;
    Element file = mFile.find(parts[0], found);
    
    bool isTransferring = false;
    if(found){
        //If already int transfer
        //TODO own functions
        for(int i = 0; i < SESSIONHANDLER_MAX_TRANSFERS; i++){
        
            FileTransfer* ft = _fFlows[i];
            
            if(ft != NULL){
                file = ft->getElement();
                isTransferring = true;
                break;
            }
        }
    }
    
    //TODO MORE CHECKING
    Message reply(*msg);
    if(!found){
        //Send NACK if file not found
        cout << "[SESSION] Requested non-existing file" << endl;
        reply.setType(TYPE_NACK);
        _trns->send(&reply, SERVER_TIMEOUT_SEND);
    }else{
        
        //If this file is not already in transrer, create new FileTransfer object
        if(!isTransferring){
            _fFlows[getFreeFlow()] = new FileTransfer(_trns, file, 0);
        }
        
        //Send ACK
        cout << "[SESSION] Requested file" << parts[0] << endl;
        reply.setType(TYPE_ACK);
        _trns->send(&reply, SERVER_TIMEOUT_SEND);
    }
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
    
    bool finished = false;
    finished = flow->transferFile(msg);    //Give chunk to the file transfer object
    
    //File is transferred successfully
    if(finished){
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
