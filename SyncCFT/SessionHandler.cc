//
//  SessionHandler.cc
//  SyncCFT
//
//  Created by Elo Matias on 3/23/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "SessionHandler.hh"


/*
 * Constructor
 */
SessionHandler::SessionHandler(uint8_t id, sockaddr cliAddr) : _id(id), _cliAddr(cliAddr){
    cout << "[SESSION] New session with id " << (unsigned int)id << " accepted"<< endl;
}


/*
 * Destructor
 */
SessionHandler::~SessionHandler(){
    cout << "[SESSION] Session " << (unsigned int)_id << " terminated"<< endl;
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