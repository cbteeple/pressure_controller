// These are the sensors used in version 3.4 of the pressure control board
#include <stdlib.h>

#ifndef __SSCSNBN030PDAC5_H__
#define __SSCSNBN030PDAC5_H__

class SSCSNBN030PDAC5
{
  public:
    int SENSOR_MODEL=4;

    float sensor_output_max = 4.25;
    float sensor_output_min = 0.25;
    float sensor_output_offset = 0.25;

    float sensor_pressure_max = 30; //PSI
    float sensor_pressure_min = -30; //PSI

};


// TODO: add all other sensors into a "sensor_cal" library


#endif