#include <stdlib.h>
#include "Arduino.h"


#ifndef __allSettings_H__
#define __allSettings_H__

class internalSettings
{
  public:
    const unsigned int firmware_version[3] = {2,3,6};
    const unsigned int cmd_spec_version[3] = {2,4,0};
    bool master_error = 0;
    bool channel_error = 0;
};


class globalSettings
{
  public:
    unsigned int looptime = 0;
    bool outputsOn=false;
    unsigned int lcdLoopTime = 500;
    bool useMasterPressure=false;
    bool masterPressureOutput=false;
    float maxPressure = 60;
    unsigned int watchdogSpikeTime = 100;
    unsigned int channelWatchedogSpikeTime = 300;
    unsigned int units[2] = {0, 0};
    unsigned long currentTime = 0;
    unsigned long currentTimeOffset = 0;
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
    int adc_res;
    float adc_max_volts;
    float adc_mult;

    float output_min;
    float output_max;
    float output_offset;
    
    float pressure_min; //PSI
    float pressure_max; //PSI
    
};

class valveSettings
{
  public:
    uint8_t valveOffset[2] = {220, 220};
    uint8_t valveMax[2] = {255, 255};
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
    float integralStart = 5.0;
    float maxPressure;
    float minPressure;
    float settime;
    bool reset=false;
};



#endif
