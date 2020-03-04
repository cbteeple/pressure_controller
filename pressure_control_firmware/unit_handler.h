#include <stdlib.h>
#include "allSettings.h"
#include "Arduino.h"

#ifndef __unit_handler_H__
#define __unit_handler_H__

class UnitHandler
{
  private:
    int in_units = 0;
    float in_mult=1.0;
    String in_name = "psi";

    int out_units = 0;
    float out_mult=1.0;
    String out_name = "psi";

    void unitsEnum(int, float &, String & );
  
  public:
    UnitHandler(){};
    void setUnits(int);
    void setInputUnits(int);
    void setOutputUnits(int);
    float convertToInternal(float);
    float convertToExternal(float);
    String getInputName();
    String getOutputName();
};

#endif
