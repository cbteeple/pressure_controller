#include <stdlib.h>

#ifndef __analog_PressureSensor_H__
#define __analog_PressureSensor_H__


class analog_PressureSensor
{
  private:
    int sensePin;
    int sensorType;
        
    //Calibration settings
    float output_max;
    float  output_min;
    float output_offset;
    float pressure_max;
    float pressure_min;
    void setCalibration(int);
    bool firstCall=true;
    float alpha = 0.01;
    

    //Data
    float pressure;
    float pressureLast;
    float pressureSmooth;
  
  public:
    analog_PressureSensor(){};
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
