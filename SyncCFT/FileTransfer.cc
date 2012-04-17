//
//  FileTransfer.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 30.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "FileTransfer.hh"

/*
 * FileTransfer constructor
 */
FileTransfer::FileTransfer(Transceiver* trns, Element file, int seqnum) : _trns(trns), _file(file), _seqnum(seqnum){
    cout << "[FILE] Transfer created" << endl;
}

FileTransfer::~FileTransfer(){
    cout << "[FILE] Transfer destroyed" << endl;
}


bool FileTransfer::transferFile(Message* msg)
{
    
    
    
    
    
    return false;
}

bool FileTransfer::sendChunk(unsigned long chunk)
{
    
    
    
    
    return true;
}
