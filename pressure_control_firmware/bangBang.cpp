
#include "bangBang.h"

#include <Arduino.h>
#include "allSettings.h"


//Initialize the controller with relevant information
void bangBang::initialize(controlSettings &newSettings){
  deadWindow = newSettings.deadzone;
  setpoint   = newSettings.setpoint;
}


//Update the controller settings
void bangBang::updateSettings(controlSettings &newSettings){
  //For simple Bang-Bang, we just update the deadzone
  deadWindow = newSettings.deadzone;
}


//Update the setpoint
void bangBang::setSetpoint(float setpoint_in){
  setpoint = setpoint_in;
}




//Run 1 step of the controller - outputs "flow rate"
float bangBang::go(float pressure){

  float lowset=setpoint-deadWindow;
  float highset=setpoint+deadWindow;
  float out=0.0;

  //If the setpoint is 0, vent
  if(setpoint==0){
    out = -1.0;
  }
  else{
    //If the setpoint is below the lower bound, pressurize
    if (pressure < lowset){
      out = 1.0;
    }
    //If the setpoint is above the upper bound, vent
    else if(pressure > highset){
      out = -1.0;
    }
    //Otherwise, close both valves
    else{
      out = 0.0;
    }
  }

  int pressureLast=pressure;

  return out;
}


