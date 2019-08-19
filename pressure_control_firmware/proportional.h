#include <stdlib.h>

#ifndef __proportional_H__
#define __proportional_H__

#include "allSettings.h"

class proportional
{
  private:
    float pressureLast;
    float pressureSmooth;

    float setpoint;
    float pidGains[3];
  
  public:
    proportional(){};
    void initialize(controlSettings &);
    void updateSettings(controlSettings &);
    void setSetpoint(float);
    float go(float);
};

#endif
