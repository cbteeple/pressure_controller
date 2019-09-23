#!/usr/bin/env python

import serial
import time
import sys
import os
import yaml
import pickle
import numbers

from pynput.keyboard import Key, KeyCode, Listener

speedFactor=1.0
wrap = True
dataBack=True
trajFolder = "trajectories"
press_set = []
new_set = False
restart=False

up_keys_char = ['q','w', 'e', 'r', 't', 'y']
down_keys_char = ['a','s', 'd', 'f', 'g', 'h']
number_pressures = [5, 10, 15, 20, 25]


up_keys = [KeyCode.from_char(key) for key in up_keys_char]
down_keys = [KeyCode.from_char(key) for key in down_keys_char]


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
        time.sleep(0.1)
        self.readStuff()

        self.sendCommand("set",0)
        time.sleep(0.1)
        self.readStuff()



    def sendCommand(self, command, values):
        txt = command

        if isinstance(values, list):
            if values:
                for val in values:
                    txt+= ";%0.3f"%(val)
        elif isinstance(values, numbers.Number):
            txt+=";%0.3f"%(values)
        else:
            raise ValueError('sendCommand expects either a list or a number')

        self.s.write(txt +'\n')



    def setPressure(self, press):
        self.sendCommand('set', press)


    def startPressMode(self):
        self.sendCommand("mode",1)

    def shutdown(self):
        self.s.write("set;0"+'\n')
        self.s.close()
        
    
    def readStuff(self):
        if self.s.in_waiting:  # Or: while ser.inWaiting():
            print(' ')
            print self.s.readline().strip()
    



def on_press(key):
    global press_set
    global new_set
    global restart

    indices_up = [i for i, x in enumerate(up_keys) if x == key]
    indices_down = [i for i, x in enumerate(down_keys) if x == key]


    try:
        if indices_up:
            if indices_up[0]<len(press_set):
                press_set[indices_up[0]] = press_set[indices_up[0]]+0.5;
                new_set = True

        elif indices_down:
            if indices_down[0]<len(press_set):
                press_set[indices_down[0]] = press_set[indices_down[0]]-0.5;
                new_set = True

        elif key == KeyCode.from_char('0'):
            press_set = [0 for i in press_set]
            new_set = True
        

        try:
            if 1<= int(key.char) <=5:
                press_set = [number_pressures[int(key.char)-1] for i in press_set]
                new_set = True

        except ValueError:
            pass



    except AttributeError:
        if key == Key.enter:
            press_set = [0 for i in press_set]
            new_set = True
            restart = True






def on_release(key):
    pass



 


if __name__ == '__main__':
    if len(sys.argv)>=1:

        if len(sys.argv)==1:
            press_set = [0,0,0,0]

        elif len(sys.argv)>1:
            for idx in range(len(sys.argv)-1):
                press_set.append(float(sys.argv[idx+1]))


        try:

            listener = Listener(
            on_press=on_press,
            on_release=on_release)
            
            

            # Get the serial object to use
            inFile=os.path.join("config","comms","serial_config.yaml")
            with open(inFile) as f:
                # use safe_load instead of load
                serial_set = yaml.safe_load(f)
                f.close()

            # Create a pressure controller object
            pres=PressureController(serial_set.get("devname"), serial_set.get("baudrate"))

            # Upload the pressure
            pres.setPressure(press_set)
            
            
            listener.start()

            while True:

                if new_set is True:
                    pres.setPressure(press_set)
                    new_set = False

                if restart:
                    pres.readStuff()
                    time.sleep(1.0)
                    pres.startPressMode()
                    restart=False
                    

                pres.readStuff()
                
        except KeyboardInterrupt:
            pres.shutdown()
            listener.stop()
    else:
        print("Please include a pressure as the input")