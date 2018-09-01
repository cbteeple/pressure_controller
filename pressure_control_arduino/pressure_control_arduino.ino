#include "i2c_Mux.h"
#include "i2c_PressureSensor.h"
#include "handleSerialCommands.h"
#include "sensorSettings.h"

int mux_addr   = 0x70;
long lastTime=0;
long currTime=0;

float deadzone_start=0.25;

//Create a mux object
  i2c_Mux mux(mux_addr);

//Create an array of pressure sensor objects
  #define SENSOR_CHIP 1
  #define numSensors 1
  i2c_PressureSensor sensors[numSensors];
  
  float pressures[numSensors];

//Create a new settings object
  sensorSettings settings;

//Create an object to handle serial commands
  handleSerialCommands handleCommands;


int valvePairPins[2]={3,4};





void setup() {
  //Start serial
    Serial.begin(115200);
    Serial.setTimeout(20);

  //Set up valves
    for (int j=0; j<2; j++){
      pinMode(valvePairPins[j], OUTPUT);
      digitalWrite(valvePairPins[j], LOW);
    }
  
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
      digitalWrite(valvePairPins[0], HIGH);
      digitalWrite(valvePairPins[1], LOW); 
    }
    else{
      if (pressures[i] < lowset){
        digitalWrite(valvePairPins[0], LOW);
        digitalWrite(valvePairPins[1], HIGH); 
      }
      else if(pressures[i] > highset){
        digitalWrite(valvePairPins[0], HIGH);
        digitalWrite(valvePairPins[1], LOW); 
      }
      else{
        digitalWrite(valvePairPins[0], LOW);
        digitalWrite(valvePairPins[1], LOW); 
      }
    }
    
  }
}





//PRINT DATA OUT FUNCTION
void printData(){
  //Serial.print(currTime);
  //Serial.print('\t');
  //Serial.print(currTime-lastTime);
  //Serial.print('\t');
  for (int i=0;i<numSensors;i++){
    Serial.print(pressures[i],5);
    Serial.print('\t');  
  }
  Serial.print('\n');
  
}


