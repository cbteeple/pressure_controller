#include <stdlib.h>

#ifndef __pidFull_H__
#define __pidFull_H__

#include "allSettings.h"

class pidFull
{
  private:
    float pressureLast;
    float pressureSmooth;
    float integral=0;
    float lastError=0;
    unsigned long timestep;
    unsigned long lastTime=0;

    float setpoint;
    float pidGains[3];
    float deadWindow;

    unsigned int integratorTime = 10;
    unsigned long lastIntegratorReset = 0;
    void resetIntegrator();
  
  public:
    pidFull(){};
    void initialize(controlSettings &);
    void updateSettings(controlSettings &);
    void setSetpoint(float);
    float go(float);
};

#endif
