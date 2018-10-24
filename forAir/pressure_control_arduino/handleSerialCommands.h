#include <stdlib.h>
#include "sensorSettings.h"


#ifndef __handleSerialCommands_H__
#define __handleSerialCommands_H__


class handleSerialCommands
{
  private:      
    int numSensors;
    String command;
    bool broadcast = true;
    bool getCommand();
    bool processCommand(sensorSettings &);
    String getStringValue(String, char, int);
  
  public:
    void initialize(int);
    bool go(sensorSettings &);
    void startBroadcast();
    void stopBroadcast();
};

#endif
