#include "eeprom_handler.h"
#include "allSettings.h"
#include "Arduino.h"
#include <EEPROMAnything.h>


//Save global settings
void eepromHandler::loadGlobal(globalSettings (&settings)){
  EEPROM_readAnything(getSaveLoc(1, 0, false),settings);
}

void eepromHandler::saveGlobal(globalSettings (&settings)){
  EEPROM_writeAnything(getSaveLoc(1, 0, false),settings);
}


void eepromHandler::loadDefaultGlobal(globalSettings (&settings)){
  EEPROM_readAnything(getSaveLoc(1, 0, true),settings);
}

void eepromHandler::saveDefaultGlobal(globalSettings (&settings)){
  EEPROM_writeAnything(getSaveLoc(1, 0, true),settings);
}


//Save control settings for a single control line.
void eepromHandler::loadCtrl(controlSettings *ctrlSettings, int channelIdx){
  EEPROM_readAnything(getSaveLoc(0, channelIdx,false), ctrlSettings);
}

void eepromHandler::saveCtrl(controlSettings *ctrlSettings, int channelIdx){
  EEPROM_writeAnything(getSaveLoc(0, channelIdx,false), ctrlSettings);
}


//Save or load default control settings for a single control line.
void eepromHandler::loadDefaultCtrl(controlSettings *ctrlSettings, int channelIdx){
  EEPROM_readAnything(getSaveLoc(0, channelIdx,true), ctrlSettings);
}

void eepromHandler::saveDefaultCtrl(controlSettings *ctrlSettings, int channelIdx){
  EEPROM_writeAnything(getSaveLoc(0, channelIdx,true), ctrlSettings);
}



int eepromHandler::getSaveLoc(int settingType, int channelIdx, bool isDefault){
  int settingsStart = 0;

  if (isDefault){
    switch(settingType){
      case 0: settingsStart = defaultSettingsStart + channelIdx*settingsLength;
              break;
      case 1: settingsStart = defaultGlobalStart;
            break;
    }
  }
  else{
    switch(settingType){
      case 0: settingsStart = settingsSaveStart + channelIdx*settingsLength;
              break;
      case 1: settingsStart = globalSaveStart;
            break;
    }
  }
  return settingsStart;
}
