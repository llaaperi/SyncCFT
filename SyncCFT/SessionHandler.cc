//
//  SessionHandler.cc
//  SyncCFT
//
//  Created by Elo Matias on 3/23/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <sstream>

#include "SessionHandler.hh"
#include "Transceiver.hh"
#include "Metafile.hh"
#include "Server.hh"


/**
 * Constuctor
 * @param server Pointer to Server object
 * @param trns Pointer to a Transceiver object
 * @param id Session ID
 * @param seqnum Sequence number
 * @param sessionKey Session key
 */
SessionHandler::SessionHandler(Server* server, Transceiver* trns, uint8_t id, uint32_t seqnum, unsigned char* sessionKey) : _server(server), _trns(trns), _id(id), _seqnum(seqnum), _sessionKey(sessionKey){
    
    cout << "[SESSION] New session with id " << (unsigned int)id << " accepted"<< endl;
    memset(_fFlows, 0, SESSIONHANDLER_MAX_TRANSFERS * sizeof(FileTransfer*));
}


/**
 * Destructor. Frees allocated dynamic memory
 */
SessionHandler::~SessionHandler(){
    
    cout << "[SESSION] Session " << (unsigned int)_id << " terminated"<< endl;
    
    if(_trns != NULL){
        delete(_trns);  //Free transceiver
    }
    
    if(_sessionKey != NULL){
        free(_sessionKey);
    }
    for(int i = 0; i < SESSIONHANDLER_MAX_TRANSFERS; i++){  //Free file transfer objects
        if(_fFlows[i] != NULL){
            delete(_fFlows[i]);
        }
    }
}


/**
 * Function for checking if source is valid for this session
 * @param msg Message to be checked
 * @return True if message source is valid
 */
bool SessionHandler::isValidSource(const Message *msg){
    
    //Compare message address to the Transceiver address
    if(!Networking::cmpAddr(msg->getAddr(), _trns->getAddr())){
        return false;
    }
    return true;
}


/**
 * Function for checking if Message is valid for this session
 * @param msg Message to be checked
 * @return True if message is valid
 */
bool SessionHandler::isValidMessage(const Message *msg){
    
    //Compare source address
    if(!isValidSource(msg)){
        cout << "Invalid source address" << endl;
        return false;
    }
    
    //Compare sequence number
    if((msg->getSeqnum() < _seqnum) && (msg->getSeqnum() > (_seqnum + (msg->getWindow() * 7)))){
        cout << "Invalid sequence number" << endl;
        return false;
    }
    
    //Check mac
    if(_sessionKey != NULL){    //Expect v2
        
        //Must be v2 if key exists
        if(msg->getVersion() != 2){
            return false;
        }
        cout << "[SESSION] Invalid MAC" << endl;
    }
    return true;
}


/**
 * Handle received message
 * @param msg Pointer to the received message
 * @return True if message is valid
 */
bool SessionHandler::newMessage(const Message* msg){

    //Check message validity
    if(!isValidMessage(msg)){
        cout << "[SESSION] Invalid message" << endl;
        return true;
    }
    // Reset idle timer
    _timer.start();
    
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
            return !quitHandler(msg);
        case TYPE_NACK:
            cout << "[SESSION] Received NACK message" << endl;
            break;
        default:
            cout << "[SESSION] Unknown message type" << endl;
            break;
    }
    return true;
}


/**
 * Handle DESCR message
 * @param msg Pointer to the message
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
    string clientDiff = mFile.getDiff(clientFile);
    string serverDiff = clientFile.getDiff(mFile);
    
    cout << "[SESSION] Diff file:" << endl << clientDiff << endl;
    reply.setPayload(clientDiff.c_str(), (int)clientDiff.length());
    
    //reply->printInfo();
    
    //Send diff
    _trns->send(&reply, SERVER_TIMEOUT_SEND);
    
    //cout << "[SESSION] Client diff: " << endl << clientDiff << endl;
    //cout << "[SESSION] Server diff: " << endl << serverDiff << endl;
    
    //Add new file source for the server if client has files that server does not
    if(!serverDiff.empty()){
        
        const struct sockaddr* cliAddr = msg->getAddr();
        stringstream ss;
        ss << atoi(Networking::getPortStr(cliAddr).c_str()) - 1;    //Server port is source port - 1
        
        _server->addSource(Networking::getAddrStr(cliAddr), ss.str());
    }
    
    cout << "[SESSION] Description handler finished" << endl;
}



/**
 * Function for requesting next free client id
 * @return Free ID or -1 if no free IDs are available
 */
int SessionHandler::getFreeFlow(){
    
    for(int i = 0; i < SESSIONHANDLER_MAX_TRANSFERS; i++){
        if(_fFlows[i] == NULL){
            return i;
        }
    }
    return -1;
}


/**
 * Check if Element file is being transferred already
 * @param file Pointer to the element
 * @return True if file is being transferred
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

/**
 * Send reply ACK to a message
 * @param file Pointer to the received message
 */
void SessionHandler::sendAck(const Message* msg){
    Message reply(*msg);
    reply.setType(TYPE_ACK);
    _trns->send(&reply, SERVER_TIMEOUT_SEND);
}


/**
 * Send reply NACK to a message
 * @param file Pointer to the received message
 */
void SessionHandler::sendNack(const Message* msg){
    Message reply(*msg);
    reply.setType(TYPE_NACK);
    _trns->send(&reply, SERVER_TIMEOUT_SEND);
}


/**
 * Parse a GET message to request chunks
 * @param msg Received GET message
 * @param file Pointer to the requested element
 * @param chunkBegin First chunk
 * @param chunkEnd Last Chunk
 * @return True if requested file was found
 */
bool SessionHandler::parseGet(const Message* msg, Element* file, uint32_t* chunkBegin, uint32_t* chunkEnd){
    
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


/**
 * Handle received GET message
 * @param msg Received message
 */
void SessionHandler::getHandler(const Message* msg){
    
    //cout << "[SESSION] Get handler started: " << msg->getPayload() << endl;
    cout << "[SESSION] Get handler started" << endl;
    
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
    if(flowIdx < 0){    //Check that there are free flows
        cout << "[SESSION] No free flows available for file" << file.getName() << endl;
        return sendNack(msg);
    }
    cout << "[SESSION] Flow " << flowIdx << " allocated for file " << file.getName() << endl;
    
    try {
        _fFlows[flowIdx] = new FileTransfer(_trns, file, 0, 0, 0, FILE_TRANSFER_TYPE_SERVER);
    } catch (...) {
        cout << "[SESSION] File could not be opened" << endl;
        return sendNack(msg);
    }

    //Send ACK
    //cout << "[SESSION] Requested file " << file.getName() << endl;
    sendAck(msg);
    
    //Send first window
    _fFlows[flowIdx]->sendFile(msg);
}


/**
 * Handle file transfer
 * @param msg Received message
 */
void SessionHandler::fileHandler(const Message* msg){
    
    cout << "[SESSION] File handler started" << endl;
    
    int flowID = 0;
    //Chooce file transfer from messages flow index
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


/**
 * Creates a 32-byte session key from two nonces and the secret key
 * @param nonce1 First 16-byte random nonce
 * @param nonce2 Second 16-byte random nonce
 * @param secretKey Stored 64-byte secret key
 */ 
void SessionHandler::createSessionKey(unsigned char* nonce1, unsigned char* nonce2, unsigned char* secretKey){
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


/**
 * Handler for session termination
 * @param msg Received message
 * @return True if the session was properly terminated
 */
bool SessionHandler::quitHandler(const Message* msg){
    
    cout << "[SESSION] Terminate handler started" << endl;
    
    //Termination is initiated
    if(msg->getType() == TYPE_QUIT){
        
        //Reply with final QUITACK
        Message reply(*msg);
        reply.setType(TYPE_ACK);
        reply.setQuit(true);
        _trns->send(&reply, SERVER_TIMEOUT_SEND);
        return true;
    }
    return false;
}


/**
 * Check if connection has been idle too long
 * @return True if expired
 */
bool SessionHandler::isExpired() {
    
    if (_timer.elapsed_s() >= SESSION_TIMEOUT) {
        return true;
    }
    return false;
}
