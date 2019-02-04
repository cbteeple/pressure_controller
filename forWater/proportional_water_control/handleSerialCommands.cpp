#include <stdlib.h>
#include "Arduino.h"
#include "handleSerialCommands.h"
#include "allSettings.h"
#include "eeprom_handler.h"

//_________________________________________________________
//PUBLIC FUNCTIONS
bool handleSerialCommands::go(globalSettings (&settings), controlSettings *ctrlSettings, trajectory (&traj)){
  bool newCommand = getCommand();
  bool newSettings = false;
  if (newCommand){
    newSettings=processCommand(settings, ctrlSettings, traj);
  }
  return newSettings;
}

void handleSerialCommands::startBroadcast(){
  broadcast=true;
}

void handleSerialCommands::stopBroadcast(){
  broadcast=false;
}


void handleSerialCommands::initialize(int num){
  numSensors=num;
}


//_________________________________________________________
//PRIVATE FUNCTIONS

bool handleSerialCommands::getCommand(){
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.toUpperCase();
    return true;
  }
  else{
    return false;
  }
}




bool handleSerialCommands::processCommand(globalSettings (&settings), controlSettings *ctrlSettings, trajectory (&traj)){
  bool newSettings=false;
  if (broadcast){
    Serial.print("_");
  }
 
  //______________________________________________________________
  //Handle changes in setpoint first so it's a fast operation
  if(command.startsWith("SET")){
    if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].setpoint= constrain(getStringValue(command,';',i+1).toFloat(),
        ctrlSettings[i].minPressure,
        ctrlSettings[i].maxPressure);
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    else if(getStringValue(command,';',1).length()){
      float allset=getStringValue(command,';',1).toFloat();
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].setpoint= constrain(allset,
        ctrlSettings[i].minPressure,
        ctrlSettings[i].maxPressure);
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("SETPOINT: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].setpoint,4);
        Serial.print('\t');
      }
    }
  }




  else if(command.startsWith("OFF")){
    settings.outputsOn = false;
    if (broadcast){
      Serial.print("Output: OFF");
    }
  }
  else if(command.startsWith("ON")){ 
    settings.outputsOn = true;
    if (broadcast){
      Serial.print("Output: ON");
    }
  }
  else if (command.startsWith("TIME")){
    if(getStringValue(command,';',1).length()){
      settings.looptime = getStringValue(command,';',1).toInt();
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("Loop Time: ");
      Serial.print(settings.looptime);
    }
  }

  else if (command.startsWith("LCDTIME")){
    if(getStringValue(command,';',1).length()){
      settings.lcdLoopTime = getStringValue(command,';',1).toInt();
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("LCD Loop Time: ");
      Serial.print(settings.lcdLoopTime);
    }
  }



  
//____________________________________________________________
//Handle MAXIMUM Software Pressure Limits
  else if(command.startsWith("MAXP")){
    if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].maxPressure= getStringValue(command,';',i+1).toFloat();
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    else if(getStringValue(command,';',1).length()){
      float allset=getStringValue(command,';',1).toFloat();
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].maxPressure= allset;
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("MAX PRESSURE: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].maxPressure,4);
        Serial.print('\t');
      }
    }
  }
  //____________________________________________________________
//Handle MINIMUM Software Pressure Limits
  else if(command.startsWith("MINP")){
    if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].minPressure= getStringValue(command,';',i+1).toFloat();
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    else if(getStringValue(command,';',1).length()){
      float allset=getStringValue(command,';',1).toFloat();
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].minPressure= allset;
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("MIN PRESSURE: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].minPressure,4);
        Serial.print('\t');
      }
    }
  }

  
    else if(command.startsWith("VALVE")){
    if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].valveDirect= getStringValue(command,';',i+1).toFloat();
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    else if(getStringValue(command,';',1).length()){
      float allset=getStringValue(command,';',1).toFloat();
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].valveDirect= allset;
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("VALVE DIRECT SETTING: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].valveDirect,4);
        Serial.print('\t');
      }
    }
  }


  
  else if(command.startsWith("WINDOW")){
     if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].deadzone=getStringValue(command,';',i+1).toFloat();
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
     
     }
    if (broadcast){
      Serial.print("WINDOW: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].deadzone,4);
        Serial.print('\t');
      }
    }
     
  }
  else if(command.startsWith("PID")){
    int channel=0;
     if(getStringValue(command,';',4).length()){
      channel = getStringValue(command,';',1).toInt();
      for (int i=0; i<3; i++){
        ctrlSettings[channel].pidGains[i]=getStringValue(command,';',i+2).toFloat();
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW PID: Chan= ");
        Serial.print(channel);
        Serial.print('\n');
      }
     
     }
    if (broadcast){
      Serial.print("PID");
      Serial.print('\n');
      for (int j=0; j<numSensors; j++){
        Serial.print("_ Gains= ");
        for (int i=0; i<3; i++){
          Serial.print(ctrlSettings[j].pidGains[i],4);
          Serial.print('\t');
        }
        Serial.print('\n');
      }
    }
     
  }
 else if(command.startsWith("MODE")){
    if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].controlMode= getStringValue(command,';',i+1).toInt();
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    else if(getStringValue(command,';',1).length()){
      int allset=getStringValue(command,';',1).toInt();
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].controlMode= allset;
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    if (broadcast){
      Serial.print("MODE: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].controlMode,4);
        Serial.print('\t');
      }
    }
  }


  else if(command.startsWith("SAVE")){
    for (int i=0; i<numSensors; i++){
      saveHandler.saveCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveGlobal(settings);

    if (broadcast){
      Serial.print("Settings saved to onboard storage");
    }
  }


  else if(command.startsWith("LOAD")){
    for (int i=0; i<numSensors; i++){
      float set_temp = ctrlSettings[i].setpoint;
      saveHandler.loadCtrl(ctrlSettings[i], i);
      ctrlSettings[i].setpoint=set_temp;
    }
    bool set_temp = settings.outputsOn;
    saveHandler.loadGlobal(settings);
    settings.outputsOn=set_temp;
    newSettings=true;

    if (broadcast){
      Serial.print("Settings retrieved from onboard storage");
    }
  }


  else if(command.startsWith("DEFSAVE")){
    for (int i=0; i<numSensors; i++){
      saveHandler.saveDefaultCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveDefaultGlobal(settings);
    if (broadcast){
      Serial.print("Settings saved as default");
    }
  }

  
  else if(command.startsWith("DEFLOAD")){
    for (int i=0; i<numSensors; i++){
      saveHandler.loadDefaultCtrl(ctrlSettings[i], i);
    }
    bool set_temp = settings.outputsOn;
    saveHandler.loadDefaultGlobal(settings);
    settings.outputsOn=set_temp;
    newSettings=true;
    if (broadcast){
      Serial.print("Settings retrieved from default");
    }
  }


  else if(command.startsWith("CHAN")){
    if(getStringValue(command,';',numSensors).length()){
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].channelOn= bool(getStringValue(command,';',i+1).toInt());
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }
    else if(getStringValue(command,';',1).length()){
      float allset=bool(getStringValue(command,';',1).toInt());
      for (int i=0; i<numSensors; i++){
        ctrlSettings[i].channelOn= allset;
      }
      newSettings=true;
      if (broadcast){
        Serial.print("NEW ");
      }
    }

    if (broadcast){
      Serial.print("CHANNELS ON: ");
      for (int i=0; i<numSensors; i++){
        Serial.print(ctrlSettings[i].channelOn);
        Serial.print('\t');
      }
    }


    
  }

  else if(command.startsWith("ECHO")){
    bool broadcast_tmp=broadcast;
    
    if(getStringValue(command,';',1).length()){
      broadcast = bool(getStringValue(command,';',1).toInt());
    }

    if(!broadcast_tmp){
      Serial.print("_");
    }
    
    if(broadcast){
      Serial.print("ECHO: ON");
    }
    else{
      Serial.print("ECHO: OFF");
    }
    if (!broadcast){
      Serial.print('\n');
    }
  }

  //
  //[trajectory length] [trajectory starting index] [wrap mode]
  else if (command.startsWith("TRAJCONFIG")){
    if(getStringValue(command,';',3).length()){
      traj.start_idx = constrain(getStringValue(command,';',1).toInt(),0,199);
      traj.len = constrain(getStringValue(command,';',2).toInt(),1,200);
      traj.wrap = bool(getStringValue(command,';',3).toInt());
    }
    if (broadcast){
      Serial.print("TRAJ CONFIG: start = ");
      Serial.print(traj.start_idx);
      Serial.print('\t');
      Serial.print("len = ");
      Serial.print(traj.len);
      Serial.print('\t');
      Serial.print("wrap = ");
      Serial.print(traj.wrap);
    }
  }

  //[index];[time];[set0];[set1];[set2];[set3]
  else if(command.startsWith("TRAJSET")){
    if(getStringValue(command,';',numSensors+2).length()){
      
      float row[6]={0,0,0,0,0,0};
      
      for (int i=0; i<numSensors+1; i++){
        row[i]= getStringValue(command,';',i+1).toFloat();
      }
      
    traj.setLine(row);

    if (broadcast){
      Serial.print("TRAJSET: ");
      for(int i=0;i<6;i++){
        Serial.print('\t');
        Serial.print(row[i],2);
      }
    }
    
    
    }

    else{
      if (broadcast){
        Serial.print("TRAJ:");
        for (int i=traj.start_idx; i<(traj.start_idx+traj.len); i++){
            Serial.print('\n');
            Serial.print(traj.trajtimes[i]);
            Serial.print('\t');
          for (int j=0; j<4; j++){
            Serial.print(traj.trajpts[i][j]);
            Serial.print('\t');
          }
        }
      }
      
    }

    
  }

  else if(command.startsWith("TRAJSTART")){
    traj.start();
  }
  
  else if(command.startsWith("TRAJSTOP")){
    traj.stop();
  }



//Unrecognized
  else {
    newSettings=false;
    if (broadcast){
      Serial.print("Unrecognized Command");
    }
  }


//End the line with a newline
  if (broadcast){
    Serial.print("\n");
  }

  return newSettings;
}




String handleSerialCommands::getStringValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


