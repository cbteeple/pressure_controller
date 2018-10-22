#include "valvePair.h"
#include <Arduino.h>


valvePair::valvePair(int pin1, int pin2){
  pinPressure=pin1;
  pinVent=pin2;
}




void valvePair::initialize(int pin1, int pin2){
  pinPressure=pin1;
  pinVent=pin2;
  pinMode(pinPressure, OUTPUT);
  digitalWrite(pinPressure, LOW);
  pinMode(pinVent, OUTPUT);
  digitalWrite(pinVent, LOW);
}



void valvePair::initialize(){
  pinMode(pinPressure, OUTPUT);
  digitalWrite(pinPressure, LOW);
  pinMode(pinVent, OUTPUT);
  digitalWrite(pinVent, LOW);
}


void valvePair::setSettings(int offset_in, int range_in){
  offset=offset_in;
  outRange=range_in;
}




void valvePair::pressureValveOn(){
  digitalWrite(pinPressure, HIGH);  
}

void valvePair::pressureValveOff(){
  digitalWrite(pinPressure, LOW);  
}

void valvePair::pressureValveAnalog(int val){
  analogWrite(pinPressure, val);  
}

void valvePair::ventValveOn(){
  digitalWrite(pinVent, HIGH);  
}

void valvePair::ventValveOff(){
  digitalWrite(pinVent, LOW);  
}

void valvePair::ventValveAnalog(int val){
  analogWrite(pinVent, val);  
}


void valvePair::pressurize(){
  ventValveOff();
  pressureValveOn();
}

void valvePair::pressurizeProportional(float val){
  ventValveOff();
  float set1=mapFloat(val,0.0,1.0,0.0,float(outRange));
  int set = constrain(set1+ offset, 0, 255);
  pressureValveAnalog(set);
  Serial.print('\t');
  Serial.print(set1,6);
  Serial.print('\t');
  Serial.print(set);
}

void valvePair::vent(){
  pressureValveOff();
  ventValveOn(); 
}


void valvePair::ventProportional(float val){
  pressureValveOff();
  float set1=mapFloat(val,0.0,1.0,0.0,float(outRange));
  int set = constrain(set1+ offset, 0, 255);
  ventValveAnalog(set);
  Serial.print('\t');
  Serial.print(set1,6);
  Serial.print('\t');
  Serial.print(set);
}





void valvePair::idle(){
  pressureValveOff();
  ventValveOff(); 
}



float valvePair::mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




