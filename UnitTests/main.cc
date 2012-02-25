//
//  main.cc
//  SyncCFT
//
//  Created by Elo Matias on 25.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include "gtest/gtest.h"

#include "utilities.hh"

int main(int argc, char **argv) {
        
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
