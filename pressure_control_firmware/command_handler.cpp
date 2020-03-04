#include <stdlib.h>
#include "Arduino.h"
#include "command_handler.h"
#include "allSettings.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include "eeprom_handler.h"
#include "unit_handler.h"

//_________________________________________________________
//PUBLIC FUNCTIONS
/*
bool CommandHandler::go(globalSettings (&settings), controlSettings *ctrlSettings, Trajectory *traj, TrajectoryControl (&trajCtrl)) {
  bool newCommand = readCommand(); //readCommand();
  bool newSettings = false;
  if (newCommand) {
    newSettings = processCommand(settings, ctrlSettings, traj, trajCtrl);
    command = "";
  }
  return newSettings;
}
*/

bool CommandHandler::go() {
  bool newCommand = readCommand(); //readCommand();
  bool newSettings = false;
  if (newCommand) {
    newSettings = processCommand();
    command = "";
  }
  return newSettings;
}

void CommandHandler::startBroadcast() {
  broadcast = true;
}

void CommandHandler::stopBroadcast() {
  broadcast = false;
}


void CommandHandler::initialize(int num, globalSettings *settings_in, controlSettings *ctrlSettings_in, Trajectory *traj_in, TrajectoryControl *trajCtrl_in, unitHandler *units_in) {
  numSensors   = num;
  settings     = settings_in;
  ctrlSettings = ctrlSettings_in;
  traj         = traj_in;
  trajCtrl     = trajCtrl_in;
  units        = units_in;
  // reserve 200 bytes for the inputString:
  command.reserve(200);

}


//_________________________________________________________
//PRIVATE FUNCTIONS

//METHOD DEPRICATED
#ifdef USB_RAWHID
bool CommandHandler::readCommand() {

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



void CommandHandler::sendString(String bc_string){
  // Reset the buffer with zeros
  for (int i=0; i<64; i++) {
    out_buffer[i] = 0;
  }
  bc_string.getBytes(out_buffer, 64);
  int n = RawHID.send(out_buffer, 1);

}

#else

bool CommandHandler::readCommand() {
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

void CommandHandler::sendString(String bc_string){
  Serial.println(bc_string);
}


#endif




//bool CommandHandler::processCommand(globalSettings (&settings), controlSettings *ctrlSettings, Trajectory *traj, TrajectoryControl (&trajCtrl)) {
bool CommandHandler::processCommand() {
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




String CommandHandler::getStringValue(String data, char separator, int index)
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







CommandHandler::FunctionPointer CommandHandler::findFunction(String str_in){
  for (int i=0; i<num_commands;i++){
    if (str_in.startsWith(str_vec[i])){
      return fun_vec[i];
    }
  }
  // If nothing is found, return the default
  return fun_default;
};




/*
WORKER FUNCTIONS
*/

void CommandHandler::SetSetpoint(){
  if (getStringValue(command, ';', numSensors + 1).length()) {
    for (int i = 0; i < numSensors; i++) {
      ctrlSettings[i].settime = constrain(getStringValue(command, ';', 1).toFloat(), 0, 1000);
      float val = getStringValue(command, ';', i + 2).toFloat();
      ctrlSettings[i].setpoint = constrain(units.convertToInternal(val),
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
      ctrlSettings[i].setpoint = constrain(units.convertToInternal(allset),
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
      bc_string += String(units.convertToExternal(ctrlSettings[i].setpoint), 4);
    }
  }
}



void CommandHandler::TrajStart(){
    if (broadcast) {
      bc_string += ("TRAJSTART: Trajectory Started");
    }
    trajCtrl->start(); //TODO: This is wrong - need to invoke this on traj_control
  }

void CommandHandler::TrajStop(){
    if (broadcast) {
      bc_string += ("TRAJSTOP: Trajectory Stopped");
    }
    trajCtrl->stop();
  }

void CommandHandler::TrajPause() {
    if (broadcast) {
      bc_string += ("TRAJPAUSE: Trajectory Paused");
    }
    trajCtrl->pause();
  }

void CommandHandler::TrajResume() {
    if (broadcast) {
      bc_string += ("TRAJRESUME: Trajectory Resumed");
    }
    trajCtrl->resume();
  }



void CommandHandler::DataOff() {
    settings->outputsOn = false;
    if (broadcast) {
      bc_string += "OFF: Outputs Off";
    }
  }

void CommandHandler::DataOn() {
    settings->outputsOn = true;
    if (broadcast) {
      bc_string += "ON: Outputs On";
    }
  }
void CommandHandler::SetDataTime() {
    if (getStringValue(command, ';', 1).length()) {
      settings->looptime = getStringValue(command, ';', 1).toInt();
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += "TIME: ";
      bc_string += String(settings->looptime);
    }
  }

void CommandHandler::SetLCDTime() {
    if (getStringValue(command, ';', 1).length()) {
      settings->lcdLoopTime = getStringValue(command, ';', 1).toInt();
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += "LCDTIME: ";
      bc_string += String(settings->lcdLoopTime);
    }
  }

  
void CommandHandler::StartIntegrator() {
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
void CommandHandler::SetMaxPressure() {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        float val = getStringValue(command, ';', i + 1).toFloat();
        ctrlSettings[i].maxPressure = units.convertToInternal(val);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      float allset = getStringValue(command, ';', 1).toFloat();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].maxPressure = units.convertToInternal(allset);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += ("MAXP: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(units.convertToExternal(ctrlSettings[i].maxPressure), 4);
        bc_string += ('\t');
      }
    }
  }
  //____________________________________________________________
  //Handle MINIMUM Software Pressure Limits
void CommandHandler::SetMinPressure() {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        float val = getStringValue(command, ';', i + 1).toFloat();
        ctrlSettings[i].minPressure = units.convertToInternal(val);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    else if (getStringValue(command, ';', 1).length()) {
      float allset = getStringValue(command, ';', 1).toFloat();
      for (int i = 0; i < numSensors; i++) {
        ctrlSettings[i].minPressure = units.convertToInternal(allset);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }
    }
    if (broadcast) {
      bc_string += ("MINP: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(units.convertToExternal(ctrlSettings[i].minPressure), 4);
        bc_string += ('\t');
      }
    }
  }


void CommandHandler::SetValves() {
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



void CommandHandler::SetWindow() {
    if (getStringValue(command, ';', numSensors).length()) {
      for (int i = 0; i < numSensors; i++) {
        float val = getStringValue(command, ';', i + 1).toFloat();
        ctrlSettings[i].deadzone = units.convertToInternal(val);
      }
      newSettings = true;
      if (broadcast) {
        bc_string += "NEW ";
      }

    }
    if (broadcast) {
      bc_string += ("WINDOW: ");
      for (int i = 0; i < numSensors; i++) {
        bc_string += String(units.convertToExternal(ctrlSettings[i].deadzone), 4);
        bc_string += ('\t');
      }
    }

  }
void CommandHandler::SetPID() {
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
void CommandHandler::SetMode() {
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


void CommandHandler::Save() {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.saveCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveGlobal(*settings);

    if (broadcast) {
      bc_string += ("SAVE: Settings saved to onboard storage");
    }
  }


void CommandHandler::Load() {
    for (int i = 0; i < numSensors; i++) {
      float set_temp = ctrlSettings[i].setpoint;
      saveHandler.loadCtrl(ctrlSettings[i], i);
      ctrlSettings[i].setpoint = set_temp;
    }
    bool set_temp = settings->outputsOn;
    saveHandler.loadGlobal(*settings);
    settings->outputsOn = set_temp;
    newSettings = true;

    if (broadcast) {
      bc_string += ("LOAD: Settings retrieved from onboard storage");
    }
  }


void CommandHandler::SaveDefault() {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.saveDefaultCtrl(ctrlSettings[i], i);
    }
    saveHandler.saveDefaultGlobal(*settings);
    if (broadcast) {
      bc_string += ("DEFSAVE: Settings saved as default");
    }
  }


void CommandHandler::LoadDefault() {
    for (int i = 0; i < numSensors; i++) {
      saveHandler.loadDefaultCtrl(ctrlSettings[i], i);
    }
    bool set_temp = settings->outputsOn;
    saveHandler.loadDefaultGlobal(*settings);
    settings->outputsOn = set_temp;
    newSettings = true;
    if (broadcast) {
      bc_string += ("DEFLOAD: Settings retrieved from default");
    }
  }


void CommandHandler::SetChannels() {
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
void CommandHandler::TrajConfig() {
    if (getStringValue(command, ';', 4).length()) {
      trajCtrl->setLength(constrain(getStringValue(command, ';', 1).toInt(), 0, 999),
                          constrain(getStringValue(command, ';', 2).toInt(), 0, 128),
                          constrain(getStringValue(command, ';', 3).toInt(), 0, 128));
      trajCtrl->suffix_after_stop = bool(getStringValue(command, ';', 4).toInt());
    }
    if (broadcast) {
      bc_string += ("TRAJCONFIG: ");
      bc_string += ("len = ");
      bc_string += String(trajCtrl->len[0]);
      bc_string += ('\t');
      bc_string += String(trajCtrl->len[1]);
      bc_string += ('\t');
      bc_string += String(trajCtrl->len[2]);
      bc_string += ('\t');
      bc_string += ("suffix after stop = ");
      bc_string += String(trajCtrl->suffix_after_stop);
    }
  }


void CommandHandler::TrajWrap() {
  if (getStringValue(command, ';', 1).length()) {

      bool wrap = bool(getStringValue(command, ';', 1).toInt());
    
      if(wrap){
        trajCtrl->num_cycles = -1; //Set to loop forever
      }
      else{
        trajCtrl->num_cycles = 1; //Set to just single trajectory
      }
    
      
    }
  if (broadcast) {
    bc_string += ("TRAJWRAP: ");
    bc_string += String(trajCtrl->num_cycles);
  }
}



void CommandHandler::SetTrajSpeed() {
  if (getStringValue(command, ';', 1).length()) {
      trajCtrl->setSpeed(getStringValue(command, ';', 1).toFloat());
    }
  if (broadcast) {
    bc_string += ("TRAJSPEED: ");
    bc_string += String(1 / traj[0].stretch_factor, 4);
  }
}


void CommandHandler::SetTrajLoop() {
  if (getStringValue(command, ';', 1).length()) {
      trajCtrl->num_cycles = constrain((getStringValue(command, ';', 1)).toInt(), -1, 1000000);
    }
  if (broadcast) {
    bc_string += ("TRAJLOOP: ");
    bc_string += String(trajCtrl->num_cycles);
  }
}


  //[index];[time];[set0];[set1];[set2];[set3]

void CommandHandler::TrajSet() {
  TrajLineSet(1);
}
void CommandHandler::PrefixSet() {
  TrajLineSet(0);
}
void CommandHandler::SuffixSet() {
  TrajLineSet(2);
}

  
void CommandHandler::TrajLineSet(int which_traj) {
  // If there is at least one channel's worth of data, pad the new row with zeros and add in new data
  String cmd_type[3] ={"PREFSET", "TRAJSET", "SUFFSET"};
  if(getStringValue(command, ';', 3).length()){

    float row[numSensors + 2];

    for (int i = 0; i < numSensors + 2; i++) {
      String new_entry=getStringValue(command, ';', i + 1);
      if(new_entry.length()==0){
        break;  
      }
      row[i] = new_entry.toFloat();
    }


    for (int i = 0; i < numSensors; i++){
      switch(which_traj){
        case 0:{
          traj[i].setPrefixLine(int(row[0]),
                float(row[1]),
                units.convertToInternal(float(row[i+2])) );
        } break;
        case 1:{
          traj[i].setTrajLine(int(row[0]),
                float(row[1]),
                units.convertToInternal(float(row[i+2])) );
        } break;
        case 2:{
          traj[i].setSuffixLine(int(row[0]),
                float(row[1]),
                units.convertToInternal(float(row[i+2])) );
        } break;
      }
    }

    // If we are broadcasting, spit out the line that was just written
    if (broadcast) {
      bc_string += cmd_type[which_traj];
      bc_string += (": ");
      bc_string += String(which_traj,1);
      bc_string += ("\t");
      for (int i = 0; i < numSensors + 2; i++) {
        bc_string += ('\t');
        bc_string += String(units.convertToExternal(row[i]), 2);
      }
    }
  }

  // If there's no new data (or incomplete), spit out the whole trajectory
  else {
    if (broadcast) {
      bc_string += cmd_type[which_traj];
      bc_string += (":");
      bc_string += ('\n');
      bc_string += ("_Prefix:");
      
      if (traj[0].len[0] ==0){
        bc_string += " Empty";
      }
      
      for (int i = 1; i < (traj[0].len[0]); i++) {
        bc_string += ('\n');
        bc_string += String(traj[0].prefixtimes[i]);
        bc_string += ('\t');
        for (int j = 0; j < numSensors; j++) {
          bc_string += String(units.convertToExternal(traj[j].prefixpts[i]));
          bc_string += ('\t');
        }
      }
      bc_string += ('\n');
      bc_string += ("_Trajectory:");

      if (traj[0].len[1] ==0){
        bc_string += " Empty";
      }

      for (int i = 0; i < (traj[0].len[1]); i++) {
        bc_string += ('\n');
        bc_string += String(traj[0].trajtimes[i]);
        bc_string += ('\t');
        for (int j = 0; j < numSensors; j++) {
          bc_string += String(units.convertToExternal(traj[j].trajpts[i]));
          bc_string += ('\t');
        }
      }

      bc_string += ('\n');
      bc_string += ("_Suffix:");

      if (traj[0].len[2] ==0){
        bc_string += " Empty";
      }

      for (int i = 1; i < (traj[0].len[2]); i++) {
        bc_string += ('\n');
        bc_string += String(traj[0].suffixtimes[i]);
        bc_string += ('\t');
        for (int j = 0; j < numSensors; j++) {
          bc_string += String(units.convertToExternal(traj[j].suffixpts[i]));
          bc_string += ('\t');
        }
      }
    }

  }


  }




void CommandHandler::SetEcho() {
    
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



void CommandHandler::SetMasterPressure(){
  if (getStringValue(command, ';', 2).length()) {
      settings->useMasterPressure = bool(getStringValue(command, ';', 1).toInt());
      settings->masterPressureOutput = bool(getStringValue(command, ';', 2).toInt());
      
      if (broadcast) {
          bc_string += "NEW ";
      }
  }
  else if (getStringValue(command, ';', 1).length()) {
      bool value = bool(getStringValue(command, ';', 1).toInt());
      settings->useMasterPressure = value;
      settings->masterPressureOutput = value;
      
      if (broadcast) {
          bc_string += "NEW ";
      }
  }

    if (broadcast) {
      bc_string += ("MASTERP: ");
      bc_string += settings->useMasterPressure;
      bc_string += '\t';
      bc_string += settings->masterPressureOutput;
    }
}


  //____________________________________________________________
  //Handle MAXIMUM Software Pressure Limits for the master pressure
void CommandHandler::SetMasterMaxPressure() {
  if (getStringValue(command, ';', 2).length()) {
      float val = getStringValue(command, ';', 1).toFloat();
      settings->maxPressure = units.convertToInternal(val);
      settings->watchdogSpikeTime = constrain(getStringValue(command, ';', 2).toInt(),0,10000);
      
      if (broadcast) {
          bc_string += "NEW ";
      }
  }
    if (broadcast) {
      bc_string += ("MASTERMAXP: ");
      bc_string += units.convertToExternal(settings->maxPressure);
      bc_string += '\t';
      bc_string += settings->watchdogSpikeTime;
  }
}


void CommandHandler::SetUnits() {
  if (getStringValue(command, ';', 1).length()) {


      int val = constrain(getStringValue(command, ';', 1).toInt(),0,1000);

      units.setUnits(val);
      
      if (broadcast) {
          bc_string += "NEW ";
      }
  }
  else if (getStringValue(command, ';', 2).length()) {


      int val = constrain(getStringValue(command, ';', 1).toInt(),0,1000);
      int val1 = constrain(getStringValue(command, ';', 2).toInt(),0,1000);

      units.setInputUnits(val);
      units.setOutputUnits(val1);
      
      if (broadcast) {
          bc_string += "NEW ";
      }
  }
    if (broadcast) {
      bc_string += ("UNITS: ");
      bc_string += units->getInputName();
      bc_string += '\t';
      bc_string += units->getOutputName();
  }
}



  //Unrecognized
void CommandHandler::Unrecognized() {
    newSettings = false;
    if (broadcast) {
      bc_string += ("UNREC: Unrecognized Command");
    }
  }
