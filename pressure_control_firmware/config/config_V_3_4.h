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
SensorSSCSNBN030PDAC5 controlSensorType;
SensorSSCDANN150PGAA5 masterSensorType;

#define MASTER_SENSOR true

// Define mcu analog input properties
#define ADC_RES 13
#define ADC_MAX_VOLTS 3.3
float   ADC_MULT = 0.6666666;

#define MAX_NUM_CHANNELS 8

// Define the type of controller to use (only one can be true)
#define CONTROL_BANGBANG false
#define CONTROL_P false
#define CONTROL_PID true


// Set default settings for things
// If using i2c sensors...
  int sensorAddr=0x58;
  bool useMux=false;
  int muxAddr=0x70;

// Set sensor pins
  int senseChannels[]={A3, A2, A1, A22, A21, A20, A15, A14, A12, A13};
  int masterSenseChannel = A24;

// Set valve pins
  int valvePins[][2]= { {23,22}, {21,20}, {2,3}, {4,5}, {6,7}, {8,9}, {10,14}, {29,30}, {38,37}, {36,35} };
  int valveOffset[][2]={{227,226},{224,224},{225,225},{225,225}, {225,225}, {225,225}, {225,225}, {225,225}, {220,225}, {225,225}};

  int masterValvePin = 28;

// Set Button pins
  int buttonPins[]={27,26,25};
  int robotPins[] ={27,26,25};
  int extPins[]   ={12,11};

// Default controller settings
  float pid_start[]={0.1,0.001,0}; 
  float deadzone_start=0.0;
  float setpoint_start=0;
  float integratorResetTime_start = -1;
  float minPressure_start = 0; //[psi]
  float maxPressure_start = 28; //[psi]