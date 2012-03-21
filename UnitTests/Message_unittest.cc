//
//  Message_unittest.cc
//  SyncCFT
//
//  Created by Elo Matias on 25.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include "Message.hh"
#include "gtest/gtest.h"


// Test creating header with all zero values.
TEST(MessageTest, AllZeroHeader) {
    
    Message msg;
    msg.initHeader(0, 0, 0, 0, 0, 0, 0, 0);
    char* header;
    header = msg.parseBytes();
    
    const uint8_t mask[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    for (int i = 0; i < 16; i++)
        EXPECT_TRUE((memcmp(&header[i], &mask[i], 1)) == 0);
}

// Test creating header with all maximum values.
TEST(MessageTest, MaxValues) {
    
    Message msg;
    msg.initHeader(7, 255, 255, 255, 65535, 65535, 4294967295, 4294967295);
    char* header;
    header = msg.parseBytes();
    const uint8_t mask[16] = {0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    
    for (int i = 0; i < 16; i++)
        EXPECT_TRUE((memcmp(&header[i], &mask[i], 1)) == 0);
}
