#include <stdlib.h>
#include "Arduino.h"

#ifndef __trajectory_H__
#define __trajectory_H__


class Trajectory
{
  private:
   
    // Define setup variables
    const static unsigned int maxLen = 10;
    const static unsigned int maxPrefixLen = 5;
    const static unsigned int maxSuffixLen = 5;
    
    //const static unsigned int maxChannels = 1;
    int curr_idx = 1;

    float lerp(float, float, float);
    
  public:
    // Define times
    float CurrTime=0;
    
    // Define important variables
    int len[3] = {2,20,2};
    bool running = false;
    bool finished[3] = {false, false, false};
    bool reset=false;
    float trajpts [maxLen];
    float trajtimes [maxLen];
    float prefixpts [maxPrefixLen];
    float prefixtimes [maxPrefixLen];
    float suffixpts [maxSuffixLen];
    float suffixtimes [maxSuffixLen];
    int current_idx[3]={0,0,0};
    int current_traj = 1;
    int start_idx = 0;
    float stretch_factor=1;


    float final_time=0;
    float final_prefix_time=0;
    float final_suffix_time=0;
    
    // constructor (empty)
    Trajectory(){};


    // Define control functions
    void start();
    void startTraj();
    void startPrefix();
    void startSuffix();
    void stop();
    void pause();
    void resume();
    
    // Define the interp function
    float interp(float);

    void updateFinalTimes();

};

#endif
