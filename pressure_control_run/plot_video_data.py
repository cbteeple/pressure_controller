#!/usr/bin/env python

import numpy as np
import sys
import os
import matplotlib
import matplotlib.pyplot as plt
import pickle

from matplotlib import rcParams
rcParams['font.family'] = 'sans-serif'
rcParams['font.size'] = 18
rcParams['font.sans-serif'] = ['Arial']

data_folder_base  = "data"



class DataPlotter:
    def __init__(self, filename, folder):
        self.filename = filename
        self.data_folder = folder


    # Read in the tracking data
    def get_tracking_data(self):
        # Read in the setpoint file
        inFile_A=os.path.join(data_folder_base,self.data_folder,self.filename+"_A.dat")
        inFile_B=os.path.join(data_folder_base,self.data_folder,self.filename+"_B.dat")


        with open(inFile_A) as f:
            # use safe_load instead of load
            inStuff_A = np.loadtxt(f, skiprows=2)
            f.close()

        with open(inFile_B) as f:
            # use safe_load instead of load
            inStuff_B = np.loadtxt(f, skiprows=2)
            f.close()

        time_A = inStuff_A[:,0]
        time_B = inStuff_B[:,0]

        # Make both sets of data start at the same time
        if time_A[0] > time_B[0]:
            inStuff_B = inStuff_B[time_B >= time_A[0]]
        else:
            inStuff_A = inStuff_A[time_A >= time_B[0]]

        # Make both sets of data end at the same time
        if time_A[-1] > time_B[-1]:
            inStuff_A = inStuff_A[time_A <= time_B[-1]]
        else:
            inStuff_B = inStuff_B[time_B <= time_A[-1]]

        self.all_data = dict()
        self.all_data['time'] = inStuff_A[:,0]
        self.all_data['pos_a'] = inStuff_A[:,1:3]
        self.all_data['pos_b'] = inStuff_B[:,1:3]


    # Calculate the object angle from two points
    def calculate_angle(self):
        diff = self.all_data['pos_a'] - self.all_data['pos_b']
        angle_raw = np.arctan2(diff[:,0], diff[:,1])
        angle_rad = np.unwrap(angle_raw, discont=np.pi/2)
        angle = np.rad2deg(angle_rad)

        self.all_data['angle'] = angle - angle[0]


    # Pickle the data for future use.
    def pickle(self):
        out_file = os.path.join(data_folder_base,
                self.data_folder,
                self.filename+'_calc.pkl')

        dirname = os.path.dirname(out_file)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        with open(out_file, 'w') as f:
            pickle.dump(self.all_data, f)


    # Plot the data and save it if nessecary
    def plot(self, save=False):
        self.fig=plt.figure(1, figsize=(12.80, 7.20), dpi=150)
        plt.clf()
        self.ax = self.fig.add_subplot(1, 1, 1)

        self.ax.plot(
            self.all_data['time'],
            self.all_data['angle'], 
            color='b',lineWidth=2.0, lineStyle='-')


        plt.xlabel("Time (s)")
        plt.ylabel("Object Angle (deg.)")

        if save:
            plt.savefig(os.path.join(data_folder_base,
                self.data_folder,
                self.filename+'_angle.svg'))
            plt.savefig(os.path.join(data_folder_base,
                self.data_folder,
                self.filename+'_angle.png'))

        plt.show()


        


if __name__ == '__main__':
    if 3 <= len(sys.argv) <= 4:
        dp = DataPlotter(sys.argv[2], sys.argv[1])      
        dp.get_tracking_data()
        dp.calculate_angle()
        dp.plot(save=True)

        if len(sys.argv)==4:
            if str(sys.argv[3]).lower() in ("save", "pickle", "-s", "-p") :
                dp.pickle()

    else:
        print('make sure you give a filename')
        
        
        
        
        
        