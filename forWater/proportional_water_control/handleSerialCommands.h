#include <stdlib.h>
#include "allSettings.h"
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

    bool getCommand();
    bool processCommand(globalSettings &, controlSettings *);
    String getStringValue(String, char, int);
  
  public:
    void initialize(int);
    bool go(globalSettings &, controlSettings *);
    void startBroadcast();
    void stopBroadcast();
};

#endif
