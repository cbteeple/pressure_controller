#include <stdlib.h>
#include "Arduino.h"

#include "analog_PressureSensor.h"
#include "allSettings.h"

//analog_PressureSensor::analog_PressureSensor(){}


void analog_PressureSensor::initialize(sensorSettings &senseSet){
  sensorType=senseSet.sensorModel;
  sensePin=senseSet.sensorPin;

  adc_max_counts = pow(2.0,senseSet.adc_res);
  adc_max_volts = senseSet.adc_max_volts;

  setCalibration(senseSet.output_max*senseSet.adc_mult,
                 senseSet.output_min*senseSet.adc_mult, 
                 senseSet.output_offset*senseSet.adc_mult,
                 senseSet.pressure_max,
                 senseSet.pressure_min);
}



void analog_PressureSensor::initialize(sensorSettings &senseSet,float a, float b, float c, float d, float e){
  sensePin=senseSet.sensorPin; 
  setCalibration(a,b,c,d,e);
}


void analog_PressureSensor::setCalibration(float a, float b, float c, float d, float e){
  output_max = a;
  output_min = b;
  output_offset = c;
  pressure_max = d;
  pressure_min = e;
}


void analog_PressureSensor::setSmoothing(float alpha_in){
  alpha=alpha_in;
}



void analog_PressureSensor::getData(void){
  //Get the data
  pressureLast=pressure;

                    
  float pressure_volts = float(analogRead(sensePin))*adc_max_volts/float(adc_max_counts);

  //Choose the correct calibration curve
  if (sensorType<4){
    pressure = float(pressure_volts-output_offset)*float(pressure_max-pressure_min)/float(output_max-output_min);
  }
  else if (sensorType==4){
    pressure = ( float(pressure_volts-output_offset) * float(pressure_max-pressure_min) / float(output_max-output_min) ) + pressure_min;
  }
  else if (sensorType==99){
    pressure = pressure_volts;
  }
  

  if (!firstCall){
    pressureSmooth=alpha*pressure+ (1-alpha)*pressureLast;
  }
  else{
    firstCall=false;
  }
  
}


float analog_PressureSensor::getPressure(void){
  return pressure;
}

float analog_PressureSensor::getPressureSmoothed(void){
  return pressureSmooth;
}
