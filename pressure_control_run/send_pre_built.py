#!/usr/bin/env python

import serial
import time
import sys
import os
import yaml
from pynput.keyboard import Key, Listener

speedFactor=1.0
dataBack=True
saveData = True
traj_folder = "traj_built"
curr_flag_file = os.path.join("traj_built","last_sent.txt")

restartFlag = False

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())





class TrajSend:
    def __init__(self, devname,baudrate):
        self.s = serial.Serial(devname,baudrate)
        self.traj_folder  = traj_folder
        self.speedFactor = speedFactor
        self.saveData = saveData

        time.sleep(1)

        self.s.write("echo;0"+'\n')
        self.s.write("load"+'\n')
        #self.s.write("load"+'\n')
        self.s.write("set;0"+'\n')
        self.s.write("mode;2"+'\n')
        #s.write('on')

        time.sleep(0.5)

    # Read in the trajectory and store it in a list of arrays
    def getTraj(self,filename):
        self.filename = filename
        # Read in the setpoint file
        inFile=os.path.join(traj_folder,filename+".traj")
        with open(inFile,'r') as f:
            # use safe_load instead of load
            trajIn = yaml.safe_load(f)
            f.close()


        # Get data from the file
        #self.settings = trajIn.get("settings")
        self.traj = trajIn.get("setpoints")
        self.wrap = trajIn.get("wrap")

        self.createOutFile(filename)
        



    def sendTraj(self):
        lastTime = 0.0
        configstring = "trajconfig;%d;%d;%d" %(0,len(self.traj),self.wrap)
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
            

    def shutdown(self):
        self.s.write("mode;3"+'\n')
        self.s.write("set;0"+'\n')
        self.s.close()


        self.out_file = open(curr_flag_file, "w+")
        self.out_file.write(self.filename)
        self.out_file.close()
        
    
    def readStuff(self):
        if self.s.in_waiting:  # Or: while ser.inWaiting():
            line = self.s.readline().strip()
            print(line)
            if self.saveData:
                self.saveStuff(line)


    




  


if __name__ == '__main__':
    if 2<= len(sys.argv)<=3:

        if len(sys.argv)==3:
            speedFact = 1.0/float(sys.argv[2])
        else:
            speedFact= 1.0
        
        try:
            # Get the serial object to use
            inFile=os.path.join("config","comms","serial_config.yaml")
            with open(inFile) as f:
                # use safe_load instead of load
                serial_set = yaml.safe_load(f)
                f.close()

            # Create a pressure controller object
            pres=TrajSend(serial_set.get("devname"), serial_set.get("baudrate"))
            pres.getTraj(sys.argv[1])


            pres.speedFactor = speedFact

            # Upload the trajectory and start it
            pres.sendTraj()
            
            pres.readStuff()
            pres.shutdown()
            
                
        except KeyboardInterrupt:
            pres.shutdown()
            
    else:
        print("Please include the filename as the input argument")