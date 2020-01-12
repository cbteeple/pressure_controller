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
  - Troubleshoot the new library structure. 


*/



bool TrajectoryControl::setLength(int prefix_len_in, int traj_len_in, int suffix_len_in){
  for(int i=0; i<num_channels; i++){
    traj[i].setLength(traj_len_in, prefix_len_in, suffix_len_in);
  }
  len[0]=prefix_len_in;
  len[1]=traj_len_in;
  len[2]=suffix_len_in;
   
}





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
  StartTime = CurrTime;
  current_traj = 0;

  for(int i=0; i<num_channels; i++){
    traj[i].startPrefix();
  }

  Serial.println("_TRAJ: Start Prefix");
}


void TrajectoryControl::startTraj(){
  all_running = true;
  current_traj = 1;
  for (int i=0; i<num_channels; i++){
    traj[i].startTraj();
  }
  Serial.println("_TRAJ: Start Main");
}

/*
Stop the trajectory and reset to the beginning
*/
void TrajectoryControl::stop(){
  all_running=false;
  current_traj = 2;
  reset=true;
  if (suffix_after_stop){
    for(int i=0; i<num_channels; i++){
      traj[i].setSuffixLine(-1, 0.0, traj[i].interp(deltaT)); //set the first line of the suffix to the current setpoint
      traj[i].startSuffix(); //start running the suffix
    }
    Serial.println("_TRAJ: Start Suffix");
  }
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
  deltaT = float(CurrTime-StartTime)/1000000.0;
  
  float p_set = traj[channel].interp(deltaT);

  return p_set;
}

void TrajectoryControl::setUpNext(){
  
  all_finished = isAllFinished();

  if (all_running & all_finished){

    if (current_traj==0){
      Serial.println("_TRAJ: End Prefix");
      startTraj();
    }
    else if (current_traj==1){
      Serial.println("_TRAJ: End Main");
      if (wrap){
        // Restart all trajectories
        for (int i=0; i<num_channels; i++){
          startTraj();
        }
      }
      else{
        stop();
      }
    }
    else if (current_traj==2){
      all_running=false;
      Serial.println("_TRAJ: End Suffix");
    }


    
  }
}




bool TrajectoryControl::isAllFinished() {
  for ( int i = 0; i < num_channels; ++i ) {
     if ( traj[i].finished == false ) {
       return false;
     }
  }

  return true;
 }



