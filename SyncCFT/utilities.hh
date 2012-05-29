//
//  utilities.hh
//  SyncCFT
//
//  Created by Elo Matias on 18.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_utilities_hh
#define SyncCFT_utilities_hh

#include <string>
#include <vector>

#define STATE_LOST 0
#define STATE_NOT_LOST 1
#define HASH_LENGTH 32

#define DEFAULT_KEYFILE ".sync.cft.key"

using namespace std;

namespace Utilities {
    
    /**
     * Splits given string using separator. Results are saved in a given vector.
     * @param str Source string for the method
     * @param separator String to be used as separator
     * @param results Vector where to save the results
     * @return Size of vector results
     */
    int split(string str, string separator, vector<string>& results);
    
    /**
     * Initialize the Markov process
     * @param p Probability paramater P
     * @param q Probability paramater Q
     */
    void initMarkov(const string& p, const string& q);
    
    /**
     * Markov process
     * Extern variables (_markov_p and _markov_q) must be initialized before use
     * @return True if packet is lost
     */
    bool isPacketLost();
    
    /** 
     * Calculates 16-byte MD5 hash from the given file
     * @param filename Name of the file
     * @param hash Store the hash here
     * @return 16-byte has hash as string
     */
    bool MD5Hash(string const& filename, string& hash);
    
    /**
     * Calculates a 32-byte SHA256 hash from the given data
     * @param ptr A buffer where to store the hash
     * @param msg Pointer to message data
     * @param len Length of message data
     */
    void SHA256Hash(unsigned char* ptr, unsigned char const* msg, long len);
    
    /**
     * Create N bytes of random data
     * @param ptr Buffer for storing the random data
     * @param len Length of the buffer
     */
    void randomBytes(unsigned char* ptr, int len);
    
    /**
     * Calculate hash from nonce and secret key
     * @param result 32-byte buffer for storing the hash
     * @param nonce 16-byte nonce
     * @param key 64-byte secret key
     */
    void nonceHash(unsigned char* result, const unsigned char* nonce, const unsigned char* key);
    
    /**
     * Create session key from two nonces and the secret key
     * @param sNonce 16-byte server nonce
     * @param cNonce 16-byte client nonce
     * @param key 64-byte secret key
     * @return  32-byte session key
     */
    unsigned char* sessionKey(const unsigned char* sNonce, const unsigned char* cNonce, const unsigned char* key);
    
    /**
     * Get N bit secret key from storage or create a new secret
     * @param ptr An address where to store the the secret 
     * @param len Length of the secret
     * @param fName Name of file storing the secret, if NULL create a new secret
     *  to a default file
     * @return true if key file was loaded, false if new key was created
     */
    bool getSecretKey(unsigned char* ptr, int len, const char* fName);
    
    
    /**
     * Print N bytes of binary data to stdout as hex values
     * @param ptr Pointer to the data
     * @param len Length of the data
     */
    void printBytes(unsigned char* ptr, int len);
}
#endif
