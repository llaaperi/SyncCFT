//
//  filesys.hh
//  SyncCFT
//
//  Created by Elo Matias on 10.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_filesys_hh
#define SyncCFT_filesys_hh

#define FT_FILE     8
#define FT_FOLDER   4

#define METAFILE ".sync.cft"

using namespace std;

void info(void);

/* Calculates 16-byte MD5 hash from the given file
 * @param filename
 * @param length of the file
  * @param store the hash here
 * @return 16-byte has hash as string
 */
bool MD5Hash(string filename, long length, string& hash);

#endif
