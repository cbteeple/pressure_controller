#include <stdlib.h>
#include <Wire.h>
#include "Arduino.h"

#include "i2c_PressureSensor.h"

//i2c_PressureSensor::i2c_PressureSensor(){}



void i2c_PressureSensor::initialize(int sensorType){
  if (sensorType==1)
    sensor_addr=0x58;
  else if (sensorType==2)
    sensor_addr=0x38;
    
  setCalibration(sensorType);

  if (TWCR == 0){ // do this check so that Wire only gets initialized once
    Wire.begin();
    Wire.setClock(400000L);
  }
}





void i2c_PressureSensor::initialize(int sensorType,int addr_in){
  sensor_addr=addr_in;    
  setCalibration(sensorType);

  if (TWCR == 0){ // do this check so that Wire only gets initialized once
    Wire.begin();
    Wire.setClock(400000L);
  }
}



void i2c_PressureSensor::initialize(int addr_in,int a, int b, int c, int d){
  sensor_addr=addr_in;
  setCalibration(a,b,c,d);

  if (TWCR == 0){ // do this check so that Wire only gets initialized once
    Wire.begin();
    Wire.setClock(400000L);
  }
}


void i2c_PressureSensor::setCalibration(int sensorType){
   if (sensorType==1){
      output_max = 14745;
      output_min = 1638;
      pressure_max = 100;
      pressure_min = 0;
   }
   else if (sensorType==2){
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



void i2c_PressureSensor::getData(void){
  //Get the data
  //Wire.beginTransmission(sensor_addr); // transmit to device
  //Wire.write(byte(data_reg));      
  //Wire.endTransmission();      // stop transmitting
                  
  Wire.requestFrom(sensor_addr, 2, true);    // request 2 bytes from slave device #112
  int pressure1 = Wire.read();
  int pressure0 = Wire.read();

  //combine things and do math
  pressure1|=0<<7;
  pressure1|=0<<6;

  long p_comb = pressure1<<8 | pressure0;
  pressure = long((p_comb - output_min)*(pressure_max-pressure_min))/float(output_max-output_min)+pressure_min;


  
}


float i2c_PressureSensor::getPressure(void){
  return pressure;
}






