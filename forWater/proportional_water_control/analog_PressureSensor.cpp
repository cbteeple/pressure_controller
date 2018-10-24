#include <stdlib.h>
#include "Arduino.h"

#include "analog_PressureSensor.h"

//analog_PressureSensor::analog_PressureSensor(){}



void analog_PressureSensor::initialize(int sensePin_in){
  sensePin=sensePin_in;
  sensorType = 1;
}



void analog_PressureSensor::initialize(int sensorType_in, int sensePin_in){
  sensorType=sensorType_in;
  sensePin=sensePin_in; 
  setCalibration(sensorType);
}



void analog_PressureSensor::initialize(int sensePin_in,int a, int b, int c, int d){
  sensePin=sensePin_in;
  setCalibration(a,b,c,d);
}


void analog_PressureSensor::setCalibration(int sensorType){
   if (sensorType==1){
      output_max = 4.5;
      output_min = 0.5;
      output_offset = 0.5;
      pressure_max = 50;
      pressure_min = 0;
   }
   else if (sensorType==2){
      output_max = 4.5;
      output_min = 0.5;
      output_offset = 0.5;
      pressure_max = 100;
      pressure_min = 0;
   }
}


void analog_PressureSensor::setCalibration(int a, int b, int c, int d){
  output_max = a;
  output_min = b;
  pressure_max = c;
  pressure_min = d;
}


void analog_PressureSensor::setSmoothing(float alpha_in){
  alpha=alpha_in;
}



void analog_PressureSensor::getData(void){
  //Get the data
  pressureLast=pressure;
                  
  float pressure_volts = float(analogRead(sensePin))*5.0/1028.0;
  pressure = float(pressure_volts-output_offset)*float(pressure_max-pressure_min)/float(output_max-output_min);

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






