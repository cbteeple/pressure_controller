#include <stdlib.h>
#include "Arduino.h"


#ifndef __allSettings_H__
#define __allSettings_H__

class globalSettings
{
  public:
    unsigned int looptime = 0;
    bool outputsOn=false;
    unsigned int lcdLoopTime = 500;
    
};



class sensorSettings
{
  public:
  	//Shared
    bool on = true;
  	int sensorModel=1;

  	//i2c stuff
  	bool useMux;
    int sensorAddr;
    int muxAddr;
    int muxChannel;

    //Analog stuff
    int sensorPin;
    int adc_res;
    int adc_max_volts;
};




class controlSettings
{
  public:
    bool channelOn = true;
    float setpoint;
    float deadzone;
    int controlMode;
    float valveDirect;
    float pidGains [3];
    float integratorResetTime;
    float integralStart = 3.0;
    float maxPressure;
    float minPressure;
    float settime;
};

class trajectory
{
  private:
   
    const static unsigned int maxLen = 1000;
    const static unsigned int maxChannels = 4;
    int curr_idx = 1;

    float lerp(float a, float b, float f){
    return a + f * (b - a);
    }
    
  public:
    unsigned long StartTime=0;
    unsigned long CurrTime=0;
    
    int len = 20;
    int start_idx = 0;
    bool wrap = false;
    bool running = false;
    float trajpts [maxLen][maxChannels];
    float trajtimes [maxLen];
    
    bool setLength(int len_in){
      bool error = false;
      if (len_in<=maxLen){
        len = len_in;
      }
      else{
        error = true;  
      }
      return error;
      
    };

    void setLine(float row[]){
      bool error =false;
      int idx = int(row[0]);
      trajtimes[idx]=row[1];

      for(int i=0;i<(6);i++){
        trajpts[idx][i]=row[i+2];
      }
    };


    void start(){
      running = true;
      curr_idx = start_idx+1;
      StartTime = CurrTime;
    }

    void stop(){
      running=false;  
    }

    float interp(int channel){

      //Make sure to interpolate in the correct region
      float deltaT = float(CurrTime-StartTime)/1000000.0;

      if(deltaT >= trajtimes[start_idx+len-1]){
        curr_idx = start_idx+len-1;
        deltaT = trajtimes[start_idx+len-1];

        running=false;  
        
      }
      else{      
        while(deltaT>trajtimes[curr_idx]){
          curr_idx++;
  
          if (curr_idx>=start_idx+len-1){
            break;
          }
        }
      }

      //Do an interpolation
      float percent = (deltaT-trajtimes[curr_idx-1]) / (trajtimes[curr_idx] - trajtimes[curr_idx-1]);
      float a = trajpts[curr_idx-1][channel];
      float b = trajpts[curr_idx][channel];
      
      float set=lerp(
        a,
        b,
        percent
        );

        if ((deltaT >= trajtimes[start_idx+len-1]) & wrap & channel==maxChannels-1){
          start();
        }

        //Serial.print("_SETPOINT: ");
        //Serial.print(channel);
        //Serial.print('\t');
        //Serial.print(set);
        //Serial.print('\n');
      return set;
    }

};




#endif



