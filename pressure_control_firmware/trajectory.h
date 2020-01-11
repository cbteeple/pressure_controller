#include <stdlib.h>
#include "Arduino.h"

#ifndef __trajectory_H__
#define __trajectory_H__


class Trajectory
{
  private:
   
    // Define setup variables
    const static unsigned int maxLen = 1024;
    //const static unsigned int maxChannels = 1;
    int curr_idx = 1;

    float lerp(float, float, float);
    
  public:
    // Define times
    unsigned long StartTime=0;
    unsigned long CurrTime=0;
    unsigned long ResumeTime=0;
    
    // Define important variables
    int len = 20;
    int start_idx = 0;
    bool wrap = false;
    bool running = false;
    bool finished = false;
    bool reset=false;
    float trajpts [maxLen];
    float trajtimes [maxLen];
    float final_time;
    
    // constructor (empty)
    Trajectory(){};

    // load the trajectory into memory
    bool setLength(int);
    bool setLine(int, float ,float);

    // Define control functions
    void start();
    void stop();
    void pause();
    void resume();
    
    // Define the interp function
    float interp(float);

    void updateFinalTime();

};

#endif
