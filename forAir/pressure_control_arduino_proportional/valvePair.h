#include <stdlib.h>

#ifndef __valvePair_H__
#define __valvePair_H__

class valvePair
{
  private:
    int pinPressure;
    int pinVent;
    int offset = 235;
    int outRange=255-offset;
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
};

#endif
