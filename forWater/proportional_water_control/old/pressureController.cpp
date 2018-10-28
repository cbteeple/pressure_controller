
#include "pressureController.h"



#include "analog_PressureSensor.h"
#include "valvePair.h"
#include <Arduino.h>


pressureController::pressureController(){
  sensor=*(new analog_PressureSensor());
  valves=*(new valvePair());
}



void pressureController::initialize(int channel_in, int sensorType, int valveArray[2]){
  sensePin=channel_in;
  sensor.initialize(sensorType,sensePin);
  valves.initialize(valveArray[0],valveArray[1]);
}

void pressureController::setSetpoint(float setpoint_in){
  setpoint = setpoint_in;
}

void pressureController::setDeadWindow(float deadWindow_in){
  deadWindow=deadWindow_in;
}

void pressureController::setPID(float pidIn[]){
  for (int i=0; i<3; i++){
    pid[i]= pidIn[i];
  }
}

void pressureController::setMode(int modeIn){
  controlMode=modeIn;
}


float pressureController::go(){
  //For the future if we decide to run a PD controller or something
  int lastPressure=pressure;

  //Get the pressure
  sensor.getData();
  pressure=sensor.getPressure();

  //Decide what to do and do it
  switch (controlMode){
    // Bang-Bang control
    case 0:{
      float lowset=setpoint-deadWindow;
      float highset=setpoint+deadWindow;
      if(setpoint==0){
        valves.vent();
      }
      else{
        if (pressure < lowset){
          valves.pressurize();
        }
        else if(pressure > highset){
          valves.vent();
        }
        else{
          valves.idle();
        }
      }
    }
    
    //P - control
    case 1:{
      float set = pid[0]*(setpoint-pressure);
      Serial.print(setpoint);
      Serial.print('\t');
      Serial.print(pressure);
      Serial.print('\t');
      Serial.print(set);
      Serial.print('\t');
      if (set <=0.0){
        valves.ventProportional(abs(set));
      }
      else{
        valves.pressurizeProportional(abs(set));
      }
      
    }
  }
  
  return pressure;
}
