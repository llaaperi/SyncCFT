//
//  utilities.hh
//  SyncCFT
//
//  Created by Elo Matias on 18.2.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_utilities_hh
#define SyncCFT_utilities_hh

#include <string>
#include <vector>

#define STATE_LOST 0
#define STATE_NOT_LOST 1


using namespace std;

namespace Utilities {
    
    /* Splits given string using separator. Results are saved in a given vector.
     * @param str, Source string for the method
     * @param separator, String to be used as separator
     * @param results, Vector where to save the results
     * @return Size of vector results
     */
    int split(string str, string separator, vector<string>& results);
    
    
    /*
     * Markov process
     */
    bool packetLost(int state, double p, double q);
        
}
#endif
