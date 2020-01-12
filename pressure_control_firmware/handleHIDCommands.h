#include <stdlib.h>
#include "Arduino.h"
#include "allSettings.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include "eeprom_handler.h"



#ifndef __handleHIDCommands_H__
#define __handleHIDCommands_H__



class handleHIDCommands
{
  typedef void (handleHIDCommands::*FunctionPointer)();
  //typedef void (*FunctionPointer)(); // function pointer type
  private:
    byte in_buffer[64];
    byte out_buffer[64];   
    int numSensors;
    String command;
    String bc_string = "_";
    bool newSettings = false;
    bool broadcast = false;
    eepromHandler saveHandler;

    // Function map setup

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
    void SetEcho();
    void Unrecognized();

    
    // Define the map to refer to the worker functions
    const static unsigned int num_commands= 25;
    String str_vec[num_commands]={"SET",
                                  "TRAJSTART",
                                  "TRAJSTOP",
                                  "TRAJPAUSE",
                                  "TRAJRESUME",
                                  "OFF",
                                  "ON",
                                  "TIME",
                                  "LCDTIME",
                                  "INTSTART",
                                  //
                                  "MAXP",
                                  "MINP",
                                  "VALVE",
                                  "WINDOW",
                                  "PID",
                                  "MODE",
                                  "SAVE",
                                  "LOAD",
                                  "DEFSAVE",
                                  "DEFLOAD",
                                  //
                                  "CHAN",
                                  "TRAJCONFIG",
                                  "TRAJWRAP",
                                  "TRAJSET",
                                  "ECHO"};

    FunctionPointer fun_vec[num_commands]={&SetSetpoint,
                                           &TrajStart,
                                           &TrajStop,
                                           &TrajPause,
                                           &TrajResume,
                                           &DataOff,
                                           &DataOn,
                                           &SetDataTime,
                                           &SetLCDTime,
                                           &StartIntegrator,
                                           //
                                           &SetMaxPressure,
                                           &SetMinPressure,
                                           &SetValves,
                                           &SetWindow,
                                           &SetPID,
                                           &SetMode,
                                           &Save,
                                           &Load,
                                           &SaveDefault,
                                           &LoadDefault,
                                           //
                                           &SetChannels,
                                           &TrajConfig,
                                           &TrajWrap,
                                           &TrajSet,
                                           &SetEcho};
    
    FunctionPointer fun_default = &Unrecognized;



    int idx = 0;

    // All of the settings pointers
    globalSettings &settings;
    controlSettings *ctrlSettings;
    Trajectory *traj;
    TrajectoryControl &trajCtrl;

    int getCommandType();
    bool readCommand();
    bool processCommand();
    String getStringValue(String, char, int);

    FunctionPointer findFunction(String);
  
  public:
    handleHIDCommands(){};
    void initialize(int, globalSettings &, controlSettings *, Trajectory *, TrajectoryControl &);
    bool go();
    void startBroadcast();
    void stopBroadcast();
    void sendString(String);
};

#endif
