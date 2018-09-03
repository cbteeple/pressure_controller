#include <stdlib.h>

#ifndef __valvePair_H__
#define __valvePair_H__

class valvePair
{
  private:
    int pinPressure;
    int pinVent;
  
  public:
    valvePair(int, int);
    void initialize();
    void pressureValveOn();
    void pressureValveOff();
    void ventValveOn();
    void ventValveOff();
    void pressurize();
    void vent();
    void idle();
};

#endif
