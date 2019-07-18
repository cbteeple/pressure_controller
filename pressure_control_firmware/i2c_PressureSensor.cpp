#include <stdlib.h>
#include <Wire.h>
#include "Arduino.h"

#include "i2c_PressureSensor.h"
#include "allSettings.h"

//i2c_PressureSensor::i2c_PressureSensor(){}






void i2c_PressureSensor::initialize(sensorSettings &senseSet){
  sensor_addr=senseSet.sensorAddr;
  use_mux = senseSet.useMux;
  mux_addr=senseSet.muxAddr;
  mux_channel=senseSet.muxChannel;
     
  setCalibration(senseSet.sensorModel);

  //if (TWCR == 0){ // do this check so that Wire only gets initialized once
    Wire.begin();
    Wire.setClock(400000L);
  //}

  setMuxChannel(mux_channel);
  
}



void i2c_PressureSensor::initialize(sensorSettings &senseSet,int a, int b, int c, int d){
  sensor_addr=senseSet.sensorAddr;
  use_mux = senseSet.useMux;
  if (use_mux){
    mux_addr=senseSet.muxAddr;
    mux_channel=senseSet.muxChannel;
  }
  setCalibration(a,b,c,d);

  //if (TWCR == 0){ // do this check so that Wire only gets initialized once
    Wire.begin();
    Wire.setClock(400000L);
  //}
}


void i2c_PressureSensor::setCalibration(int sensorModel){
   if (sensorModel==1){
      output_max = 14745;
      output_min = 1638;
      pressure_max = 100;
      pressure_min = 0;
   }
   else if (sensorModel==2){
      output_max = 14745;
      output_min = 1638;
      pressure_max = 30;
      pressure_min = -30;
   }
}


void i2c_PressureSensor::setCalibration(int a, int b, int c, int d){
  output_max = a;
  output_min = b;
  pressure_max = c;
  pressure_min = d;
}




void i2c_PressureSensor::setMuxChannel(int channel){
  delay(1);
  Wire.beginTransmission(mux_addr); // transmit to device
  Wire.write(1 << channel);      
  Wire.endTransmission();      // stop transmitting
  delay(1);
}







void i2c_PressureSensor::setSmoothing(float alpha_in){
  alpha=alpha_in;
}



void i2c_PressureSensor::getData(void){
  //Get the data
  //Wire.beginTransmission(sensor_addr); // transmit to device
  //Wire.write(byte(data_reg));      
  //Wire.endTransmission();      // stop transmitting

  //If we are using a mux, use it
  if (use_mux){
    setMuxChannel(mux_channel);
  }


  //Prepare for smoothing
  pressureLast=pressure;
  
  //Ask the sensor for data      
  Wire.requestFrom(sensor_addr, 2, true);    // request 2 bytes from slave device #112
  int pressure1 = Wire.read();
  int pressure0 = Wire.read();

  //combine things and do math
  pressure1|=0<<7;
  pressure1|=0<<6;

  long p_comb = pressure1<<8 | pressure0;
  float pressure_tmp = long((p_comb - output_min)*(pressure_max-pressure_min))/float(output_max-output_min)+pressure_min;

  //Do some error checking
  if (pressure_tmp>-10){
    pressure = pressure_tmp;
  }

  if (!firstCall){
    pressureSmooth=alpha*pressure+ (1-alpha)*pressureLast;
  }
  else{
    firstCall=false;
  }
  
}


float i2c_PressureSensor::getPressure(void){
  return pressure;
}

float i2c_PressureSensor::getPressureSmoothed(void){
  return pressureSmooth;
}
