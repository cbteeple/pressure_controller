#include "analog_PressureSensor.h"
#include "handleSerialCommands.h"
#include "sensorSettings.h"
#include "valvePair.h"
#include "pressureController.h"


long lastTime=0;
long currTime=0;

//Create an array of pressure sensor objects
  #define SENSOR_TYPE 1
  #define numSensors 1
  //analog_PressureSensor sensors[numSensors];

  int ave_len=10;
  float pressures[numSensors];

pressureController controller[numSensors];


//Create a new settings object
  globalSettings settings;
  controlSettings ctrlSettings[numSensors];

//Create an object to handle serial commands
  handleSerialCommands handleCommands;

//Create valvepair objects (right now only 1)
  //valvePair valves(3,4);
  //int valvePins[][2]= {{3,5},{6,9},{10,11}};
  int valvePins[][2]= {{6,9},{10,11}};
  int sensePins[]={A0,A1,A2,A3};

//Create controller objects for pressure control (right now only 1)
float deadzone_start=0.25;
float setpoint_start=0;
float pid_start[]={0.2,0,0};
int mode_start = 1;


void setup() {
  //Start serial
    Serial.begin(115200);
    Serial.setTimeout(20);
 
  //Initialize the pressure sensor and control objects
    for (int i=0; i<numSensors; i++){
      controller[i].initialize(sensePins[i],SENSOR_TYPE, valvePins[i]);
    }

  //Initialize control settings
    handleCommands.initialize(numSensors);
    handleCommands.startBroadcast();
    for (int i=0; i<numSensors; i++){
      ctrlSettings[i].deadzone={deadzone_start};
      ctrlSettings[i].setpoint=setpoint_start;
      ctrlSettings[i].controlMode = mode_start;

      for (int j=0; j<3; j++){
        ctrlSettings[i].pidGains[j]=pid_start[j];
      }
    }
    settings.looptime =0;
    settings.outputsOn=false;
}




void loop() {
  //Handle serial commands
   bool newSettings=handleCommands.go(settings, ctrlSettings);
  
  //Get pressure readings
    for (int i=0; i<numSensors; i++){
      //if (newSettings){
        controller[i].setSetpoint(ctrlSettings[i].setpoint);
        controller[i].setDeadWindow(ctrlSettings[i].deadzone);
        controller[i].setPID(ctrlSettings[i].pidGains);
        controller[i].setMode(ctrlSettings[i].controlMode);
      //}
      pressures[i] = controller[i].go();
      Serial.print('\n');
    }

  //Print out data at close to the correct rate
    currTime=millis();
    if (settings.outputsOn && (currTime-lastTime>= settings.looptime)){
      printData();
      lastTime=currTime;
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


