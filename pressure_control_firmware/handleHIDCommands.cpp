#include <stdlib.h>
#include "Arduino.h"
#include "handleHIDCommands.h"
#include "allSettings.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include "eeprom_handler.h"

//_________________________________________________________
//PUBLIC FUNCTIONS
/*
bool handleHIDCommands::go(globalSettings (&settings), controlSettings *ctrlSettings, Trajectory *traj, TrajectoryControl (&trajCtrl)) {
  bool newCommand = readCommand(); //readCommand();
  bool newSettings = false;
  if (newCommand) {
    newSettings = processCommand(settings, ctrlSettings, traj, trajCtrl);
    command = "";
  }
  return newSettings;
}
*/

bool handleHIDCommands::go() {
  bool newCommand = readCommand(); //readCommand();
  bool newSettings = false;
  if (newCommand) {
    newSettings = processCommand();
    command = "";
  }
  return newSettings;
}

void handleHIDCommands::startBroadcast() {
  broadcast = true;
}

void handleHIDCommands::stopBroadcast() {
  broadcast = false;
}


void handleHIDCommands::initialize(int num, globalSettings (&settings_in), controlSettings *ctrlSettings_in, Trajectory *traj_in, TrajectoryControl (&trajCtrl_in)) {
  numSensors   = num;
  settings     = settings_in;
  ctrlSettings = ctrlSettings_in;
  traj         = traj_in;
  trajCtrl     = trajCtrl_in;
  // reserve 200 bytes for the inputString:
  command.reserve(200);

}


//_________________________________________________________
//PRIVATE FUNCTIONS

//METHOD DEPRICATED
#ifdef USB_RAWHID
bool handleHIDCommands::readCommand() {

  int n = RawHID.recv(in_buffer, 1); // 0 timeout = do not wait
  if (n > 0) {

    String in_str;
    for (char c : in_buffer) in_str += c;
    
    command = in_str;
    command.toUpperCase();
    //Serial.print("Recieved: ");
    //Serial.println(command);
    return true;
  }
  else {
    return false;
  }

}



void handleHIDCommands::sendString(String bc_string){
  // Reset the buffer with zeros
  for (int i=0; i<64; i++) {
    out_buffer[i] = 0;
  }
  bc_string.getBytes(out_buffer, 64);
  int n = RawHID.send(out_buffer, 1);

}

#endif




//bool handleHIDCommands::processCommand(globalSettings (&settings), controlSettings *ctrlSettings, Trajectory *traj, TrajectoryControl (&trajCtrl)) {
bool handleHIDCommands::processCommand() {
  newSettings = false;
  bc_string = "_";

  String cmdStr = getStringValue(command, ';', 0);
  auto work_fun = findFunction(cmdStr);
  (this->*work_fun)();


  //End the line with a newline
  if (broadcast) {
    sendString(bc_string);
    //Serial.println(bc_string);
  }

  return newSettings;
}




String handleHIDCommands::getStringValue(String data, char separator, int index)
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







handleHIDCommands::FunctionPointer handleHIDCommands::findFunction(String str_in){
  for (int i=0; i<idx;i++){
    if (str_vec[i] == str_in){
      return fun_vec[i];
    }
  }
  // If nothing is found, return the default
  return fun_default;
};




/*
WORKER FUNCTIONS
*/

void handleHIDCommands::SetSetpoint(){
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
    for (int i = 0; i < numSensors; i++) {
      bc_string += '\t';
      bc_string += String(ctrlSettings[i].setpoint, 4);
    }
  }
}



void handleHIDCommands::TrajStart(){
    if (broadcast) {
      bc_string += ("TRAJSTART: Trajectory Started");
    }
    trajCtrl.start(); //TODO: This is wrong - need to invoke this on traj_control
  }

void handleHIDCommands::TrajStop(){
    if (broadcast) {
      bc_string += ("TRAJSTOP: Trajectory Stopped");
    }
    trajCtrl.stop();
  }

void handleHIDCommands::TrajPause() {
    if (broadcast) {
      bc_string += ("TRAJPAUSE: Trajectory Paused");
    }
    trajCtrl.pause();
  }

void handleHIDCommands::TrajResume() {
    if (broadcast) {
      bc_string += ("TRAJRESUME: Trajectory Resumed");
    }
    trajCtrl.resume();
  }



void handleHIDCommands::DataOff() {
    settings.outputsOn = false;
    if (broadcast) {
      bc_string += "OFF: Outputs Off";
    }
  }

void handleHIDCommands::DataOn() {
    settings.outputsOn = true;
    if (broadcast) {
      bc_string += "ON: Outputs On";
    }
  }
void handleHIDCommands::SetDataTime() {
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

void handleHIDCommands::SetLCDTime() {
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

  
void handleHIDCommands::StartIntegrator() {
      if (getStringValue(command, ';', numSensors).length()) {
        for (int i = 0; i < numSensors; i++) {
          ctrlSettings[i].integralStart = getStringValue(command, ';', i + 1).toFloat();
        }
        newSettings = true;
        if (broadcast) {
          bc_string += "NEW ";
        }
      }
      else if (getStringValue(command, ';', 1).length()) {
        float allset = getStringValue(command, ';', 1).toFloat();
        for (int i = 0; i < numSensors; i++) {
          ctrlSettings[i].integralStart = allset;
        }
        newSettings = true;
        if (broadcast) {
          bc_string += "NEW ";
        }
      }
      if (broadcast) {
        bc_string += ("INTSTART: ");
        for (int i = 0; i < numSensors; i++) {
          bc_string += String(ctrlSettings[i].integralStart, 4);
          bc_string += ('\t');
        }
      }
    }






  //____________________________________________________________
  //Handle MAXIMUM Software Pressure Limits
void handleHIDCommands::SetMaxPressure() {
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
void handleHIDCommands::SetMinPressure() {
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


void handleHIDCommands::SetValves() {
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



void handleHIDCommands::SetWindow() {
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
void handleHIDCommands::SetPID() {
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
void handleHIDCommands::SetMode() {
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


void handleHIDCommands::Save() {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.saveCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveGlobal(settings);

    if (broadcast) {
      bc_string += ("SAVE: Settings saved to onboard storage");
    }
  }


void handleHIDCommands::Load() {
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


void handleHIDCommands::SaveDefault() {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.saveDefaultCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveDefaultGlobal(settings);
    if (broadcast) {
      bc_string += ("DEFSAVE: Settings saved as default");
    }
  }


void handleHIDCommands::LoadDefault() {
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


void handleHIDCommands::SetChannels() {
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
void handleHIDCommands::TrajConfig() {
    if (getStringValue(command, ';', 3).length()) {
      trajCtrl.start_idx = constrain(getStringValue(command, ';', 1).toInt(), 0, 999);
      trajCtrl.len = constrain(getStringValue(command, ';', 2).toInt(), 1, 1000);
      trajCtrl.wrap = bool(getStringValue(command, ';', 3).toInt());
    }
    if (broadcast) {
      bc_string += ("TRAJCONFIG: start = ");
      bc_string += String(trajCtrl.start_idx);
      bc_string += ('\t');
      bc_string += ("len = ");
      bc_string += String(trajCtrl.len);
      bc_string += ('\t');
      bc_string += ("wrap = ");
      bc_string += String(trajCtrl.wrap);
    }
  }



void handleHIDCommands::TrajWrap() {
    if (getStringValue(command, ';', 1).length()) {
      trajCtrl.wrap = bool(getStringValue(command, ';', 1).toInt());
    }
    if (broadcast) {
      bc_string += ("TRAJWRAP: ");
      bc_string += String(trajCtrl.wrap);
    }
  }

  //[index];[time];[set0];[set1];[set2];[set3]
void handleHIDCommands::TrajSet() {
    if (getStringValue(command, ';', numSensors + 2).length()) {

      float row[numSensors + 2];

      for (int i = 0; i < numSensors + 2; i++) {
        row[i] = getStringValue(command, ';', i + 1).toFloat();
      }

      for (int i = 0; i < numSensors; i++){
        traj[i].setTrajLine(int(row[0]),float(row[1]),float(row[i+2]));
      }


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
        
        for (int i = traj[0].start_idx; i < (traj[0].start_idx + traj[0].len); i++) {
          bc_string += ('\n');
          bc_string += String(traj[0].trajtimes[i]);
          bc_string += ('\t');
          for (int j = 0; j < numSensors; j++) {
            bc_string += String(traj[j].trajpts[i]);
            bc_string += ('\t');
          }
        }
      }

    }


  }




void handleHIDCommands::SetEcho() {
    
    if (getStringValue(command, ';', 1).length()) {
      broadcast = bool(getStringValue(command, ';', 1).toInt());
    }


    if (broadcast) {
      bc_string += ("ECHO: ON");
    }
    else {
      sendString("_ECHO: OFF");
    }
  }


  //Unrecognized
void handleHIDCommands::Unrecognized() {
    newSettings = false;
    if (broadcast) {
      bc_string += ("UNREC: Unrecognized Command");
    }
  }
