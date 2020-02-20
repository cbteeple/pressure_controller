#!/usr/bin/env python

import serial
import time
import sys
import os
import yaml


traj_folder = "traj_built"
curr_flag_file = os.path.join("traj_built","last_sent.txt")

restartFlag = False
board_teensy= True

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())





class TrajSend:
    def __init__(self, devname,baudrate):
        self.s = serial.Serial(devname,baudrate)
        self.traj_folder  = traj_folder
        self.send_wait = 0.05;
        self.board_teensy= board_teensy

        time.sleep(0.5)

        self.s.write("echo;1"+'\n')
        #self.s.write("load"+'\n')
        #self.s.write("set;0;0"+'\n')
        #self.s.write("mode;3"+'\n')
        #s.write('on')

        
        time.sleep(0.5)
        for i in range(50):
            self.readStuff()
            #time.sleep(0.05)

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
        self.prefix = trajIn.get("prefix",None)
        self.suffix = trajIn.get("suffix",None)
        self.wrap = trajIn.get("wrap",False)
        


    def writeTrajOut(self, traj, traj_type="main"):
        num_channels=len(traj[0])
        
        if traj_type is "prefix":
            cmd_type="prefset";
        elif traj_type is "suffix":
            cmd_type="suffset";
        else:
            cmd_type="trajset";
                
        for idx, entry in enumerate(traj):
            # Send a string to the pressure controller
                                   
            string=(cmd_type+";%d;%0.3f")%(idx, entry[0]);
            for i in range(num_channels-1):
                string+=";%0.3f" %(entry[i+1])
            print(string)
            self.s.write(string+'\n')
            
            time.sleep(0.05)
            
            if not self.board_teensy:
                time.sleep(0.5)
                for i in range(3):
                    self.readStuff()
                    time.sleep(self.send_wait)


    def sendTraj(self):
        lastTime = 0.0
        pre_len=0
        suf_len=0
        if self.prefix is not None:
            pre_len = len(self.prefix)
        if self.suffix is not None:
            suf_len = len(self.suffix)
            
        configstring = "trajconfig;%d;%d;%d;%d" %(pre_len,len(self.traj),suf_len,1.0)
        print(configstring)
        self.s.write(configstring+'\n')
        for i in range(3):
            self.readStuff()
            time.sleep(0.05)
        self.writeTrajOut(self.traj,   "main")
        if self.prefix is not None:
            self.writeTrajOut(self.prefix, "prefix")
            
        if self.suffix is not None:
            self.writeTrajOut(self.suffix, "suffix")
        self.s.write("trajset"+'\n')

            # Sleep for a short time before the next send action
            #time.sleep(entry[0]-lastTime)
            #lastTime=entry[0]
            
            
        dirname = os.path.dirname(curr_flag_file)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        self.out_file = open(curr_flag_file, "w+")
        self.out_file.write(self.filename)
        self.out_file.close()
            

    def shutdown(self):
        #self.s.write("mode;3"+'\n')
        #self.s.write("set;0;0"+'\n')
        self.s.close()
        
    
    def readStuff(self):
        if self.s.in_waiting:  # Or: while ser.inWaiting():
            line = self.s.readline().strip()
            print(line)


    




  


if __name__ == '__main__':
    if 2<= len(sys.argv)<=2:

        
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



            # Upload the trajectory and start it
            pres.sendTraj()
            
            for i in range(50):
                pres.readStuff()
            pres.shutdown()
            
                
        except KeyboardInterrupt:
            pres.shutdown()
            
    else:
        print("Please include the filename as the input argument")