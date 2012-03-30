//
//  Metafile_unittest.cc
//  SyncCFT
//
//  Created by Elo Matias on 27.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include "Metafile.hh"
#include <string>
#include "gtest/gtest.h"


/*
 * Test MetaFile string constructor
 */
TEST(MetafileTest, StringConstructor) {
    
    string testString = "SyncCFT;130276;434da2e09a8be42e58d43d6e0d5f2bac;1332846616\nSyncCFT copy;30036;4da40dcd1199938f42f5670cd0a56234;1329557571\nSyncCFT copy 2;30036;4da40dcd1199938f42f5670cd0a56234;1329557575\nSyncCFT copy 3;30036;4da40dcd1199938f42f5670cd0a56234;1329852728";
    MetaFile metadata(testString.c_str(), (int)testString.length());    
    
    bool found = false;
    Element asd = metadata.find("SyncCFT", found);
    EXPECT_TRUE(found);    
}


/*
 * Test MetaFile string constructor
 */
TEST(MetafileTest, Diff) {

    string testString1 = "SyncCFT;130276;434da2e09a8be42e58d43d6e0d5f2bac;1332846616\nSyncCFT copy;30036;4da40dcd1199938f42f5670cd0a56234;1329557571\nSyncCFT copy 2;30036;5da40dcd1199938f42f5670cd0a56234;1329557575\nSyncCFT copy 3;30036;4da40dcd1199938f42f5670cd0a56234;1329852729";
    MetaFile metadata1(testString1.c_str(), (int)testString1.length());
    
    string testString2 = "SyncCFT;130276;434da2e09a8be42e58d43d6e0d5f2bac;1332846616\nSyncCFT copy;30036;4da40dcd1199938f42f5670cd0a56234;1329557571\nSyncCFT copy 2;20036;4da40dcd1199938f42f5670cd0a56234;1329557571\nSyncCFT copy diff;30036;4da40dcd1199938f42f5670cd0a56234;1329852728";
    MetaFile metadata2(testString2.c_str(), (int)testString2.length());  

    
    //metadata1.print();
    //cout << metadata2 << endl;
    
    string difference = metadata1.getDiff(metadata2);
    
    //cout << "Difference:\n" << difference << endl;
    
    string rDifference = "SyncCFT copy 2;30036;5da40dcd1199938f42f5670cd0a56234;1329557575\nSyncCFT copy 3;30036;4da40dcd1199938f42f5670cd0a56234;1329852729";
    EXPECT_TRUE(difference == rDifference);
}


/*
 * Test MetaFile == operator
 */
TEST(MetafileTest, EqualsOperator) {
    
    Element ele1;
    ele1.setName("asd1");
    ele1.setSize(12);
    ele1.setTimeStamp(123);
    ele1.setHash("asdasdasd");
    
    Element ele2;
    ele2.setName("asd1");
    ele2.setSize(12);
    ele2.setTimeStamp(123);
    ele2.setHash("asdasdasd");

    EXPECT_TRUE(ele1 == ele2);    
}
