#include <stdlib.h>
#include "allSettings.h"
#include "eeprom_handler.h"
#include "trajectory.h"
#include "trajectory_control.h"


#ifndef __handleHIDCommands_H__
#define __handleHIDCommands_H__


class handleHIDCommands
{
  private:
    byte in_buffer[64];
    byte out_buffer[64];   
    int numSensors;
    String command;
    bool broadcast = false;
    eepromHandler saveHandler;

    bool getCommand();
    bool processCommand(globalSettings &, controlSettings *, Trajectory *, TrajectoryControl &);
    String getStringValue(String, char, int);
  
  public:
    void initialize(int);
    bool go(globalSettings &, controlSettings *, Trajectory *, TrajectoryControl &);
    void startBroadcast();
    void stopBroadcast();
    void sendString(String);
};

#endif
