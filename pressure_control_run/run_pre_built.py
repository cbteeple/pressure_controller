#!/usr/bin/env python

import serial
import time
from datetime import datetime
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





class PressureController:
    def __init__(self, devname,baudrate,wrap):
        self.s = serial.Serial(devname,baudrate)
        self.traj_folder  = traj_folder
        self.speedFactor = speedFactor
        self.saveData = saveData

        time.sleep(1)

        self.s.write("echo;0"+'\n')
        #self.s.write("load"+'\n')
        self.s.write("set;0"+'\n')
        self.s.write("trajwrap;%d"%(wrap)+'\n')
        self.s.write("mode;2"+'\n')
        #s.write('on')

        time.sleep(0.5)


        with open(curr_flag_file,'r') as f:
            # use safe_load instead of load
            outfile = f.read()
            f.close()

        self.createOutFile(outfile)
      
        


    def createOutFile(self,filename):
        outFile=os.path.join('data',filename)

        dirname = os.path.dirname(outFile)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        
        i = 0
        while os.path.exists("%s_%s.txt" % (outFile,i) ):
            i += 1

        self.out_file = open("%s_%s.txt" % (outFile,i), "w+")




            
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

        self.out_file.close()
        
    
    def readStuff(self):
        if self.s.in_waiting:  # Or: while ser.inWaiting():
            line = self.s.readline().strip()
            print(line)
            if self.saveData:
                self.saveStuff(line)


    def saveStuff(self, line):
        self.out_file.write(line+'\n')
    






def on_press(key):
    try:
        print('alphanumeric key {0} pressed'.format(
            key.char))
    except AttributeError:
        print('special key {0} pressed'.format(
            key))
    pass


def on_release(key):
    global restartFlag
    if key == Key.space:
        print('Restart Trajectory')
        restartFlag =True
        print('{0} released'.format( key))
        print('_RESTART')
        restartFlag =True


listener = Listener(
    on_press=on_press,
    on_release=on_release)
listener.start()




  


if __name__ == '__main__':
    if 1<= len(sys.argv)<=2:

        if len(sys.argv)==2:
            wrap = int(sys.argv[1])
        else:
            wrap = False
        
        try:



            # Get the serial object to use
            inFile=os.path.join("config","comms","serial_config.yaml")
            with open(inFile) as f:
                # use safe_load instead of load
                serial_set = yaml.safe_load(f)
                f.close()

            # Create a pressure controller object
            pres=PressureController(serial_set.get("devname"), serial_set.get("baudrate"), wrap)

            
            pres.startTraj()
            #pres.readStuff()
            while True:
                pres.readStuff()
                if restartFlag is True:
                    pres.startTraj()
                    restartFlag = False
                
        except KeyboardInterrupt:
            listener.stop()
            pres.shutdown()
            
    else:
        print("Please include the filename as the input argument")