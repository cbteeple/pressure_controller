#include <stdlib.h>

#ifndef __sensorSettings_H__
#define __sensorSettings_H__

class globalSettings
{
  public:
    unsigned int looptime;
    bool outputsOn=false;
};


class controlSettings
{
  public:
    float deadzone;
    float setpoint;
    float pidGains [3];
    int controlMode;
};

#endif
