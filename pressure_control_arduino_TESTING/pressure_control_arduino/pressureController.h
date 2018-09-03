#include <stdlib.h>

#ifndef __pressureController_H__
#define __pressureController_H__

#include "i2c_PressureSensor.h"
#include "i2c_mux.h"
#include "valvePair.h"

class pressureController
{
  private:
    i2c_mux mux; //Reference to the mux object (created in the main by the user)
    i2c_PressureSensor sensor; //reference to a pressure sensor object
    valvePair valves; //Reference to a valvepair object 
    int muxChannel;
    float pressure;
    int sensorChip;
  
  public:
    pressureController(i2c_mux &, int, i2c_PressureSensor &, int, valvePair &);
    void initialize();
    float go();
};

#endif
