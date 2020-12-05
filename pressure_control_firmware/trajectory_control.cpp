#include <stdlib.h>
#include "Arduino.h"

#include "trajectory_control.h"


/*
TODO:
  - Transfer all top-level control logic to this library
  - Remove control logic from trajectory class, but still keep current index
    (since we never expect to run trajectories backward)
  - Update all trajectory functions in the HID and SERIAL libraries to work with new classes
  - Pass this controller class to HID and SERIAL libraries as well as trajectory array
  - Troubleshoot the new library structure. 


*/



bool TrajectoryControl::setLength(int prefix_len_in, int traj_len_in, int suffix_len_in){

  len[0]=prefix_len_in;
  len[1]=traj_len_in;
  len[2]=suffix_len_in;
   
}


bool TrajectoryControl::setSpeed(float speed_factor_in){

}





/*
Initialize the trajectory control object 
INPUTS:
    traj_in         - a vector of trajectory objects
    num_channels_in - the number of channels
*/
void TrajectoryControl::initialize(int num_channels_in){
  num_channels = num_channels_in;
}


/*
Start the trajectory from the beginning
*/
void TrajectoryControl::start(){
  reset=false;
  all_finished = false;
  all_running = true;
  StartTime = CurrTime;
  current_traj = 0;
  curr_cycle = 0;


  current_message += ("_TRAJ: Start Prefix");
  current_message += '\n';
}


void TrajectoryControl::startTraj(){
  StartTime = CurrTime;
  all_running = true;
  current_traj = 1;
  current_message += ("_TRAJ: Start Main");
  current_message += '\n';
}

/*
Stop the trajectory and reset to the beginning
*/
void TrajectoryControl::stop(){
  // Check if we are currently running some trajectory
  if (all_running){
    // If a trajectory is running, perform a controlled stop (transition to the suffix)
    current_traj = 2;
    if (suffix_after_stop){
      all_running=true;
      StartTime = CurrTime;
      current_message +=("_TRAJ: Start Suffix");
      current_message += '\n';
    }
    else{
      fullStop();
    }
  }
}

void TrajectoryControl::fullStop(){
  //reset=true;
  all_running=false;
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

  return 0.0;
}




void TrajectoryControl::setUpNext(){
  current_message="";
  
  all_finished = isAllFinished();

  if (all_running & all_finished){

    if (current_traj==0){
      current_message += ("_TRAJ: End Prefix");
      current_message += '\n';
      startTraj();
    }
    else if (current_traj==1){
      current_message += ("_TRAJ: End Main");
      current_message += '\n';
      if (num_cycles == -1 ){
        // Restart all trajectories
        startTraj();
      }
      else if (curr_cycle<num_cycles-1){
        // Restart all trajectories
        startTraj();
        curr_cycle++;
      }
      else{
        stop();
      }
    }
    else if (current_traj==2){
      fullStop();
      current_message += ("_TRAJ: End Suffix");
      current_message += '\n';
    }    
  }
}




bool TrajectoryControl::isAllFinished() {
  return true;
 }
