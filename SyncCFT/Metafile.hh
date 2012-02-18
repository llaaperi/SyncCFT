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

class File {
public:
    

    string getName() const {return name;}
    void setName(string const& newName) {name = newName;}
    size_t getSize() const {return size;}
    void setSize(size_t const newSize) {size = newSize;}
    string getHash() const {return hash;}
    void setHash(string const& newHash) {hash = newHash;}
    time_t getTimeStamp() const {return timestamp;}
    void setTimeStamp(time_t const newTime) {timestamp = newTime;}
    
private:
    string name;  
    int size;
    string hash;
    int timestamp;
};

class MetaFile {
    
public:
    
    MetaFile(string fName);
    ~MetaFile(){}
    
    /*
     * Find file
     */
    bool find(string const& name, File& file);
    
    /*
     * Find file
     */
    File& find2(string const& name, File& file);
    
    /*
     * Convert string to File
     */
    bool strToFile(string const& line, File& file);
    
    /*
     * Convert File to string
     */
    string fileToStr(File const& file) const;
    
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
    bool update(void);
    
    /*
     * Print metadata
     */
    void print(void) const;
    
    
private:
    // Rule of three
    MetaFile(MetaFile const& other);
    MetaFile& operator=(MetaFile const& other);
    
    const string fileName;
    fstream metafile;
    list<File> metadata;
};



#endif
