//
//  main.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "metafile.hh"

#define METAFILE ".sync.cft"

int main (int argc, const char * argv[])
{
    std::cout << "*** SyncCTF launched ***" << std::endl;
    
    //Print folder info
    MetaFile mFile(METAFILE);
    mFile.print();
    
    return 0;
}

