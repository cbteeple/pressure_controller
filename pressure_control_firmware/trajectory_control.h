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
   
  public:
    // Define times
    unsigned long StartTime=0;
    unsigned long CurrTime=0;
    unsigned long ResumeTime=0;
    
    // Define important variables
    int len[3] = {0,20,0};
    int start_idx = 0;
    bool wrap = false;
    bool all_running = false;
    bool all_finished = false;
    bool reset=false;
    bool suffix_after_stop = true;
    int current_traj = 0;
    float deltaT=0;
    String current_message="";
    
    // constructor (empty)
    TrajectoryControl(){};

    void initialize(Trajectory *, int);

    // load the trajectory into memory
    bool setLength(int,int,int);
    bool setLine(int, float);

    // Define control functions
    void start();
    void startTraj();
    void stop();
    void fullStop();
    void pause();
    void resume();
    
    // Define the interp function
    float interp(float, int);
    bool isAllFinished();
    void setUpNext();

};

#endif
