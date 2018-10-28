#include "analog_PressureSensor.h"
//#include "i2c_PressureSensor.h"
#include "handleSerialCommands.h"
#include "sensorSettings.h"
#include "valvePair.h"
#include "bangBang.h"


//Create an array of pressure sensor objects (Uncomment the one you want)
#define SENSOR_ANALOG true
//#define SENSOR_I2C true

#define SENSOR_MODEL 1
#define NUM_SENSORS 1


//Create a new settings object
globalSettings settings;
controlSettings ctrlSettings[NUM_SENSORS];

//Create an object to handle serial commands
handleSerialCommands handleCommands;

//Set up sensing
  //Arduino ADC pins:
    //Mega:  A0-A15                        (all pins work fine)
    //Uno:   A0-A5                         (all pins work fine)
    //Micro: A0, A1, A4 - A8, A11          (A9, A10, A12, A13 all conflict with PWM pins)
    //Nano:  A0-A7                         (A4, A5 are i2c pins)


#if(SENSOR_ANALOG)
  int senseChannels[]={A0,A1,A2,A3};
  analog_PressureSensor sensors[NUM_SENSORS];
#elif(SENSOR_I2C)
  int senseChannels[]={0,1,2,3};
  i2c_PressureSensor sensors[NUM_SENSORS];
#endif

int ave_len=10;
float pressures[NUM_SENSORS];
float valveSets[NUM_SENSORS];



//Set up valve pairs in an array
  //Arduino PWM pins:
    //Mega:  2 - 13, 44, 45, 46            (all pins work fine)
    //Uno:   3, 5, 6, 9, 10, 11            (all pins work fine)
    //Micro: 3, 5, 6, 9, 10, 11, 12, 13    (all pins work fine)
    //Nano:  3, 6, 9, 10, 11               (pin 5 fails consistently, no idea why)

  
valvePair valves[NUM_SENSORS];
int valvePins[][2]= { {6,9}, {10,11} };


//Create an array of controller objects for pressure control
bangBang controllers[NUM_SENSORS];


float deadzone_start=0.25;
float setpoint_start=0;
//float pid_start[]={0.2,0,0};
//int mode_start = 1;      



//Set up output task manager variables
long lastTime=0;
long currTime=0;




//______________________________________________________________________
void setup() {
  //Start serial
    Serial.begin(115200);
    Serial.setTimeout(20);
 
  //Initialize control settings
    handleCommands.initialize(NUM_SENSORS);
    handleCommands.startBroadcast();
    for (int i=0; i<NUM_SENSORS; i++){
      ctrlSettings[i].deadzone=deadzone_start;
      ctrlSettings[i].setpoint=setpoint_start;
      
      /*ctrlSettings[i].controlMode = mode_start;

      for (int j=0; j<3; j++){
        ctrlSettings[i].pidGains[j]=pid_start[j];
      }
      */
      
    }


  //Initialize the pressure sensor and control objects
    for (int i=0; i<NUM_SENSORS; i++){
      sensors[i].initialize(SENSOR_MODEL,senseChannels[i]);
      valves[i].initialize(valvePins[i][0],valvePins[i][1]);
      controllers[i].initialize(ctrlSettings[i]);
    }


    
    settings.looptime =0;
    settings.outputsOn=false;
}




//______________________________________________________________________
void loop() {
  //Handle serial commands
   bool newSettings=handleCommands.go(settings, ctrlSettings);
  
  //Get pressure readings
    for (int i=0; i<NUM_SENSORS; i++){
      //Get the new pressures
      sensors[i].getData();
      pressures[i] = sensors[i].getPressure();
      
      //Update controller settings
      //if (newSettings){
        controllers[i].updateSettings(ctrlSettings[i]);
        controllers[i].setSetpoint(ctrlSettings[i].setpoint);
        //controllers[i].setPID(ctrlSettings[i].pidGains);
        //controllers[i].setMode(ctrlSettings[i].controlMode);
      //}

      //Run 1 step of the controller
      valveSets[i] = controllers[i].go(pressures[i]);


      //Send new actuation signal to the valves
      valves[i].go( valveSets[i] );

      
      Serial.print('\n');
    }

  //Print out data at close to the correct rate
    currTime=millis();
    if (settings.outputsOn && (currTime-lastTime>= settings.looptime)){
      printData();
      lastTime=currTime;
    }
  
}


//______________________________________________________________________


//PRINT DATA OUT FUNCTION
void printData(){
  //Serial.print(currTime);
  //Serial.print('\t');
  //Serial.print(currTime-lastTime);
  //Serial.print('\t');
  for (int i=0;i<NUM_SENSORS;i++){
    Serial.print(pressures[i],5);
    Serial.print('\t');  
  }
  Serial.print('\n');
  
}


