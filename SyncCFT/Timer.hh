//
//  Timer.hh
//  SyncCFT
//
//  Created by Lauri Lääperi on 3.5.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#ifndef SyncCFT_Timer_hh
#define SyncCFT_Timer_hh

#include <sys/time.h>

using namespace std;

class Timer{

    timeval _startTime;
    timeval _endTime;
  
public:
    
    Timer();
    ~Timer(){};
    
    /**
     * Start or reset the timer
     */
    void start();
    
    /**
     * Stop the timer
     */
    void stop();
    
    /**
     * Get the time since start in seconds
     * @return Elapsed time in seconds
     */
    long elapsed_ms();
    
    /**
     * Get the time since start in seconds
     * @return Elapsed time in seconds
     */
    long elapsed_s();
    
private:
    

};

#endif
