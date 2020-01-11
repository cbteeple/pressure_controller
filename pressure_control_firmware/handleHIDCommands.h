#include <stdlib.h>
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
    bool broadcast = false;
    eepromHandler saveHandler;

    // All of the settings pointers
    globalSettings *settings;
    controlSettings *ctrlSettings;
    Trajectory *traj;
    TrajectoryControl *trajCtrl;


    bool getCommand();
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
