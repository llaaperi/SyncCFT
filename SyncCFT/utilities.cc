//
//  utilities.cc
//  SyncCFT
//
//  Created by Elo Matias on 18.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//


#include "utilities.hh"

#include <iostream>
#include <sys/time.h>
#include <time.h>

#include <openssl/sha.h>
#include <openssl/rand.h>

int Utilities::split(string str, string separator, vector<string>& results) {
    results.clear();
    unsigned long found = str.find_first_of(separator);
    if (found == string::npos) {
        results.push_back(str);
        return (int)results.size();
    }
    while(found != string::npos){
        if(found > 0)
            results.push_back(str.substr(0,found));
        str = str.substr(found+1);
        found = (int)str.find_first_of(separator);
    }
    if(str.length() > 0)
        results.push_back(str);
    	
    return (int)results.size();
}


/*
 * Markov process
 */
bool Utilities::packetLost(int state, double p, double q) {
    
    static bool init = true;
    if (init) { 
        srand((unsigned int)time(0));
        init = false;
    }
    
    double randomValue = (double)rand()/(double)RAND_MAX;
    cout << "Random value: " << randomValue << endl;
    
    if (state == STATE_LOST) {
        if (randomValue < q)
            return true;
        else 
            return false;
    } else {
        if (randomValue < p)
            return true;
        else
            return false;
    }
}

/* Calculates a 32-byte SHA256 hash from the given data
 * @ptr A buffer where to store the hash
 * @msg Pointer to message data
 * @len Length of message data
 * @return A pointer to the 32-byte hash
 */
void Utilities::SHA256Hash(unsigned char* ptr, unsigned char const* msg, long len)
{
    SHA256_CTX context;
    
    SHA256_Init(&context);
    SHA256_Update(&context, msg, len);
    SHA256_Final(ptr, &context);
}


/*
 * Create N bytes of random data
 * @ptr Buffer for storing the random data
 * @len Length of the buffer
 */
void Utilities::randomBytes(unsigned char* ptr, int len)
{
    if (!RAND_bytes(ptr, len)) {
        cout << "Rand bytes failed" << endl;
        ptr = NULL;
        return;
    }
    /*
    cout <<"[ ";
    for (int i = 0; i < length; i++)
        printf("%02x ", buf[i]);
    cout << "]" << endl;
    */
}


/*
 * Get N bit secret key 
 * @ptr An address where to store the the secret 
 * @len Length of the secret
 * @fName Name of file storing the secret, if NULL create a new secret
 *  to a default file
 */
void Utilities::getSecretKey(unsigned char* ptr, int len, const char* fName) {
    
    // Try to load an old secret key
    FILE* keyFile = NULL;
    int readBytes = 0;
    
    if (fName != NULL) {
        keyFile = fopen(fName, "r");
    }
    if (keyFile != NULL) {
        readBytes = fread(ptr, 1, len, keyFile);
        if (readBytes != len) {
            cout << "[MAIN] Failed to load old keyfile" << endl;
        } else {
            //cout << "[MAIN] Loaded old keyfile" << endl;
            fclose(keyFile);
            return;
        }
    }
    
    // Create new secret key
    keyFile = NULL;
    Utilities::randomBytes(ptr, len); // Create new random secret key
    //cout << "[MAIN] Created new secret key" << endl;
    keyFile = fopen(DEFAULT_KEYFILE, "w");
    if (keyFile == NULL) {
        cout << "[MAIN] Failed to open keyfile for writing" << endl;
    } else {
        fwrite(ptr, 1, len, keyFile);
        fclose(keyFile);
    }
}
