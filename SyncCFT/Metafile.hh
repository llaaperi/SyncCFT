//
//  metafile.hh
//  SyncCFT
//
//  Created by Elo Matias on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_filesys_hh
#define SyncCFT_filesys_hh

#include <list>
#include <fstream>

#define FT_FILE     8
#define FT_FOLDER   4


using namespace std;

class Element {
    string mName;  
    int mSize;
    string mHash;
    int mTimestamp;  
    
public:
    string getName() const {return mName;}
    void setName(string const& newName) {mName = newName;}
    size_t getSize() const {return mSize;}
    void setSize(size_t const size) {mSize = size;}
    string getHash() const {return mHash;}
    void setHash(string const& newHash) {mHash = newHash;}
    time_t getTimeStamp() const {return mTimestamp;}
    void setTimeStamp(time_t const timestamp) {mTimestamp = timestamp;}
    
};

class MetaFile {    
    const string fileName;
    fstream metafile;
    list<Element> metadata;
    
public:
    list<Element>& getData() {return metadata;};

    
    MetaFile(string fName);
    ~MetaFile(){}
    
    /*
     * Find element
     * @param name Name of the element
     * @param found Success status of search
     * @return Reference to the found element
     */
    Element& find(string const& name, bool& found);
    
    /*
     * Convert string to Element
     */
    bool strToElement(string const& line, Element& file);
    
    /*
     * Convert Element to string
     */
    string elementToStr(Element const& file) const;
    
    /* Calculates 16-byte MD5 hash from the given file
     * @param filename Name of the file
     * @param length Size of the file
     * @param hash Store the hash here
     * @return 16-byte has hash as string
     */
    static bool MD5Hash(string const& filename, long const length, string& hash);
    
    /*
     * Read the contents of the metatile
     */
    void read(void);
    
    /*
     * Overwrite to the metatile
     */
    bool write(void);
    
    /*
     * Updates the contents of metafile and metadata
     */
    bool updateAll(void);
    
    /*
     * Print metadata
     */
    void print(void) const;
    
private:
    // Rule of three
    MetaFile(MetaFile const& other);
    MetaFile& operator=(MetaFile const& other);

};



#endif
