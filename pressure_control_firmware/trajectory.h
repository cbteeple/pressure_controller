#include <stdlib.h>
#include "Arduino.h"

#ifndef __trajectory_H__
#define __trajectory_H__


class Trajectory
{
  private:
   
    // Define setup variables
    // If using a teensy, allow large trajectories
    #if defined(__arm__) && defined(CORE_TEENSY)
      const static unsigned int maxLen = 1024;
      const static unsigned int maxPrefixLen = 128;
      const static unsigned int maxSuffixLen = 128;
    // If using anything else, only reserve space for small trajectories
    #else
      const static unsigned int maxLen = 100;
      const static unsigned int maxPrefixLen = 16;
      const static unsigned int maxSuffixLen = 16;
    #endif
    
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

    // load the trajectory into memory
    bool setLength(int);
    bool setLength(int, int, int);
    bool setLine(int, int, float ,float);
    bool setTrajLine(int, float, float);
    bool setPrefixLine(int, float ,float);
    bool setSuffixLine(int, float ,float);
    bool setSpeed(float);

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
