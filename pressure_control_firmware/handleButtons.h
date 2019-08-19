#include <stdlib.h>
#include "allSettings.h"
#include <Button.h>


#ifndef __handleButtons_H__
#define __handleButtons_H__

class handleButtons
{
  private:
    unsigned long pressStart[3];
    unsigned long pressEnd[3];
    unsigned long longPressDuration = 1500;
    int doFunction;
    int numSensors;
    
    bool checkForPress(Button * );
    bool checkForLongPress(int);
  
  public:
    handleButtons(int );
    void initialize();
    String go(Button * , globalSettings &, controlSettings *);
};

#endif
