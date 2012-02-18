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

#include <openssl/md5.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#include "metafile.hh"
#include "utilities.hh"


MetaFile::MetaFile(string fName) : fileName(fName) {
    
    // Read metadata file
    read();
    
    // Update metadata
    update();
    
    // Save changes to metafile
    write();
}

/*
 * Find file
 */
 bool MetaFile::find(string const& name, File& file) {
    for (list<File>::iterator iter = metadata.begin(); iter != metadata.end(); iter++) {
        if (iter->getName() == name) {
            file = *iter;
            return true;
        }
    }
    return false;
}

/*
 * Convert string to File
 */
bool MetaFile::strToFile(string const& line, File& file) {
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
 * Convert File to string
 */
string MetaFile::fileToStr(File const& file) const{
    ostringstream line;
    line << file.getName() << ";" << file.getSize() << ";" << file.getHash() << ";" << file.getTimeStamp();
    return line.str();
}

/* Calculates 16-byte MD5 hash from the given file
 * @param filename Name of the file
 * @param length Size of the file
 * @param hash Store the hash here
 * @return 16-byte has hash as string
 */
bool MetaFile::MD5Hash(string const& filename, long const length, string& hash) {
    int fd;
    char* fileBuf;
    unsigned char result[MD5_DIGEST_LENGTH];
    
    fd = open(filename.c_str(), O_RDONLY);
    if(fd < 0)
        return false;
    
    // Map file to memory
    fileBuf = (char*)mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
    MD5((unsigned char*)fileBuf, length, result);
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

/*
 * Read the contents of the metatile
 */
void MetaFile::read(void) {
    string line;
    
    // Try to open directory metafile
    metafile.open(fileName.c_str(), fstream::in);
    if (metafile.is_open()) { // Old metafile found
        cout << "Metafile found" << endl;
        
        // Read configuration file line by line and save data into an array
        while (getline(metafile, line)) {
            File newFile;
            if(strToFile(line, newFile))
                metadata.push_back(newFile);
        }
        metafile.close();   
    }   
}

/*
 * Overwrite to the metatile
 */
bool MetaFile::write(void) {
    metafile.open(fileName.c_str(), fstream::out | fstream::trunc);
    if (metafile.is_open()) {
        for (list<File>::const_iterator iter = metadata.begin(); iter != metadata.end(); iter++)
            metafile << fileToStr(*iter) << endl;
        metafile.close();
        return true;
    } else
        return false; 
}

/*
 * Updates the contents of metafile and metadata
 */
bool MetaFile::update(void) {
    DIR *directory;
    struct dirent *entry;
    struct stat stats;
    list<File> newData;
    
    directory = opendir ("./");
    if (directory != NULL) {
        
        // Loop through each entry in the given directory
        while ((entry = readdir(directory))) {
            File newFile;            
            // Skip '.', '..' and hidden files
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
                continue;
            // Obtain file information
            if (stat(entry->d_name, &stats) < 0)
                continue;
            
            // Check file type
            int type = (int)entry->d_type;

            if (type == FT_FILE) {
                // Filename
                newFile.setName(entry->d_name);
                // File size
                newFile.setSize(stats.st_size);
                // MD5 hash
                string fileHash;
                if(!MD5Hash(entry->d_name, stats.st_size,fileHash))
                    continue;
                newFile.setHash(fileHash);
                // Timestamp
                newFile.setTimeStamp(time(NULL));
                
                // Check if data already exists and update only changed files
                File oldFile;
                if (find(newFile.getName(), oldFile)) {
                    if (newFile.getHash() == oldFile.getHash())
                        newData.push_back(oldFile);
                    else if (newFile.getTimeStamp() <= oldFile.getTimeStamp())
                        newData.push_back(oldFile);
                    else
                        newData.push_back(newFile);
                }     
            } else if (FT_FOLDER) {
                //cout << "Skip folder" << endl;
            } else
                cout << "Unknown filetype" << endl;
        }
        (void) closedir (directory);
        metadata = newData;
        return true;     
    }
    else {
        perror ("Couldn't open the directory");
        return false;
    }
}

/*
 * Print metadata
 */
void MetaFile::print(void) const{
    for (list<File>::const_iterator iter = metadata.begin(); iter != metadata.end(); iter++) {
        cout << fileToStr(*iter) << endl;
    }
}
