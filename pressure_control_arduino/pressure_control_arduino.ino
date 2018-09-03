#include "i2c_Mux.h"
#include "i2c_PressureSensor.h"
#include "handleSerialCommands.h"
#include "sensorSettings.h"
#include "valvePair.h"
#include "pressureController.h"


long lastTime=0;
long currTime=0;

//Create a mux object
  i2c_Mux mux(0x70);

//Create an array of pressure sensor objects
  #define SENSOR_CHIP 1
  #define numSensors 4
  //i2c_PressureSensor sensors[numSensors];

  int ave_len=10;
  float pressures[numSensors];

pressureController controller[numSensors];


//Create a new settings object
  sensorSettings settings;

//Create an object to handle serial commands
  handleSerialCommands handleCommands;

//Create valvepair objects (right now only 1)
  //valvePair valves(3,4);
  int valvePins[][2]= {{2,3},{4,5},{6,7},{8,9},{10,11}};

//Create controller objects for pressure control (right now only 1)
float deadzone_start=0.25;


void setup() {
  //Start serial
    Serial.begin(115200);
    Serial.setTimeout(20);
 
  //Initialize the pressure sensor and control objects
    for (int i=0; i<numSensors; i++){
      controller[i].initialize(mux, i, 1, valvePins[i]);
    }

  //Initialize control settings
    handleCommands.initialize(numSensors);
    handleCommands.startBroadcast();
    for (int i=0; i<numSensors; i++){
      settings.deadzones[i]={deadzone_start};
      settings.setpoints[i]=0;
    }
    settings.looptime =0;
    settings.outputsOn=true;
}




void loop() {
  //Handle serial commands
   bool newSettings=handleCommands.go(settings);
  
  //Get pressure readings
    for (int i=0; i<numSensors; i++){
      //if (newSettings){
        controller[i].setSetpoint(settings.setpoints[i]);
        controller[i].setDeadWindow(settings.deadzones[i]);
      //}
      pressures[i] = controller[i].go();
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


