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
        serverSocket = Networking::createServerSocket("55500");
        clientSocket = Networking::createClientSocket("127.0.0.1", "55500");
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
    
    struct sockaddr_in cliAddr;
    char data[] = "TESTIVIESTI"; // Length 12
    char buffer[1500];
    int status;
    
    // Send message to server
    status = write(clientSocket, data, sizeof(data));
    EXPECT_EQ(status, 12);
    
    // Receive message
    status = Networking::receivePacket(serverSocket, buffer, &cliAddr);
    EXPECT_EQ(status, 12);
    EXPECT_TRUE(!strcmp(buffer, data));

}

// Test responding to client message.
TEST_F(NetworkingTest, ServerResponse) {
    
    struct sockaddr_in cliAddr;
    char data[] = "TESTIVIESTI"; // Length 12
    char data2[] = "TESTIVIESTI2"; // Length 13
    char buffer[1500];
    char buffer2[1500];
    int status;
    
    // Send message to server
    status = write(clientSocket, data, sizeof(data));
    EXPECT_EQ(status, 12);
    
    // Receive message
    status = Networking::receivePacket(serverSocket, buffer, &cliAddr);
    EXPECT_EQ(status, 12);
    EXPECT_TRUE(!strcmp(buffer, data));
    
    status = Networking::sendPacket(serverSocket, data2, sizeof(data2), &cliAddr);
    EXPECT_EQ(status, 13);
    Networking::receivePacket(clientSocket, buffer2, &cliAddr);
    EXPECT_EQ(status, 13);
    EXPECT_TRUE(!strcmp(buffer2, data2));

}

