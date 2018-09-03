#include "valvePair.h"
#include <Arduino.h>


valvePair::valvePair(int pin1, int pin2){
  pinPressure=pin1;
  pinVent=pin2;
}


void valvePair::initialize(){
  pinMode(pinPressure, OUTPUT);
  digitalWrite(pinPressure, LOW);
  pinMode(pinVent, OUTPUT);
  digitalWrite(pinVent, LOW);
}



void valvePair::pressureValveOn(){
  digitalWrite(pinPressure, HIGH);  
}

void valvePair::pressureValveOff(){
  digitalWrite(pinPressure, LOW);  
}

void valvePair::ventValveOn(){
  digitalWrite(pinVent, HIGH);  
}

void valvePair::ventValveOff(){
  digitalWrite(pinVent, LOW);  
}

void valvePair::pressurize(){
  ventValveOff();
  pressureValveOn();
}

void valvePair::vent(){
  pressureValveOff();
  ventValveOn(); 
}

void valvePair::idle(){
  pressureValveOff();
  ventValveOff(); 
}






