#include <stdlib.h>

#ifndef __interp_lin_H__
#define __interp_lin_H__

#include "allSettings.h"

class interpLin
{
  private:

    float value_A = 0;
    float value_B = 0;
    float curr_value = 0;
    float transition_time = 0;
    bool new_goal = false;

    float lerp(float, float ,float);
  
  public:
    unsigned long StartTime=0;
    unsigned long CurrTime=0;

    bool running = false;

    interpLin(){};
    void newGoal(controlSettings &);
    float go();

    void start();
    void stop();

};

#endif
