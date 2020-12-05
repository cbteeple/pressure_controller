#include <stdlib.h>
#include "Arduino.h"

#include "trajectory.h"


/*
Start the trajectory from the beginning
*/
void Trajectory::start(){
  reset=false;
  finished[0] = false;
  finished[1] = false; 
  finished[2] = false;
  running = true;
  current_idx[0] = 1;
  current_idx[1] = 1;
  current_idx[2] = 1;
  current_traj = 0;
}

void Trajectory::startTraj(){
  current_traj = 1;
  current_idx[1] = 1;
  reset=false;
  running = true;
  finished[1] = false;
}

void Trajectory::startPrefix(){
  current_traj = 0;
  current_idx[0] = 1;
  reset=false;
  running = true;
  finished[0] = false;
}

void Trajectory::startSuffix(){
  current_traj = 2;
  current_idx[2] = 1;
  reset=false;
  running = true;
  finished[2] = false;
}



/*
Stop the trajectory and reset to the beginning
*/
void Trajectory::stop(){
  running=false;
  reset=true;
}


/*
Pause the trajectory at current point
*/
void Trajectory::pause(){
  running=false;
}


/*
Resume the trajectory from where it left off when paused
*/
void Trajectory::resume(){
  running=true;
}



void Trajectory::updateFinalTimes(){
  final_time = trajtimes[len[1]-1];
  final_prefix_time = prefixtimes[len[0]-1];
  final_suffix_time = suffixtimes[len[2]-1];
}
