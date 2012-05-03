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
    ~Timer();
    
    void start();
    void stop();
    long elapsed_ms();
    long elapsed_s();
    
private:
    

};

#endif
