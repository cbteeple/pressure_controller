#include <stdlib.h>
#include "allSettings.h"
#include "trajectory.h"
#include "trajectory_control.h"
#include "eeprom_handler.h"


#ifndef __handleSerialCommands_H__
#define __handleSerialCommands_H__


class handleSerialCommands
{
  private:      
    int numSensors;
    String command;
    bool broadcast = false;
    eepromHandler saveHandler;

    // All of the settings pointers
    globalSettings &settings;
    controlSettings *ctrlSettings;
    Trajectory *traj;
    TrajectoryControl &trajCtrl;

    bool getCommand();
    bool getCommandByChar();
    bool processCommand();
    String getStringValue(String, char, int);
  
  public:
    void initialize(int, globalSettings &, controlSettings *, Trajectory *, TrajectoryControl &);
    bool go();
    void startBroadcast();
    void stopBroadcast();
};

#endif
