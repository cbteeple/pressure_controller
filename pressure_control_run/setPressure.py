#!/usr/bin/env python

import serial
import time
import sys
import os
import yaml

speedFactor=1.0
wrap = True
dataBack=True
trajFolder = "trajectories"

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())

class PressureController:
    def __init__(self, devname,baudrate):
        self.s = serial.Serial(devname,baudrate)
        self.trajFolder  = trajFolder
        self.speedFactor = speedFactor

        time.sleep(1)

        self.s.write("echo;0"+'\n')
        self.s.write("time;100"+'\n')
        self.s.write("mode;1"+'\n')
        self.s.write("on"+'\n')
        #time.sleep(2)
            
    def setPressure(self, press):
        self.s.write("set;%0.3f"%(press)+'\n')


    def shutdown(self):
        self.s.write("off\n")
        self.s.write("set;0"+'\n')
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

            # Upload the trajectory and start it
            pres.setPressure(float(sys.argv[1]))
            #pres.readStuff()
            while True:
                pres.readStuff()
                
        except KeyboardInterrupt:
            pres.shutdown()
            
    else:
        print("Please include a pressure as the input")