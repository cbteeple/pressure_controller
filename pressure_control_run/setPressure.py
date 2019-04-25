#!/usr/bin/env python

import serial
import time
import sys
import os
import yaml
import pickle

speedFactor=1.0
wrap = True
dataBack=True
trajFolder = "trajectories"

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())


def portIsUsable(portName):
    try:
       ser = serial.Serial(port=portName)
       return True
    except:
       return False


def seriInit(devname,baudrate):
    try:
        ser= serial.Serial(devname,baudrate, timeout=1)
        ser.isOpen() # try to open port, if possible print message and proceed with 'while True:'

    except IOError: # if port is already opened, close it and open it again and print message
        #global ser
        print ("Port was already open. Unplug + replug the arduino and try again.")
        ser = None
    return ser



class PressureController:
    def __init__(self, devname,baudrate):
        self.s = seriInit(devname,baudrate)
        
        if self.s is None:
            sys.exit()
        
        self.trajFolder  = trajFolder
        self.speedFactor = speedFactor


        time.sleep(1)
        

        self.s.write("mode;1"+'\n')

            
    def setPressure(self, press):
        self.s.write("set;%0.3f"%(press)+'\n')


    def shutdown(self):
        #self.s.write("set;0"+'\n')
        self.s.close()
        
    
    def readStuff(self):
        while self.s.in_waiting:  # Or: while ser.inWaiting():
            print self.s.readline().strip()
    



  


if __name__ == '__main__':
    if len(sys.argv)==2:
        try:
            inFile=os.path.join("config","serial_config.yaml")
            with open(inFile) as f:
                # use safe_load instead of load
                serial_set = yaml.safe_load(f)
                f.close()

            # Create a pressure controller object
            pres=PressureController(serial_set.get("devname"), serial_set.get("baudrate"))

            if str(sys.argv[1]) is 's':
                pres.shutdown()
            else:
                # Upload the trajectory and start it
                pres.setPressure(float(sys.argv[1]))
            
            pres.shutdown()
            #pres.readStuff()
            #while True:
            #    pres.readStuff()
                
        except KeyboardInterrupt:
            pass
            #pres.shutdown()
    else:
        print("Please include a pressure as the input")