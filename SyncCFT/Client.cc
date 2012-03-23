//
//  Client.cc
//  SyncCFT
//
//  Created by Elo Matias on 4.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Client.hh"
#include "networking.hh"

Client::Client(list<string>& hosts, string port) throw(invalid_argument, runtime_error) : _port(port){
    
    if(true){
        
    }
    else{
        throw invalid_argument("Invalid arguments");
    }
    
    _socket = Networking::createUnconnectedSocket(_port);
    if(_socket < 0){
        throw runtime_error("Socket creation failed");
    }
    
}

/*
 * Start thread for handling clients
 */
void Client::start(void)
{
    _running = true;
    pthread_create(&_thread, NULL, handle, this);
    pthread_detach(_thread);
}


/*
 * Stop thread for handling clients
 */
void Client::stop(void) {
    _running = false;
    pthread_join(_thread, NULL);
    cout << "Client terminated" << endl;
}



/*
 * Main function for client thread
 */
void* Client::handle(void* arg)
{
    Client* handler = (Client*)arg;
    
    cout << "Client handler" << endl;
    
    char sendBuffer[NETWORKING_MTU];
    char recvBuffer[NETWORKING_MTU];
    
    struct addrinfo hints, *serverInfo, *tempInfo;
    
    bzero(&hints, sizeof(struct addrinfo)); // Zero struct values
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    // Get address information in struct addrinfo format. Works for IPv4 and IPv6.
    int temp;
    temp = getaddrinfo("localhost", "5062", &hints, &serverInfo);
    if (temp != 0) {
        perror("Running getaddrinfo failed.");
        return 0;
    }
    
    int i = 1, bytes = 0;
    while(handler->_running){
        
        Message msg;
        msg.initHeader(TYPE_HELLO);
        msg.parseToBytes(sendBuffer);
        
        //bytes = sendto(handler->_socket, sendBuffer, strlen(sendBuffer), 0, serverInfo->ai_addr, serverInfo->ai_addrlen);
        bytes = Networking::sendPacket(handler->_socket, sendBuffer, HEADER_SIZE, serverInfo->ai_addr, CLIENT_TIMEOUT_SEND);
        
        if(bytes < 0){
            cout << "Packet send failed" << endl;
        }else{
            cout << "Packet sent" << endl;
        }
        
        bytes = Networking::receivePacket(handler->_socket, recvBuffer, serverInfo->ai_addr, CLIENT_TIMEOUT_HELLO);
        
        if(bytes > 0){
            
            cout << "Received message" << endl;
            msg.parseFromBytes(recvBuffer, bytes);
            
            if(msg.getType() == TYPE_ACK){
                
                cout << "Received ack" << endl;
                
                msg.incrSeqnum();
                msg.setPayload(NULL, 0);
                msg.parseToBytes(sendBuffer);
                bytes = Networking::sendPacket(handler->_socket, sendBuffer, HEADER_SIZE, serverInfo->ai_addr, CLIENT_TIMEOUT_SEND);
                
                //Check bytes if send correctly
                
                bytes = Networking::receivePacket(handler->_socket, recvBuffer, serverInfo->ai_addr, CLIENT_TIMEOUT_HELLO);
                
                if(bytes > 0){
                    
                    msg.parseFromBytes(recvBuffer, bytes);
                    if(msg.getType() == TYPE_ACK){
                        cout << "Handshake succesfull" << endl;
                    }else{
                        cout << "Handshake failed" << endl;
                    }
                    
                }else{
                    cout << "HELLOACK not received" << endl;
                }
                
            }
        
        }
        
        sleep(5);
    }
    return 0;
}
