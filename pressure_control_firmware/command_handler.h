#include <stdlib.h>
#include "Arduino.h"
#include "allSettings.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include "eeprom_handler.h"
#include "unit_handler.h"



#ifndef __command_handler_H__
#define __command_handler_H__



class CommandHandler
{
  typedef void (CommandHandler::*FunctionPointer)();
  //typedef void (*FunctionPointer)(); // function pointer type
  private:
    byte in_buffer[64];
    byte out_buffer[64];   
    int numSensors;
    String command;
    String commandStr;
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
    void SetPressureSpikeTime();
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
    void SetTrajLoop();
    void SetTrajSpeed();
    void SetEcho();
    void Unrecognized();

    void TrajSet();
    void PrefixSet();
    void SuffixSet();
    void TrajLineSet(int);

    void SetMasterPressure();
    void SetMasterMaxPressure();

    void SetUnits();
    void GetCurrTime();
    void SetValveOffsets();

    void ResetControllers();
    void GetFirmwareVersion();
    void GetCmdSpecVersion();
    void GetErrorState();



    
    // Define the map to refer to the worker functions
    const static unsigned int num_commands= 39;
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
                                  "SPIKE",
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
                                  "TRAJSPEED",
                                  "TRAJLOOP",
                                  "TRAJWRAP",
                                  "TRAJSET",
                                  "PREFSET",
                                  "SUFFSET",
                                  "ECHO",
                                  "MASTERP",
                                  "MASTERMAXP",
                                  //
                                  "UNITS",
                                  "CURRTIME",
                                  "VOFFSET",
                                  //
                                  "RESET",
                                  "FIRMWARE",
                                  "CMDSPEC",
                                  "ERROR",};

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
                                           &SetPressureSpikeTime,
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
                                           &SetTrajSpeed,
                                           &SetTrajLoop,
                                           &TrajWrap,
                                           &TrajSet,
                                           &PrefixSet,
                                           &SuffixSet,
                                           &SetEcho,
                                           &SetMasterPressure,
                                           &SetMasterMaxPressure,
                                           //
                                           &SetUnits,
                                           &GetCurrTime,
                                           &SetValveOffsets,
                                           //
                                           &ResetControllers,
                                           &GetFirmwareVersion,
                                           &GetCmdSpecVersion,
                                           &GetErrorState};
    
    FunctionPointer fun_default = &Unrecognized;



    int idx = 0;

    // All of the settings pointers
    globalSettings *settings;
    controlSettings *ctrlSettings;
    Trajectory *traj;
    TrajectoryControl *trajCtrl;
    UnitHandler *units;
    valveSettings *valvePairSettings;
    internalSettings *intSettings;

    int getCommandType();
    bool readCommand();
    bool processCommand();
    String getStringValue(String, char, int);

    FunctionPointer findFunction(String);
  
  public:
    CommandHandler(){};
    void initialize(int, globalSettings *, controlSettings *, Trajectory *, TrajectoryControl *, UnitHandler *, valveSettings*, internalSettings *);
    bool go();
    void startBroadcast();
    void stopBroadcast();
    void sendString(String);
};

#endif
