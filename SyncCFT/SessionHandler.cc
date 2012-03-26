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

/*
 * Constructor
 */
SessionHandler::SessionHandler(int socket, struct sockaddr* cliAddr, uint8_t id) : _id(id){
    cout << "[SESSION] New session with id " << (unsigned int)id << " accepted"<< endl;
    //Create new tranceiver for this session
    _trns = new Transceiver(socket, *cliAddr);
}


/*
 * Destructor
 */
SessionHandler::~SessionHandler(){
    cout << "[SESSION] Session " << (unsigned int)_id << " terminated"<< endl;
    delete(_trns);
}


void SessionHandler::newMessage(Message* msg){

    switch (msg->getType()) {
        case TYPE_ACK:
            cout << "[SESSION] Received ACK message" << endl;
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
bool SessionHandler::isValidSource(){
    return true;
}

/*
 * Handles DESCR messages
 */
void SessionHandler::descrHandler(Message* msg){
    
    cout << "[SESSION] Description handler started" << endl;
    
    MetaFile mFile(METAFILE);
    //mFile.print();
    
    msg->setType(TYPE_DIFF);
    msg->setFirst(true);
    msg->setLast(true);
    
    //Payload
    char buffer[NETWORKING_MTU];
    int idx = 0;
    list<Element> list = mFile.getData();
    
    for(Element e : list){
        string str = mFile.elementToStr(e);
        memcpy(&buffer[idx], str.c_str(), str.length());
        idx += str.length();
        buffer[idx++] = '\n';
    }
    buffer[idx++] = 0;
    msg->setPayload(buffer, idx);
    
    msg->printInfo();
    
    //Send diff
    _trns->send(msg, SERVER_TIMEOUT_SEND);
}