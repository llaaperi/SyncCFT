//
//  utilities.cc
//  SyncCFT
//
//  Created by Elo Matias on 18.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//


#include "utilities.hh"

#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

double _markov_p = 0, _markov_q = 0;


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
 *
 */
void Utilities::initMarkov(const string& p, const string& q){
    
    double p_val = atof(p.c_str());
    double q_val = atof(q.c_str());
    
    //Only q is defined -> set p to same value
    if(p_val < 0 && q_val > 0){
        p_val = q_val;
    }
    
    //Only p is defined -> set q to same value
    if(q_val < 0 && p_val > 0){
        q_val = p_val;
    }
    
    //If p < 0, so is q and both will be initialized to 0
    if(p_val < 0){
        p_val = 0; q_val = 0;
    }
    
    //Check that values do not exceed 1.0
    if(p_val > 1.0){
        p_val = 1.0;
    }
    
    if(q_val > 1.0){
        q_val = 1.0;
    }
    
    _markov_p = p_val;
    _markov_q = q_val;
    
    cout << "Markov process initialized with values p=" << _markov_p << " q=" << _markov_q << endl;
}


/*
 * Markov process
 */
bool Utilities::isPacketLost() {
    
    //cout << "Calculate packet loss p=" << _markov_p << " q=" << _markov_q << endl;  
    
    static bool init = true, state = STATE_NOT_LOST;
    if (init) { 
        srand((unsigned int)time(0));
        init = false;
    }
    
    double randomValue = (double)rand()/(double)RAND_MAX;
    
    if (state == STATE_LOST) {
        if (randomValue < _markov_q)
            return true;
        else 
            return false;
    } else {
        if (randomValue < _markov_p)
            return true;
        else
            return false;
    }
}


/* Calculates 16-byte MD5 hash from the given file
 * @param filename Name of the file
 * @param length Size of the file
 * @param hash Store the hash here
 * @return 16-byte has hash as string
 */
bool Utilities::MD5Hash(string const& filename, long const length, string& hash) {
    
    int fd;
    char* fileBuf;
    unsigned char result[MD5_DIGEST_LENGTH];
    
    fd = open(filename.c_str(), O_RDONLY);
    if(fd < 0)
        return false;
    
    struct stat stats;
    stat(filename.c_str(), &stats);
    
    cout << "Length = " << length << ", statsLen = " << stats.st_size << endl;
    
    // Map file to memory
    fileBuf = (char*)mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
    MD5((unsigned char*)fileBuf, length, result);
    munmap(fileBuf, length);
    close(fd);
    
    // Convert hash to string
    char values[2];
    ostringstream hashStream;
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(values,"%02x", result[i]);
        hashStream << values;
    }
    hash = hashStream.str();
    return true;
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
        readBytes = (int)fread(ptr, 1, len, keyFile);
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
