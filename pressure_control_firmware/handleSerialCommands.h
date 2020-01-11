#include <stdlib.h>
#include "allSettings.h"
#include "eeprom_handler.h"
#include "trajectory.h"
#include "trajectory_control.h"


#ifndef __handleSerialCommands_H__
#define __handleSerialCommands_H__


class handleSerialCommands
{
  private:      
    int numSensors;
    String command;
    bool broadcast = false;
    eepromHandler saveHandler;

    bool getCommand();
    bool getCommandByChar();
    bool processCommand(globalSettings &, controlSettings *,  Trajectory* , TrajectoryControl &);
    String getStringValue(String, char, int);
  
  public:
    void initialize(int);
    bool go(globalSettings &, controlSettings *, Trajectory* , TrajectoryControl &);
    void startBroadcast();
    void stopBroadcast();
};

#endif
