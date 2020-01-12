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
    len = len_in;
  }
  else{
    error = true;  
  }
  return error;
  
};


bool Trajectory::setLength(int traj_len_in, int prefix_len_in , int suffix_len_in){
  bool error = false;
  if (traj_len_in<=maxLen){
    len = traj_len_in;
  }
  else{
    error = true;  
  }
  if (prefix_len_in<=maxPrefixLen){
    prefix_len = prefix_len_in;
  }
  else{
    error = true;  
  }
  if (suffix_len_in<=maxSuffixLen){
    suffix_len = suffix_len_in;
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
      if (idx<0 | idx>maxLen | time<0.0){
        return true;
      }
      trajtimes[idx] = time;
      trajpts[idx]   = value;
    }
    case -99:{
      if (idx<0 | idx>maxPrefixLen | time<0.0){
        return true;
      }
      prefixtimes[idx] = time;
      prefixpts[idx]   = value;
    }
    case 99:{
      if (idx<0 | idx>maxSuffixLen | time<0.0){
        return true;
      }
      suffixtimes[idx] = time;
      suffixpts[idx]   = value;
    }

  }

  updateFinalTimes();
  return false;
};


bool Trajectory::setTrajLine(int idx, float time, float value){
  return setLine(0, idx, time, value);
}

bool Trajectory::setPrefixLine(int idx, float time, float value){
  return setLine(-99, idx, time, value);
}

bool Trajectory::setSuffixLine(int idx, float time, float value){
  return setLine(99, idx, time, value);
}



/*
Start the trajectory from the beginning
*/
void Trajectory::start(){
  reset=false;
  finished = false;
  running = true;
  curr_idx = start_idx+1;
  StartTime = CurrTime;
  Serial.println("_TRAJ: Start");
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
  ResumeTime = CurrTime;
}


/*
Resume the trajectory from where it left off when paused
*/
void Trajectory::resume(){
  running=true;
  StartTime = CurrTime-(ResumeTime-StartTime);
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

float Trajectory::interp(float curr_time){

  CurrTime = curr_time;

  //Make sure to interpolate in the correct region
  float deltaT = float(CurrTime-StartTime)/1000000.0;

  if(deltaT >= trajtimes[start_idx+len-1]){
    curr_idx = start_idx+len-1;
    deltaT = trajtimes[start_idx+len-1];

    running=false;  
    
  }
  else{      
    while(deltaT>trajtimes[curr_idx]){
      curr_idx++;

      if (curr_idx>=start_idx+len-1){
        break;
      }
    }
  }

  //Do an interpolation
  float percent = (deltaT-trajtimes[curr_idx-1]) / (trajtimes[curr_idx] - trajtimes[curr_idx-1]);
  float a = trajpts[curr_idx-1];
  float b = trajpts[curr_idx];
  
  float p_set=lerp(
    a,
    b,
    percent
    );

    if (deltaT >= trajtimes[start_idx+len-1]){
      Serial.println("_TRAJ: End");
      finished = true;
      if (wrap ){
        start();
      }
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
  final_time = trajtimes[start_idx+len-1];
}


