#!/usr/bin/env python

import serial
import time
import sys
import os

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
        self.s.write("load"+'\n')
        self.s.write("time;100"+'\n')
        self.s.write("set;0"+'\n')
        self.s.write("mode;2"+'\n')
        #s.write('on')

        time.sleep(2)

    # Read in the trajectory and store it in a list of arrays
    def getTraj(self,filename):
        inFile=os.path.join(trajFolder,filename+".traj")
        with open(inFile,'r') as f:
            lines = f.readlines()
            f.close()

            self.traj = [[float(x) for x in line.split('\t')] for line in lines]

    def sendTraj(self):
        lastTime = 0.0
        configstring = "trajconfig;%d;%d;%d" %(0,len(self.traj),wrap)
        print(configstring)
        self.s.write(configstring+'\n')
        for idx, entry in enumerate(self.traj):
            # Send a string to the pressure controller
            string="trajset;%d;%0.3f;%0.3f;%0.3f;%0.3f;%0.3f" %(
                idx,
                self.speedFactor*entry[0],
                entry[1],
                entry[2],
                entry[3],
                entry[4])
            print(string)
            self.s.write(string+'\n')
            
            time.sleep(0.05)

            # Sleep for a short time before the next send action
            #time.sleep(entry[0]-lastTime)
            #lastTime=entry[0]
            
    def startTraj(self):
        self.s.write("trajstart"+'\n')
        if dataBack:
            self.s.write("on\n")

    def shutdown(self):
        self.s.write("off\n")
        self.s.write("trajstop"+'\n')
        self.s.write("mode;1"+'\n')
        self.s.write("set;0"+'\n')
        self.s.close()
        
    
    def readStuff(self):
        while self.s.in_waiting:  # Or: while ser.inWaiting():
            print self.s.readline().strip()
    



  


if __name__ == '__main__':
    if len(sys.argv)==2:
        try:
            pres=PressureController('COM17',115200)
            pres.getTraj(sys.argv[1])

            # Upload the trajectory and start it
            pres.sendTraj()
            
            pres.readStuff()
            pres.startTraj()
            #pres.readStuff()
            while True:
                pres.readStuff()
                
        except KeyboardInterrupt:
            pres.shutdown()
            
    else:
        print("Please include the filename as the input argument")