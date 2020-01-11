#include <stdlib.h>

#ifndef __sensors_lib_H__
#define __sensors_lib_H__

// Used in version 3.4 of the pressure control board
class SensorSSCDANN150PGAA5
{
  public:
    int   model=3;

    float output_max = 4.5;
    float output_min = 0.5;
    float output_offset = 0.5;

    float pressure_max = 150; //PSI
    float pressure_min = 0; //PSI
};

class SensorSSCSNBN030PDAC5
{
  public:
    int   model=4;

    float output_max = 4.25;
    float output_min = 0.25;
    float output_offset = 0.25;

    float pressure_max = 30; //PSI
    float pressure_min = -30; //PSI

};



// Used in version 1.0 of the pressure control board
class SensorSSCDANN060PGAA5
{
  public:
    int   model=3;

    float output_max = 4.5;
    float output_min = 0.5;
    float output_offset = 0.5;

    float pressure_max = 60; //PSI
    float pressure_min = 0; //PSI
};



// Used in version 1.0 of the vacuum control board
class SensorASDXRRX030PDAA5
{
  public:
    int   model=4;

    float output_max = 4.5;
    float output_min = 0.5;
    float output_offset = 0.5;

    float pressure_max = 30; //PSI
    float pressure_min = -30; //PSI
};


// Return the direct analog input values 
// For debugging purposes only. DO NOT CONNECT VALVES WHEN USING THIS
class SensorRawAnalogInput
{
  public:
    int   model=99;

    float output_max = 1;
    float output_min = 1;
    float output_offset = 1;

    float pressure_max = 1;
    float pressure_min = 1;
};


// No sensor connected
class SensorDisconnected
{
  public:
    int   model = 100;
    float output_max = 1;
    float output_min = 1;
    float output_offset = 1;

    float pressure_max = 1;
    float pressure_min = 1;
};


#endif