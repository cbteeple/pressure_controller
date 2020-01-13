#!/usr/bin/env python

import serial
import time
from datetime import datetime
import sys
import os
import yaml
from pynput.keyboard import Key, Listener

dataBack=True
saveData = True
traj_folder = "traj_built"
curr_flag_file = os.path.join("traj_built","last_sent.txt")


restartFlag = False

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())





class PressureController:
    def __init__(self, devname,baudrate,cycles=1,speedFactor=1.0):
        
        if baudrate is None:
            self.s = devname
        else:
            self.s = serial.Serial(devname,baudrate)
            
        self.traj_folder  = traj_folder
        self.speedFactor = speedFactor
        self.cycles      = cycles
        self.saveData = saveData

        time.sleep(1)




        with open(curr_flag_file,'r') as f:
            # use safe_load instead of load
            outfile = f.read()
            f.close()

        self.createOutFile(outfile)
      
      
    def initialize(self):
        self.s.write("echo;0"+'\n')
        #self.s.write("load"+'\n')
        self.s.write("set;0;0"+'\n')
        self.s.write("trajloop;%d"%(self.cycles)+'\n')
        self.s.write("trajspeed;%d"%(self.speedFactor)+'\n')
        self.s.write("mode;2"+'\n')
        #s.write('on')

        time.sleep(2.5)


    def createOutFile(self,filename):
        outFile=os.path.join('data',filename)

        dirname = os.path.dirname(outFile)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        
        i = 0
        while os.path.exists("%s_%04d.txt" % (outFile,i) ):
            i += 1

        self.out_file = open("%s_%04d.txt" % (outFile,i), "w+")




            
    def startTraj(self):
        if dataBack:
            self.s.write("on\n")
            
        self.s.write("trajstart"+'\n')
        

    def shutdown(self):
        self.s.write("off\n")
        self.s.write("trajstop"+'\n')
        #self.s.write("mode;3"+'\n')
        #self.s.write("set;0;0"+'\n')
        self.s.close()

        self.out_file.close()
        
    
    def readStuff(self):
        if self.s.in_waiting:  # Or: while ser.inWaiting():
            line = self.s.readline().strip()
            print(line)
            if self.saveData:
                self.saveStuff(line)


    def saveStuff(self, line):
        self.out_file.write(line+'\n')
    





  


if __name__ == '__main__':
    if 1<= len(sys.argv)<=3:
        
        if len(sys.argv)==3:
            speedFact = float(sys.argv[2])
        else:
            speedFact= 1.0

        if len(sys.argv)>=2:
            cycles = int(sys.argv[1])
        else:
            cycles = 1
        
        print(cycles)
        print(speedFact)
        
        try:
            
            def on_press(key):
                try:
                    pass
                except KeyboardInterrupt:
                    raise
            
            
            def on_release(key):
                try:
                    global restartFlag
                    global exitFlag
                    if key == Key.space:
                        print('Restart Trajectory')
                        restartFlag =True
                        print('{0} released'.format( key))
                        print('_RESTART')
                        restartFlag =True
                    
                    if key == Key.esc:
                        exitFlag=True
                        # Stop listener
                        return False
                        
                except KeyboardInterrupt:
                    raise
            
            
            
            listener = Listener(
                on_press=on_press,
                on_release=on_release)
            listener.start()




            # Get the serial object to use
            inFile=os.path.join("config","comms","serial_config.yaml")
            with open(inFile) as f:
                # use safe_load instead of load
                serial_set = yaml.safe_load(f)
                f.close()

            # Create a pressure controller object
            pres=PressureController(serial_set.get("devname"), serial_set.get("baudrate"), cycles,speedFact)

            pres.initialize();
            
            pres.startTraj()
            #pres.readStuff()
            while True:
                pres.readStuff()
                if restartFlag is True:
                    pres.startTraj()
                    restartFlag = False
                
        except KeyboardInterrupt:
            listener.stop()
            listener.stop()
            pres.shutdown()
            
    else:
        print("Make sure you sent the right number of arguments")