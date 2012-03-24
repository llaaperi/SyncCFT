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
    //msg.initHeader(0, 0, 0, 0, 0, 0, 0, 0);
    char header[HEADER_SIZE];
    msg.parseToBytes(header);
    
    //msg.printBytes();
    //msg.printInfo();
    
    const uint8_t mask[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    for (int i = 0; i < 16; i++)
        EXPECT_TRUE((memcmp(&header[i], &mask[i], 1)) == 0);
}

// Test creating header with all maximum values.
TEST(MessageTest, MaxValues) {
    
    Message msg;
    //msg.initHeader(7, 255, 255, 255, 65535, 65535, 4294967295, 4294967295);
    msg.setVersion(7);
    msg.setType((MsgType)255);
    msg.setClientID(255);
    msg.setChecksum(255);
    msg.setWindow(65535);
    msg.setSeqnum(4294967295);
    msg.setChunk(4294967295);
    msg.setPayload(NULL, 0);
    
    char header[HEADER_SIZE];
    msg.parseToBytes(header);
    const uint8_t mask[16] = {0xe0, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    
    for (int i = 0; i < 16; i++)
        EXPECT_TRUE((memcmp(&header[i], &mask[i], 1)) == 0);
}

/*
 *
 */
TEST(MessageTest, parseFromBytes){

    const char buffer[16] = {0x21, 0x01, 0x02, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02};
    
    Message msg;
    msg.parseFromBytes(buffer, 16);
    
    //msg.printInfo();
    //msg.printBytes();
    
    EXPECT_EQ(1, msg.getVersion());
    EXPECT_TRUE(msg.isFirst());
    EXPECT_FALSE(msg.isLast());
    EXPECT_EQ(1, msg.getType());
    EXPECT_EQ(2, msg.getClientID());
    EXPECT_EQ(255, msg.getChecksum());
    EXPECT_EQ(0, msg.getPayloadLength());   //Payload is NULL -> parseFromBytes sets length to 0
    EXPECT_EQ(65280, msg.getWindow());
    EXPECT_EQ(1, msg.getSeqnum());
    EXPECT_EQ(2, msg.getChunk());
}
