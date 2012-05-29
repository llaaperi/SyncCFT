//
//  Element.hh
//  SyncCFT
//
//  Created by Elo Matias on 30.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Element_hh
#define SyncCFT_Element_hh

using namespace std;

class Element {
    string _name;  
    size_t _size;
    string _hash;
    time_t _timestamp;  
    
public:
    Element(){};
    /**
     * Constructor
     * @param name Filename
     * @param size File size
     * @param hash 16-byte MD5 hash from the file
     * @param timestamp Last modified (POSIX time)
     */
    Element(string name, size_t size, string hash, time_t timestamp) : 
        _name(name), _size(size), _hash(hash), _timestamp(timestamp) {};
    ~Element(){};
    
    /**
     * Element copy constructor
     */
    Element(Element const& other);
    
    // Getters
    string getName() const {return _name;}
    size_t getSize() const {return _size;}
    string getHash() const {return _hash;}
    time_t getTimeStamp() const {return _timestamp;}
    
    // Setters
    void setName(string const& newName) {_name = newName;}
    void setSize(size_t const size) {_size = size;}
    void setHash(string const& newHash) {_hash = newHash;}
    void setTimeStamp(time_t const timestamp) {_timestamp = timestamp;}
    
    /**
     * Compare two elements
     * @param other The element being compared to
     * @return Returns 0 if elements are identical, +1 if this element is newer, or
     *  -1 if the other element is newer or same age
     */
    int compare(Element const& other) const;
};

#endif
