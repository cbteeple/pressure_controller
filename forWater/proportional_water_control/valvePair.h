#include <stdlib.h>

#ifndef __valvePair_H__
#define __valvePair_H__

class valvePair
{
  private:
    int pinPressure;
    int pinVent;
    int offset = 0;//220;
    int outMax=255;
    int outRange=outMax-offset;
    float mapFloat(float, float, float, float, float);
  
  public:
    valvePair(){};
    valvePair(int, int);
    void initialize();
    void initialize(int, int);
    void setSettings(int, int);
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

