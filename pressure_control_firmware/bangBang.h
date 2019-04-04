#include <stdlib.h>

#ifndef __bangBang_H__
#define __bangBang_H__

#include "allSettings.h"

class bangBang
{
  private:
    float pressureLast;
    float pressureSmooth;

    float deadWindow;
    float setpoint;
  
  public:
    bangBang(){};
    void initialize(controlSettings &);
    void updateSettings(controlSettings &);
    void setSetpoint(float);
    float go(float);
};

#endif


