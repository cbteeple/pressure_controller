#include <stdlib.h>
#include <ArduinoSTL.h>
#include <map>
#include "allSettings.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include "eeprom_handler.h"



#ifndef __handleHIDCommands_H__
#define __handleHIDCommands_H__


class handleHIDCommands
{
  private:
    byte in_buffer[64];
    byte out_buffer[64];   
    int numSensors;
    String command;
    String bc_string = "_";
    bool broadcast = false;
    eepromHandler saveHandler;

    // Declare all of the worker functions
    void SetSetpoint();
    void TrajStart();
    void TrajStop();
    void TrajPause();
    void TrajResume();
    void DataOff();
    void DataOn();
    void SetDataTime();
    void SetLCDTime();
    void StartIntegrator();
    void SetMaxPressure();
    void SetMinPressure();
    void SetValves();
    void SetWindow();
    void SetPID();
    void SetMode();
    void Save();
    void Load();
    void SaveDefault();
    void LoadDefault();
    void SetChannels();
    void TrajConfig();
    void TrajWrap();
    void TrajSet();
    void Echo();
    void Unrecognized();

    
    // Define the map to refer to the worker functions
    typedef void (*ScriptFunction)(void); // function pointer type
    typedef std::unordered_map<String, ScriptFunction> script_map;
    script_map command_map;

    command_map.emplace("SET",        &SetSetpoint);
    command_map.emplace("TRAJSTART",  &TrajStart);
    command_map.emplace("TRAJSTOP",   &TrajStop);
    command_map.emplace("TRAJPAUSE",  &TrajPause);
    command_map.emplace("TRAJRESUME", &TrajResume);
    command_map.emplace("OFF",        &DataOff);
    command_map.emplace("ON",         &DataOn);
    command_map.emplace("TIME",       &SetDataTime);
    command_map.emplace("LCDTIME",    &SetLCDTime);
    command_map.emplace("INTSTART",   &StartIntegrator);
    command_map.emplace("MAXP",       &SetMaxPressure);
    command_map.emplace("MINP",       &SetMinPressure);
    command_map.emplace("VALVE",      &SetValves);
    command_map.emplace("WINDOW",     &SetWindow);
    command_map.emplace("PID",        &SetPID);
    command_map.emplace("MODE",       &SetMode);
    command_map.emplace("SAVE",       &Save);
    command_map.emplace("LOAD",       &Load);
    command_map.emplace("DEFSAVE",    &SaveDefault);
    command_map.emplace("DEFLOAD",    &LoadDefault);
    command_map.emplace("CHAN",       &SetChannels);
    command_map.emplace("TRAJCONFIG", &TrajConfig);
    command_map.emplace("TRAJCONFIG", &TrajWrap);
    command_map.emplace("TRAJSET",    &TrajSet);
    command_map.emplace("ECHO",       &SetEcho);
    command_map.emplace("NA",         &Unrecognized);


    // All of the settings pointers
    globalSettings *settings;
    controlSettings *ctrlSettings;
    Trajectory *traj;
    TrajectoryControl *trajCtrl;

    int getCommandType();
    bool readCommand();
    bool processCommand();
    String getStringValue(String, char, int);
  
  public:
    void initialize(int, globalSettings *, controlSettings *, Trajectory *, TrajectoryControl *);
    bool go();
    void startBroadcast();
    void stopBroadcast();
    void sendString(String);
};

#endif
