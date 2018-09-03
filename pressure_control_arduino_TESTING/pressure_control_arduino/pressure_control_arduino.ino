#include "i2c_Mux.h"
#include "i2c_PressureSensor.h"
#include "handleSerialCommands.h"
#include "sensorSettings.h"
#include "valvePair.h"


long lastTime=0;
long currTime=0;

//Create a mux object
  i2c_Mux mux(0x70);

//Create an array of pressure sensor objects
  #define SENSOR_CHIP 1
  #define numSensors 4
  i2c_PressureSensor sensors[numSensors];
  
  float pressures[numSensors];



//Create a new settings object
  sensorSettings settings;

//Create an object to handle serial commands
  handleSerialCommands handleCommands;

//Create valvepair objects (right now only 1)
  valvePair valves(3,4);

//Create controller objects for pressure control (right now only 1)
float deadzone_start=0.25;


void setup() {
  //Start serial
    Serial.begin(115200);
    Serial.setTimeout(20);

  //Set up valves
    valves.initialize();
  
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
    settings.outputsOn=true;
}




void loop() {
  //Handle serial commands
    handleCommands.go(settings);
  
  //Get pressure readings
    getPressures(pressures);
    controlPressures(pressures,settings);

  //Print out data
    currTime=millis();
    if (settings.outputsOn && (currTime-lastTime>= settings.looptime)){
      printData();
      lastTime=currTime;
    }
  
}




//PRESSURE ARRAY FUNCTION
void getPressures(float pressures[]){
  for(int i=0; i<numSensors; i++){
    mux.setActiveChannel(i);
    sensors[i].getData();
    pressures[i]=sensors[i].getPressure();
  }
}





//PRESSURE CONTROL FUNCTION
void controlPressures(float pressures[],sensorSettings settings){
  for (int i=0; i<numSensors;i++){
    float lowset=settings.setpoints[i]-settings.deadzones[i];
    float highset=settings.setpoints[i]+settings.deadzones[i];
    if(settings.setpoints[i]==0){
      valves.vent();
    }
    else{
      if (pressures[i] < lowset){
        valves.pressurize();
      }
      else if(pressures[i] > highset){
        valves.vent();
      }
      else{
        valves.idle();
      }
    }
  }
}





//PRINT DATA OUT FUNCTION
void printData(){
  //Serial.print(currTime);
  //Serial.print('\t');
  Serial.print(currTime-lastTime);
  Serial.print('\t');
  for (int i=0;i<numSensors;i++){
    Serial.print(pressures[i],5);
    Serial.print('\t');  
  }
  Serial.print('\n');
  
}


