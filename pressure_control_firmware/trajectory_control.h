#include <stdlib.h>
#include "Arduino.h"
#include "trajectory.h"

#ifndef __trajectory_control_H__
#define __trajectory_control_H__


class TrajectoryControl
{
  private:
   
    // Define setup variables
    const static unsigned int maxLen = 1024;
    //const static unsigned int maxChannels = 1;
    int curr_idx = 1;
    int num_channels; 

    Trajectory *traj;

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
    bool all_running = false;
    bool all_finished = false;
    bool reset=false;
    
    // constructor (empty)
    TrajectoryControl(){};

    void initialize(Trajectory *, int);

    // load the trajectory into memory
    bool setLength(int);
    bool setLine(int, float);

    // Define control functions
    void start();
    void stop();
    void pause();
    void resume();
    
    // Define the interp function
    float interp(float, int);
    bool isAllFinished();

};

#endif
