#include <stdlib.h>
#include "Arduino.h"

#include "trajectory.h"
#include "trajectory_control.h"


/*
TODO:
  - Transfer all top-level control logic to this library
  - Remove control logic from trajectory class, but still keep current index
    (since we never expect to run trajectories backward)
  - Update all trajectory functions in the HID and SERIAL libraries to work with new classes
  - Pass this controller class to HID and SERIAL libraries as well as trajectory array
  - Add initialize function to the setup of main file
  - Troubleshoot the new library structure. 


*/

/*
Initialize the trajectory control object 
INPUTS:
    traj_in         - a vector of trajectory objects
    num_channels_in - the number of channels
*/
void TrajectoryControl::initialize(Trajectory *traj_in, int num_channels_in){
  traj = traj_in;
  num_channels = num_channels_in;
}


/*
Start the trajectory from the beginning
*/
void TrajectoryControl::start(){
  reset=false;
  all_finished = false;
  all_running = true;
  curr_idx = start_idx+1;
  StartTime = CurrTime;
  Serial.println("_TRAJ: Start");
}


/*
Stop the trajectory and reset to the beginning
*/
void TrajectoryControl::stop(){
  all_running=false;
  reset=true;
}


/*
Pause the trajectory at current point
*/
void TrajectoryControl::pause(){
  all_running=false;
  ResumeTime = CurrTime;
}


/*
Resume the trajectory from where it left off when paused
*/
void TrajectoryControl::resume(){
  all_running=true;
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

float TrajectoryControl::interp(float curr_time, int channel){

  CurrTime = curr_time;

  //Make sure to interpolate in the correct region
  float deltaT = float(CurrTime-StartTime)/1000000.0;

  float p_set= traj[channel].interp(deltaT);


  if (deltaT >= traj[channel].final_time){
    traj[channel].finished = true;
  }



  if (all_finished){
    Serial.println("_TRAJ: End");
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
float TrajectoryControl::lerp(float a, float b, float f){
    return a + f * (b - a);
}



bool TrajectoryControl::isAllFinished() {
  int mission_list_length = sizeof(traj) / sizeof(traj[0]);
  for ( int i = 0; i < mission_list_length; ++i ) {
     if ( traj[i].finished == false ) {
       return false;
     }
  }

  return true;
 }



