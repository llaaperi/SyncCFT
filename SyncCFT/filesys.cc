//
//  filesys.cc
//  SyncCFT
//
//  Created by Elo Matias on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include <stdio.h>
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
    
    directory = opendir ("./");
    if (directory != NULL)
        {
        while ((entry = readdir(directory))) {
            
            // Skip . and ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            stat(entry->d_name, &stats);
            cout << "Name: " << entry->d_name << endl;
            
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