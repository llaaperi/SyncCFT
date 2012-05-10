//
//  metafile.cc
//  SyncCFT
//
//  Created by Elo Matias on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <vector>

#include <ctime>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#include "metafile.hh"
#include "utilities.hh"

string _syncDir = "./Sync/";

MetaFile::MetaFile(string fName) : _fileName(fName) {
    
    // Read metadata file
    read();
    
    // Update metadata
    updateAll();
    
    // Save changes to metafile
    write();
}

/*
 * Construct metafile from received data
 */
MetaFile:: MetaFile(const char* buffer, int len) {
    string line;
    string content(buffer, len);
	istringstream stream(content);
    while (getline(stream, line)) {
        Element ele;
        if (strToElement(line, ele))
            _metadata.push_back(ele);
    }    
}

/*
 * Find element
 * @param name Name of the element
 * @param found Success status of search
 * @return Reference to the found element
 */
Element& MetaFile::find(string const& name, bool& found) {
    for (list<Element>::iterator iter = _metadata.begin(); iter != _metadata.end(); iter++) {
        if (iter->getName() == name) {
            found = true;
            return *iter;
        }
    }
    found = false;
    return *_metadata.end();
}


/*
 * Convert string to Element
 */
bool MetaFile::strToElement(string const& line, Element& file) {
    vector<string> splitted;
    if(Utilities::split(line, ";", splitted) != 4)
        return false;
    file.setName(splitted[0]);
    file.setSize(atoi(splitted[1].c_str()));
    file.setHash(splitted[2]);
    file.setTimeStamp(atoi(splitted[3].c_str()));
    return true;
}

/*
 * Convert Element to string
 */
string MetaFile::elementToStr(Element const& file) const{
    ostringstream line;
    line << file.getName() << ";" << file.getSize() << ";" << file.getHash() << ";" << file.getTimeStamp();
    return line.str();
}


/*
 * Read the contents of the metatile
 */
void MetaFile::read(void) {
    string line;
    
    // Try to open directory metafile
    _metafile.open((_syncDir + _fileName).c_str(), fstream::in);
    if (_metafile.is_open()) { // Old metafile found
        //cout << "[METAFILE] Metafile found" << endl;
        
        // Read configuration file line by line and save data into an array
        while (getline(_metafile, line)) {
            Element newFile;
            if(strToElement(line, newFile))
                _metadata.push_back(newFile);
        }
        _metafile.close();   
    }   
}

/*
 * Overwrite to the metatile
 */
bool MetaFile::write(void) {
    _metafile.open((_syncDir + _fileName).c_str(), fstream::out | fstream::trunc);
    if (_metafile.is_open()) {
        for (list<Element>::const_iterator iter = _metadata.begin(); iter != _metadata.end(); iter++)
            _metafile << elementToStr(*iter) << endl;
        _metafile.close();
        return true;
    } else {
        cout << "[METAFILE] Unable to write to the metafile" << endl;
        return false;
    }
}

/*
 * Updates the contents of metafile and metadata
 */
bool MetaFile::updateAll(void) {
    DIR *directory;
    struct dirent *entry;
    struct stat stats;
    list<Element> newMetadata;
    
    directory = opendir (_syncDir.c_str());
    if (directory == NULL) {
        cout << "[METAFILE] Couldn't open the directory " << _syncDir << endl;
        return false;
    }
        
    // Loop through each entry in the given directory
    while ((entry = readdir(directory))) {
        //cout << "[METAFILE] Directory entry: " << entry->d_name << endl;
        Element newFile;            
        // Skip '.', '..' and hidden files
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
            continue;
            // Obtain file information
        if (stat((_syncDir + entry->d_name).c_str(), &stats) < 0) {
            //cout << "[METAFILE] No file info" << endl;
            continue;
        }
        
        // Check file type
        int type = (int)entry->d_type;
        //cout << "[METAFILE] Type: " << type << endl;
        if (type == FT_FILE) {
            //cout << "[METAFILE] File: " << entry->d_name << endl;
            // Filename
            newFile.setName(entry->d_name);
            // File size
            newFile.setSize(stats.st_size);
            // MD5 hash
            string fileHash;
            if(!Utilities::MD5Hash(_syncDir + entry->d_name,fileHash)) {
                //cout << "[METAFILE] MD5 failed" << endl;
                continue;
            }
            newFile.setHash(fileHash);
            // Timestamp
            newFile.setTimeStamp(time(NULL));
            
            // Check if data already exists and update only changed files
            bool found = false;
            Element& oldFile = find(newFile.getName(), found);
            if (found) { // File already exists in metafile
                //cout << "[METAFILE] Found old entry" << endl;
                if (newFile.getHash() == oldFile.getHash()) {
                    newMetadata.push_back(oldFile);
                    continue;
                }
            }
            newMetadata.push_back(newFile);
        } else if (FT_FOLDER) {
            //cout << "Skip folder" << endl;
        } else
            cout << "Unknown filetype" << endl;
    }

    _metadata = newMetadata;
    closedir (directory);
    return true;     
}

/*
 * Print metadata
 */
void MetaFile::print(void) const{
    for (list<Element>::const_iterator iter = _metadata.begin(); iter != _metadata.end(); iter++) {
        cout << elementToStr(*iter) << endl;
    }
}

/*
 * Print metadata to stream
 */
std::ostream& operator<<(std::ostream& os, const MetaFile& m) {
    ostringstream strs;
    string collect;
    list<Element> data = m.getMetadata();
    for (list<Element>::const_iterator iter = data.begin(); iter != data.end(); iter++) {
        collect += m.elementToStr(*iter) + "\n";
    }
    collect = collect.substr(0,collect.length()-1);
    return os << collect;
}


/*
 * Compares two MetaFiles and returns all the elements, which cannot be be found
 * on the other-Metafile. In case an element has been modified the newer element
 * is chosen. If the timestamps match, the other-element is chosen.
 * @param other MetaFile to which the comparison is performed
 * @return String containing all the different elements
 */
string MetaFile::getDiff(MetaFile& other) {
    string different = "";
    for (Element e : _metadata) {
        bool found = false;
        Element& otherElement = other.find(e.getName(), found);
        if(!found) { // Element not found
            different += elementToStr(e) + "\n";
            continue;
        }
        // Compare element hashes and timestamps
        int comparison = e.compare(otherElement);
        if (comparison <= 0)  // Identical elements or the other elements is 
            continue;         // newer
        else // Own element is newr
            different += elementToStr(e) + "\n";
    }
    // Remove extra newline
    if (!different.empty())
        different = different.substr(0,different.length()-1);
    return different;
}


/*
 *
 */
string MetaFile::getDescr(){
    stringstream sStream;
    sStream << *this; 
    return sStream.str();
}
