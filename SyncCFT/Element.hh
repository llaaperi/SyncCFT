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
    Element(Element const& other);
    Element(string name, size_t size, string hash, time_t timestamp) : 
        _name(name), _size(size), _hash(hash), _timestamp(timestamp) {};
    ~Element(){};
    
    string getName() const {return _name;}
    void setName(string const& newName) {_name = newName;}
    size_t getSize() const {return _size;}
    void setSize(size_t const size) {_size = size;}
    string getHash() const {return _hash;}
    void setHash(string const& newHash) {_hash = newHash;}
    time_t getTimeStamp() const {return _timestamp;}
    void setTimeStamp(time_t const timestamp) {_timestamp = timestamp;}
    int compare(Element const& other) const;
};

#endif
