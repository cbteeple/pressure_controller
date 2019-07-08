#include <stdlib.h>
#include "Arduino.h"
#include "handleButtons.h"
#include "allSettings.h"
#include <Button.h>

//_________________________________________________________
//PUBLIC FUNCTIONS

handleButtons::handleButtons(int num){
    numSensors=num;
    doFunction = 0;
    }



void handleButtons::initialize(){
  
}



String handleButtons::go(Button *buttons, globalSettings (&settings), controlSettings *ctrlSettings){

    bool newState = checkForPress(buttons);

    if (!newState){
      return "";
    }

  
    String buttonMessage = "";
    switch (doFunction){
        case 1: {
            if (checkForLongPress(0)){
                if (ctrlSettings[0].controlMode == 1){
                    for (int i =0; i<numSensors; i++){
                        ctrlSettings[i].controlMode = 0;
                        ctrlSettings[i].valveDirect = 0;
                        
                    }
                    buttonMessage+= "Direct Valve Set\n" +String(ctrlSettings[0].valveDirect,2);
                }
                else if (ctrlSettings[0].controlMode == 0){
                    for (int i =0; i<numSensors; i++){
                        ctrlSettings[i].controlMode = 1;
                        ctrlSettings[i].valveDirect = 0;
                    }
                    buttonMessage = "r"; 
                }
            }
        } break;

        case 2:{
            if (ctrlSettings[0].controlMode == 0){
                for (int i =0; i<numSensors; i++){
                    if (ctrlSettings[i].channelOn){
                        ctrlSettings[i].valveDirect = ctrlSettings[i].valveDirect +0.05;
                        //buttonMessage+=String(ctrlSettings[i].valveDirect,2)+" ";
                        buttonMessage= "Direct Valve Set\n" +String(ctrlSettings[i].valveDirect,2);
                    }
                } 
                //buttonMessage+= "Direct Valve Set\n" +String(ctrlSettings[0].valveDirect,2);
            }
        } break;

        case 3:{
            if (ctrlSettings[0].controlMode == 0){
                for (int i =0; i<numSensors; i++){
                    if (ctrlSettings[i].channelOn){
                        ctrlSettings[i].valveDirect = ctrlSettings[i].valveDirect -0.05;
                        //buttonMessage+=String(ctrlSettings[i].valveDirect,2)+" ";
                        buttonMessage= "Direct Valve Set\n" +String(ctrlSettings[i].valveDirect,2);
                    }
                } 
                //buttonMessage+= "Direct Valve Set\n" +String(ctrlSettings[0].valveDirect,2);
            }
        } break;
            
        default:{
        }
    
    }
    doFunction = 0;
    return buttonMessage;
}


bool handleButtons::checkForPress(Button *buttons){
    bool newState=0;
    for (int idx=0; idx<3; idx++){
      if (buttons[idx].toggled()) {
        newState = 1;
        if (buttons[idx].read() == Button::PRESSED){
          pressStart[idx] = millis();
        }
        else{
          pressEnd[idx]  = millis();
          doFunction = idx+1;
        }
      }
    }
    return newState;

}


bool handleButtons::checkForLongPress(int buttonNum){
    bool longPress=pressEnd[buttonNum]-pressStart[buttonNum] >= longPressDuration;
    return longPress;
}


