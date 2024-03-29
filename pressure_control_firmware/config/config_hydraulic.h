#include "Arduino.h"
#include "sensors_lib.h"
//CONFIG FOR PNEUMATIC PRESSURE SYSTEM


//SENSING:
  //Arduino ADC pins:
    //Mega:  A0-A15                        (all pins work fine)
    //Uno:   A0-A5                         (all pins work fine)
    //Micro: A0, A1, A4 - A8, A11          (A9, A10, A12, A13 all conflict with PWM pins)
    //Nano:  A0-A7                         (A4, A5 are i2c pins)

//VALVES:
  //Arduino PWM pins:
    //Mega:  2 - 13, 44, 45, 46            (all pins work fine)
    //Uno:   3, 5, 6, 9, 10, 11            (all pins work fine)
    //Micro: 3, 5, 6, 9, 10, 11, 12, 13    (all pins work fine)
    //Nano:  3, 6, 9, 10, 11               (pin 5 fails consistently, no idea why)

// Use raw USB communication as an HID object - Comment out for Serial communication
//#define COMMS_USB


// Define the type of sensor to use (only one can be true)
#define SENSOR_ANALOG true
#define SENSOR_I2C false

//Define the sensor types
SensorM5231_000005_050PG controlSensorType;
SensorDisconnected masterSensorType;

#define MASTER_SENSOR false

// Define mcu analog input properties
#define ADC_RES 10
#define ADC_MAX_VOLTS 5.0
float   ADC_MULT = 1.0;

#define MAX_NUM_CHANNELS 3

//Define the type of controller to use (only one can be true)
#define CONTROL_BANGBANG false
#define CONTROL_P false
#define CONTROL_PID true

//Set default settings for things
//If using i2c sensors...
  int sensorAddr=0x58;
  bool useMux=true;
  int muxAddr=0x70;

//Set sensor pins
  //int senseChannels[]={A0,A1,A2,A3,A4,A5,A6,A7,A8};
  int senseChannels[]={A1,A2,A3};
  int masterSenseChannel = A4;

//Set valve pins
  int valvePins[][2]= { {4,5}, {6,7}, {8,9}};
  int valveOffset[][2]={{205,205},{205,205},{205,205}};

  int masterValvePin = 10;

//Set Button pins
  int buttonPins[]={2,18,19};
  int robotPins[] ={2,18,19};
  int extPins[]   ={11,12};



//Default controller settings
  float pid_start[]={0.6,0.03,0}; 
  float deadzone_start=0.0;
  float setpoint_start=0;
  float integratorResetTime_start = -1;
  float minPressure_start = 0; //[psi]
  float maxPressure_start = 7; //[psi]
