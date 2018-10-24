
#include "pressureController.h"

pressureController::pressureController(i2c_mux &mux_in, int muxChannel_in, i2c_PressureSensor & sensor_in, int sensorChip_in, valvePair &valves_in){
  mux=mux_in;
  muxChannel=muxChannel_in;
  sensor=sensor_in;
  valves=valves_in;
  sensorChip=sensorChip_in
}

pressureController::initialize(){
  sensor.initialize(SENSOR_CHIP)
}


float pressureController::go(){
  int lastPressure=pressure;

  pressure=sensor.


  return pressure
}
