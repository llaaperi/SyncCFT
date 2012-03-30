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

#include "Element.hh"

#define METAFILE ".sync.cft"
#define FT_FILE     8
#define FT_FOLDER   4


using namespace std;

class MetaFile {    
    const string _fileName;
    fstream _metafile;
    list<Element> _metadata;
    
public:
    list<Element>& getData() {return _metadata;};
    
    MetaFile(string fName);
    MetaFile(const char* buffer, int len);
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
    
    
    /*
     * Diff
     */
    string getDiff(MetaFile& other);
    
    
    /*
     *
     */
    string getDescr();
    
    
    /*
     * Return the metadata
     */
    list<Element> getMetadata() const {return _metadata;}

    
private:
    // Rule of three
    MetaFile(MetaFile const& other);
    MetaFile& operator=(MetaFile const& other);

};

/*
 * Print metadata to stream
 */
ostream& operator<<(ostream& os, const MetaFile& m);

inline bool operator==(const Element& lhs, const Element& rhs) { 
    if (lhs.getName() != rhs.getName())
        return false;
    if (lhs.getSize() != rhs.getSize())
        return false;
    if (lhs.getHash() != rhs.getHash())
        return false;
    if (lhs.getTimeStamp() != rhs.getTimeStamp())
        return false;
    return true; 
}

#endif
