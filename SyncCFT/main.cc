//
//  main.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Metafile.hh"

#define METAFILE ".sync.cft"

int main (int argc, const char * argv[])
{
    std::cout << "*** SyncCTF launched ***" << std::endl;
    
    //Print folder info
    MetaFile mFile(METAFILE);
    mFile.print();
    
    // Start client first
    
    
    // Pass reference to client object to the server
    
    
    return 0;
}

