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
};

class trajectory
{
  private:
    const static unsigned int maxLen = 200;
    int curr_idx = 1;
    
  public:
    unsigned long StartTime=0;
    unsigned long CurrTime=0;
    
    int len = 20;
    bool wrap = false;
    bool running = false;
    float trajpts [maxLen][4];
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

      for(int i=2;i<(6);i++){
        trajpts[idx][i-2]=row[i];
      }
    };


    void start(){
      running = true;
      curr_idx = 1;
      StartTime = CurrTime;
    }

    void stop(){
      running=false;  
    }

    float interp(int channel){

      //Make sure to interpolate in the correct region
      float deltaT = float(CurrTime-StartTime)/1000.0;

      if(deltaT >= trajtimes[len-1]){
        curr_idx = len-1;
        deltaT = trajtimes[len-1];

        running=false;  
        
      }
      else{      
        while(deltaT>trajtimes[curr_idx]){
          curr_idx++;
  
          if (curr_idx>=len-1){
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

        if ((deltaT >= trajtimes[len-1]) & wrap & channel==3){
          start();
        }

        //Serial.print("_SETPOINT: ");
        //Serial.print(channel);
        //Serial.print('\t');
        //Serial.print(set);
        //Serial.print('\n');
      return set;
    }



    float lerp(float a, float b, float f){
    return a + f * (b - a);
    }

};




#endif

