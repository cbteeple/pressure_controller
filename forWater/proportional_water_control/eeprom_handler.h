#include <stdlib.h>
#include "allSettings.h"

#ifndef __eeprom_handler_H__
#define __eeprom_handler_H__

class eepromHandler
{
  private:
    int defaultGlobalStart = 500;
    int defaultSettingsStart = 550;
    int globalSaveStart = 0;
    int settingsSaveStart = 50;
    int settingsLength = 50; 

    int getSaveLoc(int, int, bool);
  
  public:
    eepromHandler(){};
    void initialize(int, int);
    void loadGlobal(globalSettings &);
    void saveGlobal(globalSettings &);
    void loadDefaultGlobal(globalSettings &);
    void saveDefaultGlobal(globalSettings &);
    void loadCtrl(controlSettings &, int);
    void saveCtrl(controlSettings &, int);
    void loadDefaultCtrl(controlSettings &, int);
    void saveDefaultCtrl(controlSettings &, int);
};

#endif
