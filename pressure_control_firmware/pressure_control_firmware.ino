#include "allSettings.h"
#include "trajectory_control.h"
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
//#include "config/config_V_3_4.h"
//#include "config/config_hydraulic.h"
#include "config/nano_test.h"



//Create new settings objects
globalSettings settings;
UnitHandler units;
controlSettings ctrlSettings[MAX_NUM_CHANNELS];

//Create new trajectory objects
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



int ave_len=10;
float pressures[MAX_NUM_CHANNELS];
float masterPressure;


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

    
 
  //Initialize control settings
    handleCommands.initialize(MAX_NUM_CHANNELS, &settings, ctrlSettings, &trajCtrl, &units);
    handleCommands.startBroadcast();

    // Initialize master sensor
    #if(MASTER_SENSOR)
      settings.useMasterPressure=true;
      settings.masterPressureOutput=true;
    #else
      settings.useMasterPressure=false;
      settings.masterPressureOutput=false;
    #endif


    trajCtrl.initialize(MAX_NUM_CHANNELS);

    settings.looptime =0;
    settings.lcdLoopTime = 333;
    settings.outputsOn=false;

    units.setInputUnits(settings.units[0]);
    units.setOutputUnits(settings.units[1]);


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
  

  bool newSettings=handleCommands.go();


    #if(MASTER_SENSOR)
    // Read the master sensor
    masterPressure = 30.0;
    #endif
  
  
  //Get pressure readings and do control
    for (int i=0; i<MAX_NUM_CHANNELS; i++){   
      //Update controller settings if there are new ones

      //if we are in mode 2, set the setpoint to be a linear interpolation between trajectory points
        if (ctrlSettings[i].controlMode==2){
          if (run_traj){
            //Set setpoint
            setpoint_local[i] = sin(curr_time/1000);
          }
          if (traj_reset){
            setpoint_local[i] =0.0;
          }

        }
        else{
          if (newSettings){
                      
            if (ctrlSettings[i].controlMode==1){
              setpoint_local[i] = ctrlSettings[i].setpoint;
            }
          }

          if (ctrlSettings[i].controlMode==3) {
            setpoint_local[i] = sin(curr_time/1000);
          }
          

        }
      
        
        pressures[i] = setpoint_local[i];


        //Software Watchdog - If pressure exceeds max, vent forever until the mode gets reset.
        if (pressures[i] > ctrlSettings[i].maxPressure){
          ventUntilReset();
          
        }



    }

  //Print out data at close to the correct rate
  currentTime=millis();
  if (settings.outputsOn && (currentTime-previousTime>= settings.looptime)){
    printData();
    previousTime=currentTime;
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
  }
}
