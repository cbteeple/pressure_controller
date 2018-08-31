#include <stdlib.h>
#include "sensorSettings.h"


class handleSerialCommands
{
  private:      
    int numSensors;
    String command;
    bool broadcast = true;
    bool getCommand();
    void processCommand(sensorSettings &);
    String getStringValue(String, char, int);
  
  public:
    void initialize(int);
    void go(sensorSettings &);
    void startBroadcast();
    void stopBroadcast();
};
