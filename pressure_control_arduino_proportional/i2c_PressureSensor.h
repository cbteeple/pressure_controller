#include <stdlib.h>

#ifndef __i2c_PressureSensor_H__
#define __i2c_PressureSensor_H__


class i2c_PressureSensor
{
  private:
    int sensor_addr; //address of the sensor
    
    //Calibration settings
    int output_max;
    int output_min;
    int pressure_max;
    int pressure_min;
    void setCalibration(int);
    bool firstCall=true;
    float alpha = 0.01;
    

    //Data
    float pressure;
    float pressureLast;
    float pressureSmooth;
  
  public:
    i2c_PressureSensor(){};
    void initialize(int);
    void initialize(int,int);
    void initialize(int,int,int,int,int);
    void setCalibration(int, int, int, int);
    void setSmoothing(float);
    void getData(void);
    float getPressure(void);
    float getPressureSmoothed(void);
};

#endif
