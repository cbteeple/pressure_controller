#include <stdlib.h>

#ifndef __sensorSettings_H__
#define __sensorSettings_H__

class sensorSettings
{
  public:
    float deadzones[20];
    float setpoints[20];
    unsigned int looptime;
    bool outputsOn=false;
};

#endif
