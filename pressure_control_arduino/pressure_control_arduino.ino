#include "i2c_Mux.h"
#include "i2c_PressureSensor.h"
#include "handleSerialCommands.h"
#include "sensorSettings.h"

int mux_addr   = 0x70;
long lastTime=0;
long currTime=0;

float deadzone_start=0.5;

i2c_Mux mux(mux_addr);

//Create an array of pressure sensor objects
#define SENSOR_CHIP 1
#define numSensors 2
i2c_PressureSensor sensors[numSensors];

float pressures[numSensors];

sensorSettings settings;

handleSerialCommands handleCommands;



void setup() {
  Serial.begin(115200);
  Serial.setTimeout(20);
  
  //Initialize the pressure sensor objects
  for (int i=0; i<numSensors; i++){
    sensors[i].initialize(SENSOR_CHIP);
  }

  //Initialize control settings
  handleCommands.initialize(numSensors);
  handleCommands.startBroadcast();
  for (int i=0; i<numSensors; i++){
    settings.deadzones[i]={deadzone_start};
    settings.setpoints[i]=0;
  }
  settings.looptime =100;
}

void loop() {

  handleCommands.go(settings);
  
  //Get pressure readings
  getPressures(pressures);

  //Print out data
  currTime=millis();
  if (settings.outputsOn && (currTime-lastTime>= settings.looptime)){
    printData();
    lastTime=currTime;
  }
  
}


void getPressures(float pressures[]){
  for(int i=0; i<numSensors; i++){
    mux.setActiveChannel(i);
    sensors[i].getData();
    pressures[i]=sensors[i].getPressure();
  }
}


void printData(){
  Serial.print(currTime);
  Serial.print('\t');
  Serial.print(currTime-lastTime);
  Serial.print('\t');
  for (int i=0;i<numSensors;i++){
    Serial.print(pressures[i],5);
    Serial.print('\t');  
  }
  Serial.print('\n');
  
}


