//
//  networking_unittest.cc
//  SyncCFT
//
//  Created by Elo Matias on 25.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include "networking.hh"
#include "gtest/gtest.h"

// To use a test fixture, derive a class from testing::Test.
class NetworkingTest : public testing::Test {
protected:
    virtual void SetUp() {
        serverSocket = Networking::createUnconnectedSocket("55500");
        clientSocket = Networking::createConnectedSocket("127.0.0.1", "55500");
    }
    
    virtual void TearDown() {
        close(serverSocket);
        close(clientSocket);
    }
    
    int serverSocket;
    int clientSocket;
};

// Test creating server socket.
TEST_F(NetworkingTest, CreateServerSocket) {
    EXPECT_GT(serverSocket, 0);
}

// Test creating client socket.
TEST_F(NetworkingTest, CreateClientSocket) {
    EXPECT_GT(clientSocket, 0);
}

// Test sending message from client to server.
TEST_F(NetworkingTest, ClientToServerMsg) {
    
    struct sockaddr cliAddr;
    char data[] = "TESTIVIESTI"; // Length 12
    char buffer[1500];
    int status;
    
    // Send message to server
    status = (int)write(clientSocket, data, sizeof(data));
    EXPECT_EQ(status, 12);
    
    // Receive message
    status = Networking::receivePacket(serverSocket, buffer, &cliAddr, 5);
    EXPECT_EQ(status, 12);
    EXPECT_TRUE(!strcmp(buffer, data));

}

// Test responding to client message.
TEST_F(NetworkingTest, ServerResponse) {
    
    struct sockaddr cliAddr;
    char data[] = "TESTIVIESTI"; // Length 12
    char data2[] = "TESTIVIESTI2"; // Length 13
    char buffer[1500];
    char buffer2[1500];
    int status;
    
    // Send message to server
    status = (int)write(clientSocket, data, sizeof(data));
    EXPECT_EQ(status, 12);
    
    // Receive message
    status = Networking::receivePacket(serverSocket, buffer, &cliAddr, 5);
    EXPECT_EQ(status, 12);
    EXPECT_TRUE(!strcmp(buffer, data));
    
    status = Networking::sendPacket(serverSocket, data2, sizeof(data2), &cliAddr, 5);
    EXPECT_EQ(status, 13);
    Networking::receivePacket(clientSocket, buffer2, &cliAddr, 5);
    EXPECT_EQ(status, 13);
    EXPECT_TRUE(!strcmp(buffer2, data2));

}


TEST_F(NetworkingTest, CompareAddress){
    
    struct sockaddr cliAddr;
    char data[] = "TESTIVIESTI"; // Length 12
    char buffer[1500];
    
    //Get sockaddr struct
    write(clientSocket, data, sizeof(data));
    Networking::receivePacket(serverSocket, buffer, &cliAddr, 5);
    
    //Comapre NULL addresses
    EXPECT_FALSE(Networking::cmpAddr(NULL, NULL));
    EXPECT_FALSE(Networking::cmpAddr(&cliAddr, NULL));
    EXPECT_FALSE(Networking::cmpAddr(NULL, &cliAddr));
    
    //Matching addresses
    EXPECT_TRUE(Networking::cmpAddr(&cliAddr, &cliAddr));
    
    //Mismatching ports
    struct sockaddr cliAddr2;
    memcpy(&cliAddr2, &cliAddr, sizeof(struct sockaddr));
    ((sockaddr_in*)&cliAddr2)->sin_port = 500;
    EXPECT_FALSE(Networking::cmpAddr(&cliAddr, &cliAddr2));
    EXPECT_FALSE(Networking::cmpAddr(&cliAddr2, &cliAddr));
    
    //Mismatching addresses
    struct sockaddr cliAddr3;
    memcpy(&cliAddr3, &cliAddr, sizeof(struct sockaddr));
    ((sockaddr_in*)&cliAddr3)->sin_addr.s_addr = 500;
    EXPECT_FALSE(Networking::cmpAddr(&cliAddr, &cliAddr3));
    EXPECT_FALSE(Networking::cmpAddr(&cliAddr3, &cliAddr));
}

