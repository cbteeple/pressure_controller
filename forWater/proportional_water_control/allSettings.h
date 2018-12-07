#include <stdlib.h>

#ifndef __allSettings_H__
#define __allSettings_H__

class globalSettings
{
  public:
    unsigned int looptime = 0;
    bool outputsOn=false;
    
};



class sensorSettings
{
  public:
  	//Shared
    bool on = true;
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
    bool channelOn = true;
    float setpoint;
    float deadzone;
    int controlMode;
    float valveDirect;
    float pidGains [3];
    float integratorResetTime;
    float integralStart = 3.0;
};




#endif
