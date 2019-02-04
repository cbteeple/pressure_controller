#!/usr/bin/env python

import serial
import time
import sys
import os

speedFactor=1.0
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

        self.s.write('load')
        self.s.write('time;100')
        self.s.write('set;0')
        #s.write('on')

        time.sleep(2)

    # Read in the trajectory and store it in a list of arrays
    def getTraj(self,filename):
        inFile=os.path.join(trajFolder,filename+".traj")
        with open(inFile,'r') as f:
            lines = f.readlines()
            f.close()

            self.traj = [[float(x) for x in line.split('\t')] for line in lines]

    def runTraj(self):
        lastTime = 0.0
        for entry in self.traj:
            # Send a string to the pressure controller
            string="set;%0.3f;%0.3f;%0.3f;%0.3f" %(
                self.speedFactor*entry[1],
                self.speedFactor*entry[2],
                self.speedFactor*entry[3],
                self.speedFactor*entry[4])
            print(string)
            self.s.write(string+'\n')

            # Sleep for a short time before the next send action
            time.sleep(entry[0]-lastTime)
            lastTime=entry[0]

    def shutdown(self):
        self.s.close()
    



  


if __name__ == '__main__':
    if len(sys.argv)==2:
        try:
            pres=PressureController('COM17',115200)
            pres.getTraj(sys.argv[1])

            # Loop the trajectory until you stop it
            while True:
                pres.runTraj()
                
        except KeyboardInterrupt:
            pres.shutdown()
            
    else:
        print("Please include the filename as the input argument")