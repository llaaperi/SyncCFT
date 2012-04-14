//
//  SessionHandler_unittest.cc
//  SyncCFT
//
//  Created by Elo Matias on 11.4.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "SessionHandler.hh"
#include "utilities.hh"

#include "gtest/gtest.h"


/* NOTE: MessageHandler desctructor have to be disabled for simple testing */

/*
 * Test session key calculation
 */
TEST(SessionHandlerTest, SessionKeyGenerationTest)
{
    
    SessionHandler handler;
    
    unsigned char nonce1[16] = "nonce1";
    unsigned char nonce2[16] = "nonce2";
    unsigned char secretkey[64] = "salainenavain";
    
    handler.createSessionKey(nonce1, nonce2, secretkey);
    
    const unsigned char* key = handler.getSessionKey();
    
    /*
    cout <<"[ ";
    for (int i = 0; i < 32; i++)
        printf("%02x ", key[i]);
    cout << "]" << endl;
    */
    
    const uint8_t sampleKey[32] = {0x82, 0x8e, 0xf0 , 0x0f , 0x42 , 0x04 , 0x12 , 0xcf , 0x54 , 0x48 , 0x99 , 0x3c , 0x10 , 0x3f , 0x34 , 0xc5 , 0xef , 0x6f , 0x0e , 0xeb , 0xd1 , 0xd7 , 0x61 , 0x92 , 0x3d , 0x3a , 0x45 , 0xd6 , 0x15 , 0xbf , 0xb5 , 0x43};
    
    for (int i = 0; i < 32; i++)
        EXPECT_TRUE((memcmp(&key[i], &sampleKey[i], 1)) == 0);
    /*
    Utilities::randomBytes(nonce1, 16);
    Utilities::randomBytes(nonce2, 16);
    Utilities::randomBytes(secretkey, 64);
    */

}
