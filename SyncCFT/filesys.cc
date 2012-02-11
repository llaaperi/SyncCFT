//
//  filesys.cc
//  SyncCFT
//
//  Created by Elo Matias on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <list>
#include <ctime>

#include <openssl/md5.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "filesys.hh"

using namespace std;

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
                cout << "Metafile: " << line << endl;
                // Save data into an array
                metadata.push_back(line);
            }
            metafile.close();
            
            
        } else { // No metafile found
            cout << "No metafile found" << endl;
            metafile.open(METAFILE, fstream::out);
            if (metafile.is_open()) { // Old metafile found
                cout << "Opened new metafile" << endl;
                metafile << "SEKALAISTA DATAAAAAAAA" << endl;
                metafile.close();
            }
            
            
            
            
        }
        
        while ((entry = readdir(directory))) {
            
            // Skip . and ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            stat(entry->d_name, &stats);
            cout << "Name: " << entry->d_name << endl;
            time_t seconds;
            seconds = time(NULL);
            cout << "Time: " << seconds << endl;
            
            // MD5 hash
            unsigned char result[MD5_DIGEST_LENGTH];
            const char* str = "Test sentenc";
            MD5((const unsigned char *)str,strlen(str),result);
            
            // output
            cout << "Hash: ";
            for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
                printf("%02x", result[i]);
            printf("\n");
            
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
            
            cout << "Size: " << stats.st_size << endl << "---" << endl;
        }
        (void) closedir (directory);
        }
    else
        perror ("Couldn't open the directory");
}