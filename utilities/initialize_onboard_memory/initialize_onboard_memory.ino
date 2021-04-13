#include "allSettings.h"
#include "eeprom_handler.h"

#define MAX_NUM_CHANNELS 10

//Create new settings objects
globalSettings settings;
controlSettings ctrlSettings[MAX_NUM_CHANNELS];
valveSettings  valvePairSettings[MAX_NUM_CHANNELS];

eepromHandler saveHandler;


float pid_start[]={0.1,0.001,0}; 
float deadzone_start=0.0;
float setpoint_start=0;
float integratorResetTime_start = -1;
float minPressure_start = 0; //[psi]
float maxPressure_start = 28; //[psi]


//______________________________________________________________________
void setup() {
  //Start serial
    Serial.begin(115200);
    //Serial.flush();
    Serial.setTimeout(2);


    for (int i=0; i<MAX_NUM_CHANNELS; i++){
      
      //Initialize control settings
      ctrlSettings[i].setpoint=setpoint_start;
      ctrlSettings[i].maxPressure = maxPressure_start;
      ctrlSettings[i].minPressure = minPressure_start;
      ctrlSettings[i].controlMode = 3;
      ctrlSettings[i].valveDirect = 0;
      
      for (int j=0; j<3; j++){
        ctrlSettings[i].pidGains[j]=pid_start[j];
      }
      ctrlSettings[i].deadzone=deadzone_start;
      ctrlSettings[i].integratorResetTime=integratorResetTime_start;

    }

    settings.looptime =20;
    settings.lcdLoopTime = 333;
    settings.outputsOn=false;

    //Save the settings to EEPROM
    saveSettings();

}




//______________________________________________________________________
void loop() {
    
}


//______________________________________________________________________





void saveSettings() {
  for (int i = 0; i < MAX_NUM_CHANNELS; i++) {
    saveHandler.saveCtrl(ctrlSettings[i], i);
    saveHandler.saveValves(valvePairSettings[i], i);
  }
  saveHandler.saveGlobal(settings);
  Serial.println("Onboard Memory Initialized");
}
