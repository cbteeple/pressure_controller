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


//Digital valves

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


//Analog valves
void valvePair::pressureValveAnalog(int val){
   analogWrite(pinPressure, val); 
}

void valvePair::ventValveAnalog(int val){
  analogWrite(pinVent, val); 
}


void valvePair::pressurize(){
  ventValveOff();
  pressureValveOn();
}

void valvePair::pressurizeProportional(float val){
  ventValveAnalog(0);
  float set1=mapFloat(val,0.0,1.0,0.0,float(outRange));
  int set = constrain(set1+ offset, 0, 255);
  pressureValveAnalog(set);
  //Serial.print("Pres_P");
  //Serial.print('\t');
  //Serial.print('\t');
  //Serial.print(set1,6);
  //Serial.print('\t');
  //Serial.print(set);
}

void valvePair::vent(){
  pressureValveOff();
  ventValveOn(); 
}


void valvePair::ventProportional(float val){
  pressureValveAnalog(0);
  float set1=mapFloat(val,0.0,1.0,0.0,float(outRange));
  int set = constrain(set1+ offset, 0, 255);
  ventValveAnalog(set);
  //Serial.print("Vent_P");
  //Serial.print('\t');
  //Serial.print('\t');
  //Serial.print(set1,6);
  //Serial.print('\t');
  //Serial.print(set);
}





void valvePair::idle(){
  pressureValveOff();
  ventValveOff(); 
}



float valvePair::mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




//Update controller settings
void valvePair::go(float act_in){

  //Check for saturation
 /* if (act_in == 0.0 || act_in == -1.0){
    vent();
  }
  else if (act_in == 1.0){
    pressurize();
  }
  //Otherwise, vent proportionally
  else{
  */
  Serial.print(act_in);
  Serial.print('\t');
    if (act_in <0.0){
      ventProportional(abs(act_in));
    }
    else if(act_in ==0.0){
      idle();
    }
    else{
      pressurizeProportional(abs(act_in));
    }  
    
}





