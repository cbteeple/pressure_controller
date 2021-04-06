#include "analog_PressureSensor.h"
#include "i2c_PressureSensor.h"
#include "i2c_mux.h"
#include "handleButtons.h"
#include "allSettings.h"
#include "valvePair.h"
#include "bangBang.h"
#include "proportional.h"
#include "pidFull.h"
#include "interp_lin.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include <EasyLCD.h>
#include "command_handler.h"
#include "unit_handler.h"


//Include the config file from the system you are using
//#include "config/config_pneumatic_teensy.h"
//#include "config/config_pneumatic_teensy8.h"
//#include "config/config_pneumatic_teensy7.h"
//#include "config/config_vacuum.h"
//#include "config/config_V_3_4_no_master.h"
//#include "config/config_V_3_4_fivechannel.h"
//#include "config/config_V_3_4_microprop.h"
#include "config/config_V_3_4.h"
//#include "config/config_hydraulic.h"



//Create new settings objects
globalSettings settings;
UnitHandler units;
controlSettings ctrlSettings[MAX_NUM_CHANNELS];
sensorSettings senseSettings[MAX_NUM_CHANNELS];

#if(MASTER_SENSOR)
  sensorSettings masterSenseSettings;
#endif

valveSettings  valvePairSettings[MAX_NUM_CHANNELS];

//Create new trajectory objects
Trajectory traj[MAX_NUM_CHANNELS];
TrajectoryControl trajCtrl;

//Create an object to handle serial commands

CommandHandler handleCommands;
  
#ifdef USB_RAWHID
  #define VENDOR_ID               0x16C0
  #define PRODUCT_ID              0x0486
  #define RAWHID_USAGE_PAGE       0xFFAB  // recommended: 0xFF00 to 0xFFFF
  #define RAWHID_USAGE            0x0200  // recommended: 0x0100 to 0xFFFF

#endif
//#ifndef USB_RAWHID
 // #include "handleSerialCommands.h"
 // handleSerialCommands handleCommands;
//#endif

eepromHandler saveHandler;

Button  buttons[3] { {buttonPins[0]}, { buttonPins[1] }, { buttonPins[2] } };
handleButtons buttonHandler(MAX_NUM_CHANNELS);



i2c_Mux mux(muxAddr);

//Set up sensing
#if(SENSOR_ANALOG)
  analog_PressureSensor sensors[MAX_NUM_CHANNELS];
#elif(SENSOR_I2C)
  int senseChannels[]={0,1,2,3,4,5,6,7};
  i2c_PressureSensor sensors[MAX_NUM_CHANNELS];
#endif

#if(MASTER_SENSOR)
analog_PressureSensor masterSensor;
#endif


int ave_len=10;
float pressures[MAX_NUM_CHANNELS];
float masterPressure;
float valveSets[MAX_NUM_CHANNELS];


//Set up valve pairs  
valvePair valves[MAX_NUM_CHANNELS];

interpLin setpoint_interp[MAX_NUM_CHANNELS];


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
    Serial.setTimeout(2);

   int adc_res = setADCRes(ADC_RES);


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
    handleCommands.initialize(MAX_NUM_CHANNELS, &settings, ctrlSettings, traj, &trajCtrl, &units);
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
      senseSettings[i].sensorModel=controlSensorType.model;

      if(SENSOR_ANALOG){
        senseSettings[i].sensorPin=senseChannels[i];
        senseSettings[i].adc_res=adc_res;
        senseSettings[i].adc_max_volts=ADC_MAX_VOLTS;
        senseSettings[i].adc_mult = ADC_MULT;
        senseSettings[i].output_min=controlSensorType.output_min;
        senseSettings[i].output_max=controlSensorType.output_max;
        senseSettings[i].output_offset=controlSensorType.output_offset;
        senseSettings[i].pressure_min=controlSensorType.pressure_min;
        senseSettings[i].pressure_max=controlSensorType.pressure_max;

      }
      else if(SENSOR_I2C){
        senseSettings[i].sensorAddr= sensorAddr;
        senseSettings[i].useMux     = false;
        senseSettings[i].muxAddr    = muxAddr;
        senseSettings[i].muxChannel = senseChannels[i];
      }

      //Inbitialize valve settings
      for (int j=0; j<2; j++){
        valvePairSettings[i].valveOffset[j] = valveOffset[i][j];
      
      }
    }

    // Initialize master sensor
    #if(MASTER_SENSOR)
      masterSenseSettings.sensorModel=masterSensorType.model;
      masterSenseSettings.sensorPin=masterSenseChannel;
      masterSenseSettings.adc_res=adc_res;
      masterSenseSettings.adc_max_volts=ADC_MAX_VOLTS;
      masterSenseSettings.adc_mult = ADC_MULT;
      masterSenseSettings.output_min=masterSensorType.output_min;
      masterSenseSettings.output_max=masterSensorType.output_max;
      masterSenseSettings.output_offset=masterSensorType.output_offset;
      masterSenseSettings.pressure_min=masterSensorType.pressure_min;
      masterSenseSettings.pressure_max=masterSensorType.pressure_max;

      settings.useMasterPressure=true;
      settings.masterPressureOutput=true;
    #else
      settings.useMasterPressure=false;
      settings.masterPressureOutput=false;
    #endif


  //Initialize the pressure sensor and control objects
    for (int i=0; i<MAX_NUM_CHANNELS; i++){
      sensors[i].initialize(senseSettings[i]);
      valves[i].initialize(valvePins[i][0],valvePins[i][1]);
      valves[i].setSettings(valvePairSettings[i]);
      controllers[i].initialize(ctrlSettings[i]);
    }

    #if(MASTER_SENSOR)
      masterSensor.initialize(masterSenseSettings);
    #endif

    trajCtrl.initialize(traj, MAX_NUM_CHANNELS);

    settings.looptime =0;
    settings.lcdLoopTime = 333;
    settings.outputsOn=false;

    units.setInputUnits(settings.units[0]);
    units.setOutputUnits(settings.units[1]);

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









bool lcdOverride = false;
float setpoint_local[MAX_NUM_CHANNELS];

bool run_traj = false;
bool traj_reset = false;

unsigned long curr_time=0;

bool firstcall = true;

unsigned long watchdog_start_time = 0;
bool watchdog_triggered = false;

//______________________________________________________________________
void loop() {
  //Serial.println("_words need to be here (for some reason)");
  //Handle serial commands
  curr_time = micros();


  run_traj = trajCtrl.all_running;
  traj_reset = trajCtrl.reset;
  trajCtrl.CurrTime = curr_time;

  settings.currentTime = currentTime;
  

  
  
  //bool newSettings=handleCommands.go(settings, ctrlSettings, traj, trajCtrl );
  
  bool newSettings=handleCommands.go();
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

    #if(MASTER_SENSOR)
    // Read the master sensor
      if (masterSensor.connected){
        masterSensor.getData();
        masterPressure = masterSensor.getPressure();
      }
    #endif
  
  
  //Get pressure readings and do control
    for (int i=0; i<MAX_NUM_CHANNELS; i++){   
      //Update controller settings if there are new ones

      //if we are in mode 2, set the setpoint to be a linear interpolation between trajectory points
        if (ctrlSettings[i].controlMode==2){
          if (run_traj){
            //Set setpoint
            setpoint_local[i] = trajCtrl.interp(curr_time,i);
            controllers[i].setSetpoint(setpoint_local[i]);
          }
          if (traj_reset){
            setpoint_local[i] =0.0;
            controllers[i].setSetpoint(setpoint_local[i]);
          }

        }
        else{
          if (newSettings){
            controllers[i].updateSettings(ctrlSettings[i]);
          
            if (ctrlSettings[i].controlMode==1){
              setpoint_local[i] = ctrlSettings[i].setpoint;
              controllers[i].setSetpoint(setpoint_local[i]);
            }
            else if (ctrlSettings[i].controlMode==3){
              setpoint_interp[i].newGoal(ctrlSettings[i]);
            }
          }

          if (ctrlSettings[i].controlMode==3) {
            setpoint_interp[i].CurrTime = curr_time;
            setpoint_local[i] = setpoint_interp[i].go();
            controllers[i].setSetpoint(setpoint_local[i]);
          }
          

        }
      
        //Get the new pressures
        if (useMux){
          mux.setActiveChannel(senseChannels[i]);
        }

        if (sensors[i].connected){
          sensors[i].getData();
          pressures[i] = sensors[i].getPressure();
        }


        //Software Watchdog - If pressure exceeds max, vent forever until the mode gets reset.
        if (pressures[i] > ctrlSettings[i].maxPressure){
          ventUntilReset();
          
        }

        //Software Watchdog on input pressure line
        #if(MASTER_SENSOR)
          if (settings.useMasterPressure){
            if (masterPressure > settings.maxPressure){
              if (!watchdog_triggered){
                watchdog_triggered=true;
                watchdog_start_time = curr_time;
              }
              else{
                if ((watchdog_start_time-curr_time)>=settings.watchdogSpikeTime){
                  watchdog_triggered=false;
                  ventUntilReset();
                  watchdog_start_time = 0;
                }
             }         
            }
            else{
              watchdog_start_time=0;
              watchdog_triggered=false;
            }
          }
        #endif

        //Perform control if the channel is on
        if (ctrlSettings[i].channelOn){
          
          //Run 1 step of the controller if we are not in mode 0
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
        valves[i].go(0);
        //pressures[i]=0;
      }
    }

  if (ctrlSettings[0].controlMode==2){
    trajCtrl.setUpNext();
  }

  //Print out data at close to the correct rate
  currentTime=millis();
  if (settings.outputsOn && (currentTime-previousTime>= settings.looptime)){
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

  if(trajCtrl.current_message.length()){
    printMessage(trajCtrl.current_message);
  }
    
}


//______________________________________________________________________


//PRINT DATA OUT FUNCTION
#ifdef USB_RAWHID
  void printData(){
    handleCommands.sendString(generateSetpointStr());
    handleCommands.sendString(generateDataStr());

    if (settings.masterPressureOutput){
      handleCommands.sendString(generateMasterStr());
    }
  }

  void printMessage(String message){
    handleCommands.sendString(message);
  }

  

#else
  void printData(){
    Serial.println(generateSetpointStr());
    Serial.println(generateDataStr());
    if (settings.masterPressureOutput){
      Serial.println(generateMasterStr());
    }
  }

  void printMessage(String message){
    Serial.print(message);
  }
#endif


String generateSetpointStr(){
  String send_str = "";
  send_str+=String(currentTime);
  send_str+=('\t');
  send_str+="0";
  for (int i=0; i<MAX_NUM_CHANNELS; i++){
    send_str+=('\t'); 
    send_str+=String( units.convertToExternal(setpoint_local[i]),2);
  }
  return send_str;
}


String generateDataStr(){
  String send_str = "";
  send_str+=String(currentTime);
  send_str+=('\t');
  send_str+="1";
  for (int i=0; i<MAX_NUM_CHANNELS; i++){
    send_str+=('\t'); 
    send_str+=String(units.convertToExternal(pressures[i]),2);  
  }
  return send_str;
}

String generateMasterStr(){
  String send_str = "";
  
  send_str+=String(currentTime);
  send_str+=('\t');
  send_str+="2";
  send_str+=('\t');  
  send_str+=String(units.convertToExternal(masterPressure),2); 
  return send_str;
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
      strTmp = String(units.convertToExternal(pressures[i]), 1);
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
    strTmp = String(units.convertToExternal(pressures[i]), 1);
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
  units.setInputUnits(settings.units[0]);
  units.setOutputUnits(settings.units[1]);
}



void ventUntilReset(){
  for (int i=0; i<MAX_NUM_CHANNELS; i++){  
    ctrlSettings[i].controlMode = 0;
    ctrlSettings[i].valveDirect = -1.0;
    valves[i].go( ctrlSettings[i].valveDirect );
  }
}



#ifdef TEENSYDUINO
int setADCRes(int adc_res){
  analogReadResolution(adc_res);
  return adc_res;
}

#else
int setADCRes(int adc_res){
  // Set back to the default
  adc_res = 10;
  return adc_res;
}

#endif
