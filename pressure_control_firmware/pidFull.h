#include <stdlib.h>

#ifndef __pidFull_H__
#define __pidFull_H__

#include "allSettings.h"

class pidFull
{
  private:
    float pressureLast;
    float pressureSmooth;
    double integral=0.0L;
    float lastError=0;
    unsigned long timestep;
    unsigned long lastTime=0;

    float setpoint;
    float pidGains[3];
    float deadWindow;

    unsigned int integratorTime = 10;
    bool integratorResetOn = false;
    unsigned long lastIntegratorReset = 0;
    float integralStart=0.05;
    void resetIntegrator();
  
  public:
    pidFull(){};
    void initialize(controlSettings &);
    void updateSettings(controlSettings &);
    void setSetpoint(float);
    void setSetpointDirect(float);
    float go(float);
};

#endif
