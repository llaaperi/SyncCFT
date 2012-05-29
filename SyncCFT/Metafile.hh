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
    
    
    /**
     * Construct new MetaFile object from stored MetaFile
     * @param fName Name of the saved MetaFile
     */
    MetaFile(string fName);
    
    /**
     * Constructor used to create new metafile from packet
     * payload
     * @param buffer Packet payload
     * @param len Length of the payload
     */
    MetaFile(const char* buffer, int len);
    ~MetaFile(){}
    
    
    /**
     * Find element from metafile
     * @param name Name of the element
     * @param found Success status of search
     * @return Reference to the found element
     */
    Element& find(string const& name, bool& found);
    
    /**
     * Convert string to Element
     * @param line Element info stored in a string
     * @param file Reference to a Element where to save the info
     * @return Success status of the operation
     */
    bool strToElement(string const& line, Element& file);
    
    /**
     * Convert Element to string
     * @param file Reference to the Element from which to read the info
     * @return String containing the element info
     */
    string elementToStr(Element const& file) const;
    
    /**
     * Read the contents of the metatile
     */
    void read(void);
    
    /**
     * Overwrite to the metatile
     * @return Success status of the operation
     */
    bool write(void);
    
    /**
     * Updates the contents of metafile and metadata
     * @return Success status of the operation
     */
    bool updateAll(void);
    
    /**
     * Print metadata
     */
    void print(void) const;
    
    /**
     * Compares two MetaFiles and returns all the elements, which cannot be be found
     * on the other-Metafile. In case an element has been modified the newer element
     * is chosen. If the timestamps match, the other-element is chosen.
     * @param other MetaFile to which the comparison is performed
     * @return String containing all the different elements
     */
    string getDiff(MetaFile& other);
    
    /**
     * Get contents of Metafile
     * @return Contents of the Metafile as a string
     */
    string getDescr();
    
    /**
     * Get the Metadata
     * @return List of all stored Elements
     */
    list<Element> getMetadata() const {return _metadata;}

    
private:
    // The rule of three
    MetaFile(MetaFile const& other);
    MetaFile& operator=(MetaFile const& other);

};

/**
 * Print metadata to stream
 * @param os The output stream to print into
 * @param m Metafile to print
 * @return Metadata contents
 */
ostream& operator<<(ostream& os, const MetaFile& m);

/**
 * Equal operator to Elements
 * @param lhs First element
 * @param rhs Second element
 * @return True if the elements are equal
 */
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
