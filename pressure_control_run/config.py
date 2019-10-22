#!/usr/bin/env python

import serial
import time
import sys
import os
import yaml
import numbers

config_folder = "config"

# Read in data from the pressure controller (this seems not to work yet)
def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())

class configSender:
    def __init__(self, devname,baudrate):
        self.s = serial.Serial(devname,baudrate)
        self.config_folder  = config_folder

        time.sleep(2)
        self.s.write("off"+'\n')
        time.sleep(0.1)
        self.sendCommand("chan",1)
        time.sleep(0.1)
        self.sendCommand("mode",0)
        time.sleep(0.1)
        self.sendCommand("valve",-1)
        time.sleep(0.1)
        self.sendCommand("valve",-0.001)
        time.sleep(0.1)
        #self.s.write("load"+'\n')
        self.readStuff()
            

    def readConfig(self, filename):
        inFile=os.path.join(self.config_folder,filename+".yaml")

        if os.path.isfile(inFile):
            with open(inFile) as f:
                # use safe_load instead of load
                self.config = yaml.safe_load(f)
                f.close()
        else:
            self.config=None
            print("The config file does not exist")
            self.shutdown()
            


    def setConfig(self, filename):
        self.readConfig(filename)

        if self.config:
            self.sendCommand("echo",bool(self.config.get("echo")))
            time.sleep(0.1)
            self.readStuff()

            self.num_channels = self.config.get("channels").get("num_channels")
            time.sleep(0.1)
            self.readStuff()
            
            self.sendCommand("chan",self.config.get("channels").get("states"))
            time.sleep(0.1)
            self.readStuff()
            

            self.sendCommand("maxp", self.config.get("max_pressure") )
            time.sleep(0.1)
            self.readStuff()
            self.sendCommand("minp", self.config.get("min_pressure") )
            time.sleep(0.1)
            self.readStuff()
            
            self.handlePID()
            
            self.sendCommand("time",int(self.config.get("data_loop_time")))
            time.sleep(0.1)
            self.readStuff()

            self.sendCommand("valve",0)
            time.sleep(0.1)
            self.readStuff()
            
            self.sendCommand("mode",3)
            time.sleep(0.1)
            self.readStuff()
            
            self.sendCommand("save",[])
            time.sleep(0.1)
            self.readStuff()
            #self.sendCommand("save",[])



    def sendCommand(self, command, values):
        txt = command
        print("%s \t %s"%(command, values))
        if isinstance(values, list):
            if values:
                for val in values:
                    txt+= ";%0.5f"%(val)
        elif isinstance(values, numbers.Number):
            txt+=";%0.5f"%(values)
        else:
            raise ValueError('sendCommand expects either a list or a number')

        print(txt)
        self.s.write(txt +'\n')


    def handlePID(self):
        pid = self.config.get("PID")

        all_equal = pid.get("all_equal")
        if all_equal:
            values = []
            for idx in range(self.num_channels):
                values.append(pid.get("values"))
        else:
            values = pid.get("values")
            
        # Send out the settings for all channels
        for idx in range(self.num_channels):
            self.sendCommand("pid",[idx]+values[idx])
            time.sleep(0.1)
            self.readStuff()



    def shutdown(self):
        self.s.close()
        
    
    def readStuff(self):
        while self.s.in_waiting:  # Or: while ser.inWaiting():
            print self.s.readline().strip()
    


def get_serial_path():
    inFile=os.path.join("config","comms","serial_config.yaml")
    with open(inFile) as f:
        # use safe_load instead of load
        serial_set = yaml.safe_load(f)
        f.close()

    return serial_set
  


if __name__ == '__main__':
    if len(sys.argv)==2:
        serial_set = get_serial_path()
        
        print(serial_set)

        # Create a config object
        pres=configSender(serial_set.get("devname"), serial_set.get("baudrate"))

        # Upload the configuration and save it
        pres.setConfig(sys.argv[1])
        #pres.readStuff()
        #while True:
            #pres.readStuff()
            
        
        pres.shutdown()
    elif len(sys.argv)==1:
        serial_set = get_serial_path()

        print(serial_set)
        
        # Create a config object
        pres=configSender(serial_set.get("devname"), serial_set.get("baudrate"))

        # Upload the configuration and save it
        pres.setConfig('default')
        #pres.readStuff()
        #while True:
            #pres.readStuff()
            
        pres.shutdown()



    else:
        print("Please include a config file as the input")