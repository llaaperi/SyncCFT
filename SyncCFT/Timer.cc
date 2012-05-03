//
//  Timer.cc
//  SyncCFT
//
//  Created by Lauri Lääperi on 3.5.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>
#include "Timer.hh"


Timer::Timer(){

}

Timer::~Timer(){

}


/*
 *
 */
void Timer::start(){
    gettimeofday(&_startTime, NULL);
}


/*
 *
 */
void Timer::stop(){
    gettimeofday(&_endTime, NULL);
}


/*
 *
 */
long Timer::elapsed_ms(){
    
    timeval curr;
    gettimeofday(&curr, NULL);
    
    long start = (_startTime.tv_sec * 1000000) + _startTime.tv_usec;
    long end = (curr.tv_sec * 1000000) + curr.tv_usec;
    return (end - start) / 1000;
}


/*
 *
 */
long Timer::elapsed_s(){
    return elapsed_ms() / 1000;
}
