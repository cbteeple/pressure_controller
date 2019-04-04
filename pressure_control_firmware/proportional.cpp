
#include "proportional.h"

#include <Arduino.h>
#include "allSettings.h"


//Initialize the controller with relevant information
void proportional::initialize(controlSettings &newSettings){
  //update PID gain values
  for (int i=0; i<3; i++){
    pidGains[i]= newSettings.pidGains[i];
  }  
  setpoint   = newSettings.setpoint;
  
}


//Update the controller settings
void proportional::updateSettings(controlSettings &newSettings){
  //update PID gain values
  for (int i=0; i<3; i++){
    pidGains[i]= newSettings.pidGains[i];
  }
}


//Update the setpoint
void proportional::setSetpoint(float setpoint_in){
  setpoint = setpoint_in;
}




//Run 1 step of the controller - outputs "flow rate"
float proportional::go(float pressure){
  //Serial.print(setpoint,2);
  //Serial.print('\t');  
  //Do proportional control
  float out = pidGains[0]*(setpoint-pressure);
  out=constrain(out,-1.0,1.0);

  int pressureLast=pressure;

  return out;
}


