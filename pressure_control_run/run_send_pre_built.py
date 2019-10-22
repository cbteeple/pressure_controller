#!/usr/bin/env python

import serial
import time
from datetime import datetime
import sys
import os
import yaml
from pynput.keyboard import Key, Listener
from send_pre_built import TrajSend
from run_pre_built import PressureController

speedFactor=1.0
dataBack=True
saveData = True
traj_folder = "traj_built"
curr_flag_file = os.path.join("traj_built","last_sent.txt")

board_teensy = False;


restartFlag = False
exitFlag    = False

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())



  


if __name__ == '__main__':
    if 2<= len(sys.argv)<=4:

        if len(sys.argv)==4:
            wrap = int(sys.argv[3])
        else:
            wrap = False
            
        
        if len(sys.argv)==3:
            speedFact = 1.0/float(sys.argv[2])
        else:
            speedFact= 1.0
        
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
                    
                    if key == Key.ctrl:
                        exitFlag=True
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
            traj=TrajSend(serial_set.get("devname"), serial_set.get("baudrate"))
            traj.board_teensy=board_teensy
            traj.getTraj(sys.argv[1])

            traj.speedFactor = speedFact

            # Upload the trajectory and start it
            traj.sendTraj()
            traj.readStuff()



            # Create a pressure controller object
            pres=PressureController(traj.s, None, wrap)

            
            pres.startTraj()
            #pres.readStuff()
            while True:
                pres.readStuff()
                if restartFlag is True:
                    pres.startTraj()
                    restartFlag = False
                
                if exitFlag:
                    listener.stop()
                
        except KeyboardInterrupt:
            listener.stop()
            listener.stop()
            pres.shutdown()
            
    else:
        print("Please include the filename as the input argument")