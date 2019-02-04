#include <stdlib.h>

#ifndef __i2c_PressureSensor_H__
#define __i2c_PressureSensor_H__

#include "allSettings.h"

class i2c_PressureSensor
{
  private:
    int sensor_addr; //address of the sensor
    bool mux_addr;
    bool use_mux;
    int mux_channel;

    
    //Calibration settings
    int output_max;
    int output_min;
    int pressure_max;
    int pressure_min;
    void setCalibration(int);
    void setMuxChannel(int);
    bool firstCall=true;
    float alpha = 0.01;
    

    //Data
    float pressure;
    float pressureLast;
    float pressureSmooth;
  
  public:
    i2c_PressureSensor(){};
    void initialize(sensorSettings &);
    void initialize(sensorSettings &,int,int,int,int);
    void setCalibration(int, int, int, int);
    void setSmoothing(float);
    void getData(void);
    float getPressure(void);
    float getPressureSmoothed(void);
};

#endif

