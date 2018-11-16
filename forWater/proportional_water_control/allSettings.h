#include <stdlib.h>

#ifndef __allSettings_H__
#define __allSettings_H__

class globalSettings
{
  public:
    unsigned int looptime;
    bool outputsOn=false;
};



class sensorSettings
{
  public:
  	//Shared
  	int sensorModel=1;

  	//i2c stuff
  	bool useMux;
    int sensorAddr;
    int muxAddr;
    int muxChannel;

    //Analog stuff
    int sensorPin;
};




class controlSettings
{
  public:
    float setpoint;
    float deadzone;
    int controlMode;
    float pidGains [3];
    float integratorResetTime;
};




#endif
