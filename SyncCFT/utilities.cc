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

/**
 * Splits given string using separator. Results are saved in a given vector.
 * @param str Source string for the method
 * @param separator String to be used as separator
 * @param results Vector where to save the results
 * @return Size of vector results
 */
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


/**
 * Initialize the Markov process
 * @param p Probability paramater P
 * @param q Probability paramater Q
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


/**
 * Markov process
 * Extern variables (_markov_p and _markov_q) must be initialized before use
 * @return True if packet is lost
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


/** 
 * Calculates 16-byte MD5 hash from the given file
 * @param filename Name of the file
 * @param hash Store the hash here
 * @return 16-byte has hash as string
 */
bool Utilities::MD5Hash(string const& filename, string& hash) {
    
    int fd;
    char* fileBuf;
    unsigned char result[MD5_DIGEST_LENGTH];
    
    fd = open(filename.c_str(), O_RDONLY);
    if(fd < 0)
        return false;
    
    struct stat stats;
    stat(filename.c_str(), &stats);
    
    //cout << "[UTILITIES] StatsLen = " << stats.st_size << endl;
    
    // Map file to memory
    fileBuf = (char*)mmap(0, stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
    MD5((unsigned char*)fileBuf, stats.st_size, result);
    munmap(fileBuf, stats.st_size);
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


/**
 * Calculates a 32-byte SHA256 hash from the given data
 * @param ptr A buffer where to store the hash
 * @param msg Pointer to message data
 * @param len Length of message data
 */
void Utilities::SHA256Hash(unsigned char* ptr, unsigned char const* msg, long len)
{
    SHA256_CTX context;
    
    SHA256_Init(&context);
    SHA256_Update(&context, msg, len);
    SHA256_Final(ptr, &context);
}


/**
 * Create N bytes of random data
 * @param ptr Buffer for storing the random data
 * @param len Length of the buffer
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


/**
 * Calculate hash from nonce and secret key
 * @param result 32-byte buffer for storing the hash
 * @param nonce 16-byte nonce
 * @param key 64-byte secret key
 */
void Utilities::nonceHash(unsigned char* result, const unsigned char* nonce, const unsigned char* key){
    
    unsigned char hashInput[80];    //Key (64) + nonce (16)
	memset(result, 0, 32);
    memcpy(hashInput, nonce, 16);
    memcpy(hashInput + 16, key, 64);
    /*
    cout << "[UTILITIES] Nonce:" << endl;
    printBytes((unsigned char*)nonce, 16);
    cout << endl;
    cout << "[UTILITIES] Key:" << endl;
    printBytes((unsigned char*)key, 64);
    cout << endl;
    cout << "[UTILITIES] Sum:" << endl;
    printBytes((unsigned char*)hashInput, 80);
    cout << endl;
    */
    Utilities::SHA256Hash(result, hashInput, 80);
    /*
    cout << "[UTILITIES] Hash:" << endl;
    printBytes((unsigned char*)result, 32);
    cout << endl;
    */
}


/**
 * Create session key from two nonces and the secret key
 * @param sNonce 16-byte server nonce
 * @param cNonce 16-byte client nonce
 * @param key 64-byte secret key
 * @return  32-byte session key
 */
unsigned char* Utilities::sessionKey(const unsigned char* sNonce, const unsigned char* cNonce, const unsigned char* key){
    
    unsigned char hashInput[96];    //Key (64) + sNonce (16) + cNonce (16)
    unsigned char* sessionKey = (unsigned char*)calloc(HASH_LENGTH, sizeof(unsigned char));
    if(sessionKey != NULL){
        memcpy(hashInput, cNonce, 16);
        memcpy(hashInput + 16, sNonce, 16);
        memcpy(hashInput + 32, key, 64);
    
        Utilities::SHA256Hash(sessionKey, hashInput, 96);
    }
    return sessionKey;
}


/**
 * Get N bit secret key from storage or create a new secret
 * @param ptr An address where to store the the secret 
 * @param len Length of the secret
 * @param fName Name of file storing the secret, if NULL create a new secret
 *  to a default file
 * @return true if key file was loaded, false if new key was created
 */
bool Utilities::getSecretKey(unsigned char* ptr, int len, const char* fName) {
    
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
            return true;
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
    return false;
}


/**
 * Print N bytes of binary data to stdout as hex values
 * @param ptr Pointer to the data
 * @param len Length of the data
 */
void Utilities::printBytes(unsigned char* ptr, int len){
    for(int i = 0; i < len; i++){
        printf("%02x ", ptr[i]);
    }    
}
