#include "analog_PressureSensor.h"
#include "i2c_PressureSensor.h"
#include "handleSerialCommands.h"
#include "allSettings.h"
#include "valvePair.h"
#include "bangBang.h"
#include "proportional.h"
#include "pidFull.h"

#include <EasyLCD.h>

//SENSING:
  //Arduino ADC pins:
    //Mega:  A0-A15                        (all pins work fine)
    //Uno:   A0-A5                         (all pins work fine)
    //Micro: A0, A1, A4 - A8, A11          (A9, A10, A12, A13 all conflict with PWM pins)
    //Nano:  A0-A7                         (A4, A5 are i2c pins)

//VALVES:
  //Arduino PWM pins:
    //Mega:  2 - 13, 44, 45, 46            (all pins work fine)
    //Uno:   3, 5, 6, 9, 10, 11            (all pins work fine)
    //Micro: 3, 5, 6, 9, 10, 11, 12, 13    (all pins work fine)
    //Nano:  3, 6, 9, 10, 11               (pin 5 fails consistently, no idea why)




//Define the type of sensor to use (only one can be true)
#define SENSOR_ANALOG true
#define SENSOR_I2C false

#define SENSOR_MODEL 1
#define MAX_NUM_CHANNELS 6

//Define the type of controller to use (only one can be true)
#define CONTROL_BANGBANG false
#define CONTROL_P false
#define CONTROL_PID true


//Set default settings for things
//If using i2c sensors...
  int sensorAddr=0x58;
  bool useMux=true;
  int muxAddr=0x70;

//Set valve pins
  //int valvePins[][2]= { {6,9}, {10,11} };
  int valvePins[][2]= { {2,3}, {4,5}, {6,7}, {8,9}, {10,11}, {12,13} };
  int valveOffset=205;

//Default controller settings
  float deadzone_start=0.0;
  float setpoint_start=0;
  float pid_start[]={0.6,0.03,0}; 
  float integratorResetTime_start = -1;
  float minPressure_start = 0; //[psi]
  float maxPressure_start = 20; //[psi]


//Create a new settings object
globalSettings settings;
controlSettings ctrlSettings[MAX_NUM_CHANNELS];
sensorSettings senseSettings[MAX_NUM_CHANNELS];

//Create an object to handle serial commands
handleSerialCommands handleCommands;

//Set up sensing
#if(SENSOR_ANALOG)
  int senseChannels[]={A0,A1,A2,A3};
  analog_PressureSensor sensors[MAX_NUM_CHANNELS];
#elif(SENSOR_I2C)
  int senseChannels[]={0,1,2,3,4,5,6,7};
  i2c_PressureSensor sensors[MAX_NUM_CHANNELS];
#endif

int ave_len=10;
float pressures[MAX_NUM_CHANNELS];
float valveSets[MAX_NUM_CHANNELS];


//Set up valve pairs  
valvePair valves[MAX_NUM_CHANNELS];


//Create an array of controller objects for pressure control
#if CONTROL_BANGBANG
  bangBang controllers[MAX_NUM_CHANNELS];
#elif CONTROL_P
  proportional controllers[MAX_NUM_CHANNELS];
#elif CONTROL_PID
  pidFull controllers[MAX_NUM_CHANNELS];
#endif 


//Set up LCD Screen
uint8_t lcd_addr = 0x27;
EasyLCD lcd(lcd_addr, 16, 2);

bool lcdAttached= false;
int currLCDIndex=0;

//Set up output task manager variables
unsigned long previousTime=0;
unsigned long previousLCDTime=0;
unsigned long currentTime=0;




//______________________________________________________________________
void setup() {
  //Start serial
    Serial.begin(115200);
    //Serial.flush();
    Serial.setTimeout(10);
 
  //Initialize control settings
    handleCommands.initialize(MAX_NUM_CHANNELS);
    handleCommands.startBroadcast();
    for (int i=0; i<MAX_NUM_CHANNELS; i++){
      
      //Initialize control settings
      ctrlSettings[i].setpoint=setpoint_start;
      ctrlSettings[i].maxPressure = maxPressure_start;
      ctrlSettings[i].minPressure = minPressure_start;
      ctrlSettings[i].controlMode = 1;
      ctrlSettings[i].valveDirect = 0;
      
      if(CONTROL_BANGBANG){
        ctrlSettings[i].deadzone=deadzone_start;
      }
      else if (CONTROL_P || CONTROL_PID){
        for (int j=0; j<3; j++){
          ctrlSettings[i].pidGains[j]=pid_start[j];
          ctrlSettings[i].deadzone=deadzone_start;
          ctrlSettings[i].integratorResetTime=integratorResetTime_start;
        }
      }


      //Initialize sensor settings
      senseSettings[i].sensorModel=SENSOR_MODEL;

      if(SENSOR_ANALOG){
        senseSettings[i].sensorPin=senseChannels[i];
      }
      else if(SENSOR_I2C){
        senseSettings[i].sensorAddr= sensorAddr;
        senseSettings[i].useMux     = useMux;
        senseSettings[i].muxAddr    = muxAddr;
        senseSettings[i].muxChannel = senseChannels[i];
      }
      
    }


  //Initialize the pressure sensor and control objects
    for (int i=0; i<MAX_NUM_CHANNELS; i++){
      sensors[i].initialize(senseSettings[i]);
      valves[i].initialize(valvePins[i][0],valvePins[i][1]);
      valves[i].setSettings(valveOffset,255);
      controllers[i].initialize(ctrlSettings[i]);
    }

    
    settings.looptime =0;
    settings.lcdLoopTime = 333;
    settings.outputsOn=false;

    // Initialize the LCD
    lcdAttached = lcd.begin();
    if (!lcdAttached){
      Serial.println("LCD Not Attached!");  
    }
    lcd.clearOnUpdate(false);

}




//______________________________________________________________________
void loop() {
  //Serial.println("_words need to be here (for some reason)");
  //Handle serial commands
   bool newSettings=handleCommands.go(settings, ctrlSettings);
  
  //Get pressure readings
    for (int i=0; i<MAX_NUM_CHANNELS; i++){   
      //Update controller settings if there are new ones
      if (newSettings){
        controllers[i].updateSettings(ctrlSettings[i]);
        controllers[i].setSetpoint(ctrlSettings[i].setpoint);
      }
      
        //Get the new pressures
        sensors[i].getData();
        pressures[i] = sensors[i].getPressure();
          
        if (ctrlSettings[i].channelOn){
          
        if (ctrlSettings[i].controlMode==1){
    
          //Run 1 step of the controller
          valveSets[i] = controllers[i].go(pressures[i]);
        }
        else{
           valveSets[i]=ctrlSettings[i].valveDirect;
        }
    
          //Send new actuation signal to the valves
          valves[i].go( valveSets[i] );
          
          //Serial.print('\n');
        
      }
      else{
        //pressures[i]=0;
      }
    }

  //Print out data at close to the correct rate
  currentTime=millis();
  if (settings.outputsOn && (currentTime-previousTime>= settings.looptime)){
    Serial.print(currentTime);
    Serial.print('\t');
    Serial.print(currentTime-previousTime);
    Serial.print('\t');
    printData();
    previousTime=currentTime;
  }

  if (lcdAttached && (currentTime-previousLCDTime>= settings.lcdLoopTime/MAX_NUM_CHANNELS)){
    //lcdUpdate();
    lcdUpdateDistributed();
    previousLCDTime=currentTime;
  }
    
  
}


//______________________________________________________________________


//PRINT DATA OUT FUNCTION
void printData(){
  for (int i=0; i<MAX_NUM_CHANNELS; i++){
    Serial.print(pressures[i],4);
    Serial.print('\t');  
  }
  Serial.print('\n');
  
}



//LCD UPDATE FUNCTION
void lcdUpdate(){
  String toWrite = "";
  for (int i=0; i<MAX_NUM_CHANNELS; i++){
    if ( i>0 && (i)%3 == 0){
      toWrite += '\n';
    }

    String strTmp="";
    if (ctrlSettings[i].channelOn){  
      strTmp = String(pressures[i], 1);
      if (strTmp.length() <4){
        strTmp =' '+strTmp;
      }
    }
    else{
      strTmp = "<  >";
    }
    
    toWrite += strTmp;
    toWrite += ' ';
  }
  lcd.write(toWrite);  
}


void lcdUpdateDistributed(){
  
  String toWrite = "";
  
  int i = currLCDIndex;

  int currRow = currLCDIndex/3;
  int currCol = (currLCDIndex-currRow*3)*5;
  
  String strTmp="";
  if (ctrlSettings[i].channelOn){  
    strTmp = String(pressures[i], 1);
    if (strTmp.length() <4){
      strTmp =' '+strTmp;
    }
  }
  else{
    strTmp = "<  >";
  }
  
  toWrite += strTmp;
  toWrite += ' ';
  lcd.writeAtPosition(toWrite,currRow,currCol);

  currLCDIndex++;
  if (currLCDIndex>=MAX_NUM_CHANNELS){
    currLCDIndex=0;
  }
  
}

