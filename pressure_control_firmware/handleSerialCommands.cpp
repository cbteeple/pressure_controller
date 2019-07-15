#include <stdlib.h>
#include "Arduino.h"
#include "handleSerialCommands.h"
#include "allSettings.h"
#include "eeprom_handler.h"

//_________________________________________________________
//PUBLIC FUNCTIONS
bool handleSerialCommands::go(globalSettings (&settings), controlSettings *ctrlSettings, trajectory (&traj)) {
  bool newCommand = getCommandByChar(); //getCommand();
  bool newSettings = false;
  if (newCommand) {
    newSettings = processCommand(settings, ctrlSettings, traj);
    command = "";
  }
  return newSettings;
}

void handleSerialCommands::startBroadcast() {
  broadcast = true;
}

void handleSerialCommands::stopBroadcast() {
  broadcast = false;
}


void handleSerialCommands::initialize(int num) {
  numSensors = num;
  // reserve 200 bytes for the inputString:
  command.reserve(200);
}


//_________________________________________________________
//PRIVATE FUNCTIONS

//METHOD DEPRICATED
bool handleSerialCommands::getCommand() {
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.toUpperCase();
    return true;
  }
  else {
    return false;
  }
}


bool handleSerialCommands::getCommandByChar() {
  //unsigned long start_time = micros();
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // Add new byte to the inputString:
    command += inChar;
    // If the incoming character is a newline, set a flag so we can process it
    if (inChar == '\n') {
      command.toUpperCase();
      //Serial.print("_SER: Line Complete");
      //Serial.print('\t');
      //Serial.println(micros() - start_time);
      return true;
    }
  }
  return false;
}




bool handleSerialCommands::processCommand(globalSettings (&settings), controlSettings *ctrlSettings, trajectory (&traj)) {
  bool newSettings = false;
  String bc_string = "_";

  //______________________________________________________________
  //Handle changes in setpoint first so it's a fast operation
  if (command.startsWith("SET")) {
    if (getStringValue(command, ';', numSensors + 1).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].settime = constrain(getStringValue(command, ';', 1).toFloat(), 0, 1000);
        ctrlSettings[i].setpoint = constrain(getStringValue(command, ';', i + 2).toFloat(),
                                             ctrlSettings[i].minPressure,
                                             ctrlSettings[i].maxPressure);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 2).length()) {
      float allset = getStringValue(command, ';', 2).toFloat();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].settime = constrain(getStringValue(command, ';', 1).toFloat(), 0, 1000);
        ctrlSettings[i].setpoint = constrain(allset,
                                             ctrlSettings[i].minPressure,
                                             ctrlSettings[i].maxPressure);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += "SET: ";
      bc_string += String(ctrlSettings[0].settime, 4);
      bc_string += '\t';
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].setpoint, 4);
        bc_string += '\t';
      }
    }
  }



  else if (command.startsWith("TRAJSTART")) {
    if (broadcast) {
      bc_string += ("TRAJSTART: Trajectory Started");
    }
    traj.start();
  }

  else if (command.startsWith("TRAJSTOP")) {
    if (broadcast) {
      bc_string += ("TRAJSTOP: Trajectory Stopped");
    }
    traj.stop();
  }

   else if (command.startsWith("TRAJPAUSE")) {
    if (broadcast) {
      bc_string += ("TRAJPAUSE: Trajectory Paused");
    }
    traj.pause();
  }

  else if (command.startsWith("TRAJRESUME")) {
    if (broadcast) {
      bc_string += ("TRAJRESUME: Trajectory Resumed");
    }
    traj.resume();
  }



  else if (command.startsWith("OFF")) {
    settings.outputsOn = false;
    if (broadcast) {
      bc_string += "OFF: Outputs Off";
    }
  }
  else if (command.startsWith("ON")) {
    settings.outputsOn = true;
    if (broadcast) {
      bc_string += "ON: Outputs On";
    }
  }
  else if (command.startsWith("TIME")) {
    if (getStringValue(command, ';', 1).length()) {
      settings.looptime = getStringValue(command, ';', 1).toInt();
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += "TIME: ";
      bc_string += String(settings.looptime);
    }
  }

  else if (command.startsWith("LCDTIME")) {
    if (getStringValue(command, ';', 1).length()) {
      settings.lcdLoopTime = getStringValue(command, ';', 1).toInt();
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += "LCDTIME: ";
      bc_string += String(settings.lcdLoopTime);
    }
  }




  //____________________________________________________________
  //Handle MAXIMUM Software Pressure Limits
  else if (command.startsWith("MAXP")) {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].maxPressure = getStringValue(command, ';', i + 1).toFloat();
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      float allset = getStringValue(command, ';', 1).toFloat();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].maxPressure = allset;
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += ("MAXP: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].maxPressure, 4);
        bc_string += ('\t');
      }
    }
  }
  //____________________________________________________________
  //Handle MINIMUM Software Pressure Limits
  else if (command.startsWith("MINP")) {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].minPressure = getStringValue(command, ';', i + 1).toFloat();
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      float allset = getStringValue(command, ';', 1).toFloat();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].minPressure = allset;
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += ("MINP: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].minPressure, 4);
        bc_string += ('\t');
      }
    }
  }


  else if (command.startsWith("VALVE")) {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].valveDirect = getStringValue(command, ';', i + 1).toFloat();
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      float allset = getStringValue(command, ';', 1).toFloat();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].valveDirect = allset;
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += ("VALVE: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].valveDirect, 4);
        bc_string += ('\t');
      }
    }
  }



  else if (command.startsWith("WINDOW")) {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].deadzone = getStringValue(command, ';', i + 1).toFloat();
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }

    }
    if (broadcast) {
      bc_string += ("WINDOW: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].deadzone, 4);
        bc_string += ('\t');
      }
    }

  }
  else if (command.startsWith("PID")) {
    int channel = 0;
    if (getStringValue(command, ';', 4).length()) {
      channel = getStringValue(command, ';', 1).toInt();
      for (int i = 0; i < 3; i++) {
        ctrlSettings[channel].pidGains[i] = getStringValue(command, ';', i + 2).toFloat();
      }
      newSettings = true;
      if (broadcast) {
        bc_string += ("NEW PID: Chan= ");
        bc_string += String(channel);
      }

    }
    else {
      if (broadcast) {
        bc_string += ("PID");
        bc_string += ('\n');
        for (int j = 0; j < numSensors; j++) {
          bc_string += ("_ Gains= ");
          for (int i = 0; i < 3; i++) {
            bc_string += String(ctrlSettings[j].pidGains[i], 4);
            bc_string += ('\t');
          }
          bc_string += ('\n');
        }
      }
    }
  }
  else if (command.startsWith("MODE")) {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].controlMode = getStringValue(command, ';', i + 1).toInt();
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      int allset = getStringValue(command, ';', 1).toInt();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].controlMode = allset;
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += ("MODE: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].controlMode, 4);
        bc_string += ('\t');
      }
    }
  }


  else if (command.startsWith("SAVE")) {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.saveCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveGlobal(settings);

    if (broadcast) {
      bc_string += ("SAVE: Settings saved to onboard storage");
    }
  }


  else if (command.startsWith("LOAD")) {
    for (int i = 0; i < numSensors; i++) {
      float set_temp = ctrlSettings[i].setpoint;
      saveHandler.loadCtrl(ctrlSettings[i], i);
      ctrlSettings[i].setpoint = set_temp;
    }
    bool set_temp = settings.outputsOn;
    saveHandler.loadGlobal(settings);
    settings.outputsOn = set_temp;
    newSettings = true;

    if (broadcast) {
      bc_string += ("LOAD: Settings retrieved from onboard storage");
    }
  }


  else if (command.startsWith("DEFSAVE")) {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.saveDefaultCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveDefaultGlobal(settings);
    if (broadcast) {
      bc_string += ("DEFSAVE: Settings saved as default");
    }
  }


  else if (command.startsWith("DEFLOAD")) {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.loadDefaultCtrl(ctrlSettings[i], i);
    }
    bool set_temp = settings.outputsOn;
    saveHandler.loadDefaultGlobal(settings);
    settings.outputsOn = set_temp;
    newSettings = true;
    if (broadcast) {
      bc_string += ("DEFLOAD: Settings retrieved from default");
    }
  }


  else if (command.startsWith("CHAN")) {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].channelOn = bool(getStringValue(command, ';', i + 1).toInt());
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      float allset = bool(getStringValue(command, ';', 1).toInt());
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].channelOn = allset;
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }

    if (broadcast) {
      bc_string += ("CHAN: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(ctrlSettings[i].channelOn);
        bc_string += ('\t');
      }
    }



  }

  //
  //[trajectory length] [trajectory starting index] [wrap mode]
  else if (command.startsWith("TRAJCONFIG")) {
    if (getStringValue(command, ';', 3).length()) {
      traj.start_idx = constrain(getStringValue(command, ';', 1).toInt(), 0, 999);
      traj.len = constrain(getStringValue(command, ';', 2).toInt(), 1, 1000);
      traj.wrap = bool(getStringValue(command, ';', 3).toInt());
    }
    if (broadcast) {
      bc_string += ("TRAJCONFIG: start = ");
      bc_string += String(traj.start_idx);
      bc_string += ('\t');
      bc_string += ("len = ");
      bc_string += String(traj.len);
      bc_string += ('\t');
      bc_string += ("wrap = ");
      bc_string += String(traj.wrap);
    }
  }

  //[index];[time];[set0];[set1];[set2];[set3]
  else if (command.startsWith("TRAJSET")) {
    if (getStringValue(command, ';', numSensors + 2).length()) {

      float row[numSensors + 2];

      for (int i = 0; i < numSensors + 2; i++) {
        row[i] = getStringValue(command, ';', i + 1).toFloat();
      }

      traj.setLine(row);

      if (broadcast) {
        bc_string += ("TRAJSET: ");
        for (int i = 0; i < numSensors + 2; i++) {
          bc_string += ('\t');
          bc_string += String(row[i], 2);
        }
      }


    }

    else {
      if (broadcast) {
        bc_string += ("TRAJSET:");
        for (int i = traj.start_idx; i < (traj.start_idx + traj.len); i++) {
          bc_string += ('\n');
          bc_string += String(traj.trajtimes[i]);
          bc_string += ('\t');
          for (int j = 0; j < numSensors; j++) {
            bc_string += String(traj.trajpts[i][j]);
            bc_string += ('\t');
          }
        }
      }

    }


  }




  else if (command.startsWith("ECHO")) {
    
    if (getStringValue(command, ';', 1).length()) {
      broadcast = bool(getStringValue(command, ';', 1).toInt());
    }


    if (broadcast) {
      bc_string += ("ECHO: ON");
    }
    else {
      Serial.println("_ECHO: OFF");
    }
  }


  //Unrecognized
  else {
    newSettings = false;
    if (broadcast) {
      bc_string += ("UNREC: Unrecognized Command");
    }
  }


  //End the line with a newline
  if (broadcast) {
    Serial.println(bc_string);
  }

  return newSettings;
}




String handleSerialCommands::getStringValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
