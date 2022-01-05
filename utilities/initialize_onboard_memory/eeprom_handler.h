#include <stdlib.h>
#include "allSettings.h"

#ifndef __eeprom_handler_H__
#define __eeprom_handler_H__

class eepromHandler
{
  private:
    int defaultGlobalStart = 550;
    int defaultSettingsStart = 600;
    int globalSaveStart = 0;
    int valveSaveStart = 50;
    int valveLength = 4;
    int settingsSaveStart = 100;
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
    void loadValves(valveSettings &, int);
    void saveValves(valveSettings &, int);
};

#endif
