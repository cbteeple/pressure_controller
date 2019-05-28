#include "analog_PressureSensor.h"
#include "i2c_PressureSensor.h"
#include "i2c_mux.h"
#include "handleSerialCommands.h"
#include "handleButtons.h"
#include "allSettings.h"
#include "valvePair.h"
#include "bangBang.h"
#include "proportional.h"
#include "pidFull.h"



#include <EasyLCD.h>



//Include the config file from the system you are using
#include "config/config_pneumatic_teensy.h"





//Create a new settings object
globalSettings settings;
controlSettings ctrlSettings[MAX_NUM_CHANNELS];
sensorSettings senseSettings[MAX_NUM_CHANNELS];

//Create an object to handle serial commands
handleSerialCommands handleCommands;
eepromHandler saveHandler;

Button  buttons[3] { {buttonPins[0]}, { buttonPins[1] }, { buttonPins[2] } };
handleButtons buttonHandler(MAX_NUM_CHANNELS);

trajectory traj;

i2c_Mux mux(muxAddr);

//Set up sensing
#if(SENSOR_ANALOG)
  int senseChannels[]={A0,A1,A2,A3,A4,A5,A6,A7};
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

    analogReadResolution(ADC_RES);


  //Buttons:
  /*
    for (int i=0; i<3; i++){
      pinMode(buttonPins[i], INPUT_PULLUP);
    }
    attachInterrupt(digitalPinToInterrupt(buttonPins[0]), buttons.up, CHANGE);
    attachInterrupt(digitalPinToInterrupt(buttonPins[1]), buttons.down, CHANGE);
    attachInterrupt(digitalPinToInterrupt(buttonPins[2]), buttons.enter, CHANGE);
    */
    
 
  //Initialize control settings
    for (int idx=0; idx<3; idx++){
      buttons[idx].begin();
    }
  
    buttonHandler.initialize();
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
        senseSettings[i].adc_res=ADC_RES;
        senseSettings[i].adc_max_volts=ADC_MAX_VOLTS;
      }
      else if(SENSOR_I2C){
        senseSettings[i].sensorAddr= sensorAddr;
        senseSettings[i].useMux     = false;
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
      Serial.println("_LCD Not Attached!");  
    }
    lcd.clearOnUpdate(false);
    lcd.fadeTime(250);


    //Get saved settings
    loadSettings();

}



bool runtraj = traj.running;
bool lcdOverride = false;
float setpoint_local[MAX_NUM_CHANNELS];

//______________________________________________________________________
void loop() {
  //Serial.println("_words need to be here (for some reason)");
  //Handle serial commands

  traj.CurrTime = millis();
  runtraj = traj.running;
  
  bool newSettings=handleCommands.go(settings, ctrlSettings,traj);
  String buttonMessage= buttonHandler.go(buttons,settings, ctrlSettings); 

  if (buttonMessage =="r"){
    lcd.clearOnUpdate(true);
    lcd.fadeOnUpdate(true);
    
    lcdMessage("");
    lcd.clearOnUpdate(false);
    lcd.fadeOnUpdate(false);
    
    lcdOverride = false;
  }
  else if (buttonMessage != ""){
    lcd.clearOnUpdate(true);
    lcdMessage(buttonMessage);
    lcd.clearOnUpdate(false);
    lcdOverride = true;
  }

  
  
  //Get pressure readings and do control
    for (int i=0; i<MAX_NUM_CHANNELS; i++){   
      //Update controller settings if there are new ones

      //if we are in mode 2, set the setpoint to be a linear interpolation between trajectory points
        if (ctrlSettings[i].controlMode==2){
          if (runtraj){
            //Set setpoint
            setpoint_local[i] = traj.interp(i);
            controllers[i].setSetpoint(setpoint_local[i]);
          }
        }
        else{
          if (newSettings){
<<<<<<< HEAD
          //NOT THIS!
=======
          setpoint_local[i] = ctrlSettings[i].setpoint;
>>>>>>> c5b72a4... Added nice top-level button interface, added setpoint output to serial
          controllers[i].updateSettings(ctrlSettings[i]);
          controllers[i].setSetpoint(setpoint_local[i]);
          }  
        }
      
        //Get the new pressures
        if (useMux){
          mux.setActiveChannel(senseChannels[i]);
        }

        //NOT THIS
        sensors[i].getData();
        pressures[i] = sensors[i].getPressure();


        //Software Watchdog - If pressure exceeds max, vent forever until the mode gets reset.
        if (pressures[i] > ctrlSettings[i].maxPressure){
          ventUntilReset();
          
        }

        //Perform control if the channel is on
        if (ctrlSettings[i].channelOn){
          
          //Run 1 step of the controller if we are in mode 1 or 2
          if (ctrlSettings[i].controlMode>=1){
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
    if (!lcdOverride){
      lcdUpdateDistributed();
      previousLCDTime=currentTime;
    }
  }
    
  
}


//______________________________________________________________________


//PRINT DATA OUT FUNCTION
void printData(){
  for (int i=0; i<MAX_NUM_CHANNELS; i++){
    Serial.print(setpoint_local[i],3);
    Serial.print('\t'); 
    Serial.print(pressures[i],3);
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

void lcdMessage(String message){
  lcd.write(message);
}






void loadSettings(){
  for (int i=0; i<MAX_NUM_CHANNELS; i++){
    float set_temp = ctrlSettings[i].setpoint;
    saveHandler.loadCtrl(ctrlSettings[i], i);
    ctrlSettings[i].setpoint=set_temp;
  }
  bool set_temp = settings.outputsOn;
  saveHandler.loadGlobal(settings);
  settings.outputsOn=set_temp;
}



void ventUntilReset(){
  for (int i=0; i<MAX_NUM_CHANNELS; i++){  
    ctrlSettings[i].controlMode = 0;
    ctrlSettings[i].valveDirect = -1.0;
    valves[i].go( ctrlSettings[i].valveDirect );
  }
  }
