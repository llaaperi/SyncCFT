//
//  utilities_unittest.cc
//  SyncCFT
//
//  Created by Elo Matias on 29.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include "utilities.hh"
#include <string>
#include "gtest/gtest.h"


/*
 * Test MetaFile string constructor
 */
TEST(UtilitiesTest, MarkovChain) {
    
    //Utilities::packetLost(STATE_LOST, 0.5, 0.5);
    //Utilities::packetLost(STATE_NOT_LOST, 0.5, 0.5);
    //Utilities::packetLost(STATE_NOT_LOST, 0.5, 0.5);
    //Utilities::packetLost(STATE_NOT_LOST, 0.5, 0.5);
    //Utilities::packetLost(STATE_NOT_LOST, 0.5, 0.5);
    //Utilities::packetLost(STATE_NOT_LOST, 0.5, 0.5);
}

TEST(UtilitiesTest, getSecretKey) {
    
    unsigned char secretKey1[512];
    unsigned char secretKey2[512];
    
    Utilities::getSecretKey(secretKey1, 512, NULL);
    Utilities::getSecretKey(secretKey2, 512, DEFAULT_KEYFILE);

    for (int i = 0; i < 512; i++)
        EXPECT_TRUE((memcmp(&secretKey1[i], &secretKey2[i], 1)) == 0);
    
}

