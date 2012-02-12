//
//  filesys.cc
//  SyncCFT
//
//  Created by Elo Matias on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
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

#include "filesys.hh"

/* Calculates 16-byte MD5 hash from the given file
 * @param filename
 * @param length of the file
 * @param store the hash here
 * @return 16-byte has hash as string
 */
bool MD5Hash(string filename, long length, string& hash) {
    int fd;
    char* fileBuf;
    unsigned char result[MD5_DIGEST_LENGTH];
    
    fd = open(filename.c_str(), O_RDONLY);
    if(fd < 0) return false;
    
    // Map file to memory
    fileBuf = (char*)mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
    MD5((unsigned char*)fileBuf, length, result);
    
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

void info(void) {
    DIR *directory;
    struct dirent *entry;
    struct stat stats;
    fstream metafile;
    list<string> metadata;
    
    
    directory = opendir ("./");
    if (directory != NULL) {
        
        // Try to open directory metafile
        metafile.open(METAFILE);
        if (metafile.is_open()) { // Old metafile found
            cout << "Old metafile found" << endl;
            
            // Read configuration file line by line
            string line;
            while (getline(metafile, line)) {
                //cout << "Metafile: " << line << endl;
                // Save data into an array
                metadata.push_back(line);
            }
            metafile.close();   
        } else { // No metafile found
            cout << "No metafile found" << endl;
            metafile.open(METAFILE, fstream::out);
            if (metafile.is_open()) { // Old metafile found
                cout << "Opened new metafile" << endl;
                
                
                // TODO
                
                metafile.close();
            }
        }
        
        // Loop through each entry in the given directory
        while ((entry = readdir(directory))) {
            
            // Skip '.', '..' and hidden files
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
                continue;
            
            stat(entry->d_name, &stats);
            cout << "Name: " << entry->d_name << endl;
            time_t seconds;
            seconds = time(NULL);
            cout << "Time: " << seconds << endl;
            
            
            // Check file type
            string typeStr;
            int type = (int)entry->d_type; 
            switch (type) {
                case FT_FILE:
                    typeStr = "file";
                    break;
                case FT_FOLDER:
                    typeStr = "folder";
                    break;
                default:
                    typeStr = "unknown";
                    break;
            }
            cout << "Type: " << typeStr << endl;
            
            cout << "Size: " << stats.st_size << endl;
            
            if (type == FT_FILE) {
                string fileHash;
                if(MD5Hash(entry->d_name, stats.st_size,fileHash))
                    cout << "Hash: " << fileHash << endl;
            }
            cout << "---" << endl;
        }
        (void) closedir (directory);
        }
    else
        perror ("Couldn't open the directory");
}