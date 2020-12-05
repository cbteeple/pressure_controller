#include "Arduino.h"
#include "sensors_lib.h"
//CONFIG FOR PNEUMATIC PRESSURE SYSTEM

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

#define MAX_NUM_CHANNELS 1

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
  //int senseChannels[]={A3, A2, A1, A22, A21, A20, A15, A14, A12, A13};

  // Using all the even-numbered channels on the board.
  // These will be channels 0-4 (ignore the numbers on the board)
  int senseChannels[]={A2, A3, A4, A5, A6}; 
  int masterSenseChannel = A1;

// Set valve pins
  int valvePins[][2]= { {8,9}, {10,11}, {2,3}, {4,5}, {6,7} };
  int valveOffset[][2]={{225,225},{225,225},{225,225},{225,225}, {225,225}};

  int masterValvePin = 12;

// Set Button pins
  int buttonPins[]={13,14,15};
  int robotPins[] ={13,14,15};
  int extPins[]   ={16,17};

// Default controller settings
  float pid_start[]={0.1,0.001,0}; 
  float deadzone_start=0.0;
  float setpoint_start=0;
  float integratorResetTime_start = -1; // Turn off integrator chopping
  float minPressure_start = 0; //[psi]
  float maxPressure_start = 28; //[psi]