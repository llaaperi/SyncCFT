//
//  SessionHandler.cc
//  SyncCFT
//
//  Created by Elo Matias on 3/23/12.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "SessionHandler.hh"

SessionHandler::SessionHandler(uint8_t id, sockaddr cliAddr) : _id(id), _cliAddr(cliAddr){
    
    cout << "[SERVER] New session with id " << (unsigned int)id << " accepted"<< endl;
    
}

void SessionHandler::newMessage(Message* msg){

    switch (msg->getType()) {
        case TYPE_ACK:
            cout << "Received ACK message" << endl;
            break;
        case TYPE_HELLO:
            cout << "Received HELLO message" << endl;
            break;
        case TYPE_DESCR:
            cout << "Received DESCR message" << endl;
            break;
        case TYPE_DIFF:
            cout << "Received DIFF message" << endl;
            break;
        case TYPE_GET:
            cout << "Received GET message" << endl;
            break;
        case TYPE_FILE:
            cout << "Received FILE message" << endl;
            break;
        case TYPE_QUIT:
            cout << "Received QUIT message" << endl;
            break;
        case TYPE_NACK:
            cout << "Received NACK message" << endl;
            break;
        default:
            cout << "Unknown message type" << endl;
            break;
    }
}


/*
 * Function for checking if source is valid for this session
 */
bool SessionHandler::isValidSource(){
    return true;
}