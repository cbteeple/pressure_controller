#include <stdlib.h>
#include "Arduino.h"

#include "trajectory.h"


/*
Set a the length of the trajectory 
INPUTS:
    len_in - the length

OUTPUTS:
    error  - the error state after execution.
             true = successful, false = error occured

EXAMPLE:
error = setLength(len_in);
*/
bool Trajectory::setLength(int len_in){
  bool error = false;
  if (len_in<=maxLen){
    len[1] = len_in;
  }
  else{
    error = true;  
  }
  return error;
  
};


bool Trajectory::setLength(int traj_len_in, int prefix_len_in , int suffix_len_in){
  bool error = false;
  if (prefix_len_in<=maxPrefixLen){
    len[0] = prefix_len_in+1;
  }
  else{
    error = true;  
  }
  if (traj_len_in<=maxLen){
    len[1] = traj_len_in;
  }
  else{
    error = true;  
  }
  if (suffix_len_in<=maxSuffixLen){
    len[2] = suffix_len_in+1;
  }
  else{
    error = true;  
  }

  
  return error;
  
};


/*
Set a line in the trajectory
INPUTS:
    whichTraj - designated which trajectory segment to store the current line in
    idx    - the line index to overwrite
    time   - the associated trajectory time with this point
    value  - the pressure value to store

OUTPUTS:
    error  - the error state after execution.
             true = successful, false = error occured

EXAMPLE:
error = setLine(idx, time, row);
*/
bool Trajectory::setLine(int whichTraj, int idx, float time, float value){
  switch( whichTraj){
    case 0:{
      if (idx<0 | idx>maxPrefixLen-1 | time<0.0){
        return true;
      }
      prefixtimes[idx+1] = time;
      prefixpts[idx+1]   = value;
    } break;
    case 1:{
      if (idx<0 | idx>maxLen | time<0.0){
        return true;
      }
      trajtimes[idx] = time;
      trajpts[idx]   = value;
    } break;
    case 2:{
      if (idx<0 | idx>maxSuffixLen-1 | time<0.0){
        return true;
      }
      suffixtimes[idx+1] = time;
      suffixpts[idx+1]   = value;
    } break;

  }

  updateFinalTimes();
  return false;
};


bool Trajectory::setPrefixLine(int idx, float time, float value){
  return setLine(0, idx, time, value);  
}

bool Trajectory::setTrajLine(int idx, float time, float value){
  return setLine(1, idx, time, value); 
}

bool Trajectory::setSuffixLine(int idx, float time, float value){
  return setLine(2, idx, time, value);
}



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


/*
Interpolate the trajectory given the current time
INPUTS:
    curr_time - the time at which to get the trajectory value

OUTPUTS:
    p_set - Linear interpolation between a and b
          according to: a+f*(b-a)

EXAMPLE:
p_set = interp(curr_time);
*/

float Trajectory::interp(float deltaT){
  float *times;
  float *pts;
  int *curr_idx;
  curr_idx = &current_idx[current_traj];
  int *curr_len;
  curr_len= &len[current_traj];


  switch(current_traj){
    case 0:{
      times = prefixtimes;
      pts   = prefixpts;
    } break;
    case 1:{
      times = trajtimes;
      pts   = trajpts;
    } break;
    case 2:{
      times = suffixtimes;
      pts   = suffixpts;
    } break;
    default:{
      times = trajtimes;
      pts   = trajpts;
    } break;

  }

  Serial.println(deltaT,4);


  float p_set = 0;
  // If the trajectory is not finished, do the interpolation
  if (!finished[current_traj]){
    //If deltaT is larger than the largest time in the trajectory, it must be finished.
    if(deltaT >= times[*curr_len-1]){
      Serial.println("TRAJECTORY: Time exceeds largest index");
      Serial.println(times[*curr_len-1],4);
      *curr_idx = *curr_len-1;
      deltaT = times[*curr_len-1];
  
      running=false;
      finished[current_traj] = true;
      
    }
    // If deltaT is in the correct range, do the interpolation
    else{ 
      Serial.println("TRAJECTORY: find the right index");
      Serial.println(times[*curr_idx],4);
      while(deltaT>times[*curr_idx]){
        curr_idx++;
  
        if (*curr_idx >= *curr_len-1){
          break;
        }
      }
    }
  
    //Do an interpolation
    float percent = (deltaT-times[*curr_idx-1]) / (times[*curr_idx] - times[*curr_idx-1]);
    float a = pts[*curr_idx-1];
    float b = pts[*curr_idx];

    Serial.print(percent,4);
    Serial.print('\t');
    Serial.print(a,4);
    Serial.print('\t');
    Serial.println(b,4);
    
    p_set=lerp(a, b, percent);
  }
  //If the trajectory is finished, hold the last value
  else{
    p_set = pts[*curr_len-1];
  }

  return p_set;
}


/*
Do a linear interpolation
INPUTS:
    a - the first value
    b - the second value
    f - interpolation parameter
        (ranges from 0.0 to 1.0 for interp between a and b)

OUTPUTS:
    out - Linear interpolation between a and b
          according to: a+f*(b-a)

EXAMPLE:
out = lerp(idx, time, row);
*/
float Trajectory::lerp(float a, float b, float f){
    return a + f * (b - a);
}


void Trajectory::updateFinalTimes(){
  final_time = trajtimes[len[1]-1];
  final_prefix_time = prefixtimes[len[0]-1];
  final_suffix_time = suffixtimes[len[2]-1];
}


