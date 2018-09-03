#include <stdlib.h>

#ifndef __pressureController_H__
#define __pressureController_H__

#include "i2c_PressureSensor.h"
#include "i2c_mux.h"
#include "valvePair.h"

class pressureController
{
  private:
    i2c_Mux mux; //Reference to the mux object (created in the main by the user)
    i2c_PressureSensor sensor; //reference to a pressure sensor object
    valvePair valves; //Reference to a valvepair object 
    int muxChannel;
    int sensorChip;
    float pressure;
    float pressureSmooth;

    float deadWindow;
    float setpoint;
  
  public:
    pressureController();
    void initialize(i2c_Mux &, int, int, int[]);
    void setSetpoint(float);
    void setDeadWindow(float);
    float go();
};

#endif
