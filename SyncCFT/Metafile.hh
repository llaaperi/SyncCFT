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

extern string _syncDir;

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
    
    /**
     * Find element
     * @name Name of the element
     * @found Success status of search
     * @return Reference to the found element
     **/
    Element& find(string const& name, bool& found);
    
    /**
     * Convert string to Element
     * @line Element info stored in a string
     * @file Reference to a Element where to save the info
     * @return Success status of the operation
     **/
    bool strToElement(string const& line, Element& file);
    
    /**
     * Convert Element to string
     * @file Reference to the Element from which to read the info
     * @return String containing the element info
     **/
    string elementToStr(Element const& file) const;
    
    /**
     * Read the contents of the metatile
     **/
    void read(void);
    
    /**
     * Overwrite to the metatile
     * @return Success status of the operation
     **/
    bool write(void);
    
    /**
     * Updates the contents of metafile and metadata
     * @return Success status of the operation
     **/
    bool updateAll(void);
    
    /**
     * Print metadata
     **/
    void print(void) const;
    
    
    /**
     * Compare two MetaFiles
     * @other MetaFile to compare
     * @return Difference of the MetaFiles as string
     **/
    string getDiff(MetaFile& other);
    
    
    /**
     * Get contents of Metafile
     * @return Contents of the Metafile as a string
     **/
    string getDescr();
    
    
    /**
     * Get the Metadata
     * @return List of all stored Elements
     **/
    list<Element> getMetadata() const {return _metadata;}

    
private:
    // Rule of three
    MetaFile(MetaFile const& other);
    MetaFile& operator=(MetaFile const& other);

};

/**
 * Print metadata to stream
 **/
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
