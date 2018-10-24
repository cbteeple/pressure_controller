#include <stdlib.h>
#include <Wire.h>
#include "Arduino.h"

#include "i2c_Mux.h"



i2c_Mux::i2c_Mux(int mux_addr_in){
  mux_addr=mux_addr_in;
  //Start i2c stuff
  if (TWCR == 0){ // do this check so that Wire only gets initialized once
    Wire.begin();
    Wire.setClock(400000L);
  }
}

void i2c_Mux::setActiveChannel(int channel){
  delay(1);
  Wire.beginTransmission(mux_addr); // transmit to device
  Wire.write(1 << channel);      
  Wire.endTransmission();      // stop transmitting
  delay(1);
}
