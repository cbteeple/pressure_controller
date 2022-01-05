
#include "pidFull.h"

#include <Arduino.h>
#include "allSettings.h"


//Initialize the controller with relevant information
void pidFull::initialize(controlSettings &newSettings){
  //update PID gain values
  for (int i=0; i<3; i++){
    pidGains[i]= newSettings.pidGains[i];
  }  
  setpoint   = newSettings.setpoint;
  
}


//Update the controller settings
void pidFull::updateSettings(controlSettings &newSettings){
  //update PID gain values
  for (int i=0; i<3; i++){
    pidGains[i]= newSettings.pidGains[i];
  }
  deadWindow = newSettings.deadzone;
  integratorTime = newSettings.integratorResetTime;
  integralStart = newSettings.integralStart;
  if (newSettings.integratorResetTime==-1.0){
    integratorResetOn = false; 
  }
  else{
    integratorResetOn = true;
  }

  // If main reset is true, rest the controller
  if (newSettings.reset){
    resetIntegrator();
    lastError=0;
    newSettings.reset=false; // mark reset finished
  }
}


void pidFull::resetIntegrator(){
    integral = 0;
    lastIntegratorReset=millis();
}


//Update the setpoint
void pidFull::setSetpoint(float setpoint_in){
  if (integratorResetOn){
    resetIntegrator();
  }
  setpoint = setpoint_in;
}

//Update the setpoint
void pidFull::setSetpointDirect(float setpoint_in){
  setpoint = setpoint_in;
}




//Run 1 step of the controller - outputs "flow rate"
float pidFull::go(float pressure){
  //Serial.print(setpoint,2);
  //Serial.print('\t');  
  //Do pid control
  float out=0.0;
  unsigned long currTime=micros();
  timestep = currTime-lastTime;
  
  float error=setpoint-pressure;

  if (abs(error) < deadWindow){
    out = 0;
  }
  else{
    if (integratorResetOn){
      if(currTime - lastIntegratorReset >=integratorTime){
         resetIntegrator();
      }
    }
  
    integral += error*timestep/1000000.0;
    float dError = (error - lastError)/timestep/1000000.0;
  
    //Serial.print(integral,2);
    //Serial.print('\t');  

    if(abs(error)>integralStart){
      resetIntegrator();
    }
    
    float p_out = pidGains[0]*(error);
    float i_out = pidGains[1]*(integral);
    float d_out = pidGains[2]*(dError);
    out=p_out + i_out + d_out;
    out=constrain(out,-1.0,1.0);
  }

  pressureLast=pressure;
  lastError=error;
  lastTime = currTime;

  return out;
}
