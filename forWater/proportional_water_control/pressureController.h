#include <stdlib.h>

#ifndef __pressureController_H__
#define __pressureController_H__

#include "analog_PressureSensor.h"
#include "valvePair.h"

class pressureController
{
  private:
    analog_PressureSensor sensor; //reference to a pressure sensor object
    valvePair valves; //Reference to a valvepair object 
    int sensePin;
    int sensorType;
    float pressure;
    float pressureSmooth;

    float deadWindow;
    float setpoint;
    float pid[3];
    int controlMode=0;
  
  public:
    pressureController();
    void initialize(int,int, int[]);
    void setSetpoint(float);
    void setDeadWindow(float);
    void setPID(float []);
    void setMode(int);
    float go();
};

#endif
