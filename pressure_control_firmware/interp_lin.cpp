
#include "interp_lin.h"

#include <Arduino.h>
#include "allSettings.h"




//Update the setpoint
void interpLin::newGoal(controlSettings &newSettings){
  value_A = curr_value;
  value_B = newSettings.setpoint;
  transition_time = newSettings.settime;
  new_goal = true;
}




void interpLin::start(){
  running = true;
  StartTime = CurrTime;
}

void interpLin::stop(){
  running=false;  
}







//Run an interpolation
float interpLin::go(){

  if (new_goal){
    new_goal = false;
    start();
  }


  float deltaT = float(CurrTime-StartTime)/1000000.0;

  if (deltaT >= transition_time){
    curr_value = value_B;
    running=false;
  }
  //else, do the interpolation
  else{
    float percent = deltaT/transition_time;
    curr_value=lerp(value_A, value_B, percent);
  }

  return curr_value;
}




float interpLin::lerp(float a, float b, float f){
    return a + f * (b - a);
}



