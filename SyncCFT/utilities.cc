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
 * @param A buffer where to store the hash
 * @param message Data
 * @return A pointer to the 32-byte hash
 */
void Utilities::SHA256Hash(unsigned char* buffer, unsigned char const* message, long length)
{
    SHA256_CTX context;
    
    SHA256_Init(&context);
    SHA256_Update(&context, message, length);
    SHA256_Final(buffer, &context);
}


/*
 * Create N bytes of random data
 * @param buf Buffer where to store the random data
 * @param lenth The length of the buffer
 */
void Utilities::randomBytes(unsigned char* buf, int length)
{
    if (!RAND_bytes(buf, length)) {
        cout << "Rand bytes failed" << endl;
        buf = NULL;
        return;
    }
    /*
    cout <<"[ ";
    for (int i = 0; i < length; i++)
        printf("%02x ", buf[i]);
    cout << "]" << endl;
    */
}

