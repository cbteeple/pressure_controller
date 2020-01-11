#include <stdlib.h>
#include "allSettings.h"

#ifndef __analog_PressureSensor_H__
#define __analog_PressureSensor_H__


class analog_PressureSensor
{
  private:
    int sensePin;
    int sensorType;
    float adc_max_counts = 1023.0;
    float adc_max_volts = 5.0;
        
    //Calibration settings
    float output_max;
    float  output_min;
    float output_offset;
    float pressure_max;
    float pressure_min;
    bool firstCall=true;
    float alpha = 0.01;
    

    //Data
    float pressure;
    float pressureLast;
    float pressureSmooth;
  
  public:
    bool  connected=true;
  
    analog_PressureSensor(){};
    void initialize(sensorSettings &);
    void initialize(sensorSettings &,float,float,float,float, float);
    void setCalibration(float, float, float, float, float);
    void setSmoothing(float);
    void getData(void);
    float getPressure(void);
    float getPressureSmoothed(void);
};

#endif
