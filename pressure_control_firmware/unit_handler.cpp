#include "unit_handler.h"
#include "allSettings.h"
#include "Arduino.h"


void UnitHandler::unitsEnum(int new_units, float &mult, String &name){
  switch (new_units) {
    case 0 : { //psi - same as interal pressure
      mult = 1.0;
      name = "psi";
    } break; 
    case 1 : { //kPa - convert
      mult = 6.89475729;
      name = "kPa";
    } break; 
    case 2 : { //bar - convert
      mult = 0.0689475728;
      name = "bar";
    } break; 
    case 3 : { //atm - convert
      mult = 0.0680459638; 
      name = "atm";
    } break; 
  }
}

//Save global settings
void UnitHandler::setUnits(int new_units){
  unitsEnum(new_units, in_mult, in_name);
  
  in_units = new_units;
  out_mult = in_mult;
  out_name = in_name;
  out_units = new_units;
}

void UnitHandler::setInputUnits(int new_units){
  unitsEnum(new_units, in_mult, in_name);
  in_units = new_units;
}

void UnitHandler::setOutputUnits(int new_units){
  unitsEnum(new_units, out_mult, out_name);
  out_units = new_units;
}

float UnitHandler::convertToInternal(float pressure_in){
  return pressure_in/in_mult;
}


float UnitHandler::convertToExternal(float pressure_in){
  return pressure_in*out_mult;
}

String UnitHandler::getInputName(){
  return in_name;
}

String UnitHandler::getOutputName(){
  return out_name;
}
