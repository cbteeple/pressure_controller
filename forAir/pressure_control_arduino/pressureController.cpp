
#include "pressureController.h"



#include "i2c_PressureSensor.h"
#include "i2c_mux.h"
#include "valvePair.h"


pressureController::pressureController(){
  sensor=*(new i2c_PressureSensor());
  valves=*(new valvePair());
}



void pressureController::initialize(i2c_Mux &mux_in, int muxChannel_in, int sensorChip_in, int valveArray[2]){
  mux=mux_in;
  muxChannel=muxChannel_in;
  sensorChip=sensorChip_in;
  sensor.initialize(sensorChip);
  valves.initialize(valveArray[0],valveArray[1]);
}

void pressureController::setSetpoint(float setpoint_in){
  setpoint = setpoint_in;
}

void pressureController::setDeadWindow(float deadWindow_in){
  deadWindow=deadWindow_in;
}


float pressureController::go(){
  //For the future if we decide to run a PD controller or something
  int lastPressure=pressure;

  //Get the pressure
  mux.setActiveChannel(muxChannel);
  sensor.getData();
  pressure=sensor.getPressure();

  //Decide what to do and do it
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

  return pressure;
}
