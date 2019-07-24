#include <stdlib.h>

#ifndef __valvePair_H__
#define __valvePair_H__

#include "allSettings.h"

class valvePair
{
  private:
    int pinPressure;
    int pinVent;
    int offset_p = 0;
    int offset_v = 0;
    int outMax_p = 255;
    int outMax_v = 255;
    int outRange_p=outMax_p-offset_p;
    int outRange_v=outMax_v-offset_v;
    float mapFloat(float, float, float, float, float);
  
  public:
    valvePair(){};
    valvePair(int, int);
    void initialize();
    void initialize(int, int);
    void setSettings(valveSettings &);
    void pressureValveOn();
    void pressureValveOff();
    void pressureValveAnalog(int);
    void ventValveOn();
    void ventValveOff();
    void ventValveAnalog(int);
    void pressurize();
    void pressurizeProportional(float);
    void vent();
    void ventProportional(float);
    void idle();
    void go(float);
};

#endif
