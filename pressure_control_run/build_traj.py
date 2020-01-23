#!/usr/bin/env python

import yaml
import numpy as np
import scipy as sp
import scipy.interpolate
from scipy.interpolate import CubicSpline
import scipy.signal as signal
import sys
import os
import matplotlib
import matplotlib.pyplot as plt
import numbers
import copy


font = {'family' : 'arial',
        'weight' : 'normal',
        'size'   : 14}	
matplotlib.rc('font', **font)
plt.clf()


traj_folder = "traj_setup"
out_folder  = "traj_built"




class trajBuilder:
    def __init__(self, filename, folder):
        self.filename = filename
        self.traj_folder = folder
        self.out_folder = out_folder
        self.getSettings()



    def getSettings(self):
        # Read in the setpoint file
        inFile=os.path.join(self.traj_folder,self.filename+".yaml")
        with open(inFile) as f:
            # use safe_load instead of load
            inStuff = yaml.safe_load(f)
            f.close()

        self.settings = inStuff.get("settings",None)
        self.config = inStuff.get("config",None)
        
        self.traj_type = str(self.settings.get("traj_type"))
        self.subsample_num = self.config.get("subsample_num")


    # Save yaml files of trajectories generated.
    def saveOut(self, outTraj, prefix=None, suffix=None):
        outDict = {}
        outDict['setpoints']= outTraj

        if prefix is not None:
            outDict['prefix'] = prefix

        if suffix is not None:
            outDict['suffix'] = suffix


        outFile=os.path.join(self.out_folder,self.filename+".traj")

        dirname = os.path.dirname(outFile)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        with open(outFile, 'w') as f:
            yaml.dump(outDict, f, default_flow_style=None)



    def go(self):
        if self.traj_type == "waveform":
            self.doWaveform()
        elif self.traj_type == "interp":
            self.doInterp()
        elif self.traj_type == "none":
            self.doNone()
        else:
            print('Please give your trajectory a valid type')




    def doWaveform(self):
        freq_in = self.config.get("waveform_freq")

        # Convert the frequency to floats
        if isinstance(freq_in, numbers.Number):
            freq = float(freq_in)
        elif isinstance(freq_in, list):
            freq = []
            for item in freq_in:
                freq.append(float(item))


        press_max = np.array(self.config.get("waveform_max"))
        press_min = np.array(self.config.get("waveform_min"))
        waveform_type = self.config.get("waveform_type")


        setpts = self.config.get("setpoints",None)
        prefix = setpts.get("prefix",None)
        suffix = setpts.get("suffix",None)


        channels =  self.config.get("channels")
        num_cycles = float(self.config.get("num_cycles"))

        press_amp = (press_max-press_min)/2.0 *channels
        press_off = (press_max+press_min)/2.0 * channels

        # Make the waveform
        traj = []
        if waveform_type == "square-sampled":
            time_samp = np.linspace(0,num_cycles/freq, self.subsample_num+1 )
            traj = signal.square(2.0*np.pi * freq*time_samp)

        elif waveform_type == "square":
            time_samp_0 = np.linspace(0, num_cycles/freq, num_cycles +1)

            time_samp_1 = np.linspace(1/freq -0.51/freq, num_cycles/freq - 0.51/freq, num_cycles )
            time_samp = np.append(time_samp_0, time_samp_1)

            time_samp_2 = np.linspace(1/freq -0.50/freq ,num_cycles/freq - 0.50/freq, num_cycles)
            time_samp = np.append(time_samp, time_samp_2)

            time_samp_3 = np.linspace(1/freq -0.01/freq ,num_cycles/freq - 0.01/freq, num_cycles)
            time_samp = np.append(time_samp, time_samp_3)

            time_samp = np.sort(time_samp)

            traj = np.array([-1,-1,1,1])
            traj = np.tile(traj,int(num_cycles))
            traj = np.append(traj, traj[0])


        elif waveform_type == "sin":
            time_samp = np.linspace(0,num_cycles/freq, self.subsample_num+1 )
            traj = np.sin(2.0*np.pi * freq*time_samp)

        elif waveform_type == "cos-up":
            time_samp = np.linspace(0,num_cycles/freq, self.subsample_num+1 )
            traj = np.cos(2.0*np.pi * freq*time_samp)

        elif waveform_type == "cos-down":
            time_samp = np.linspace(0,num_cycles/freq, self.subsample_num+1 )
            traj = -np.cos(2.0*np.pi * freq*time_samp)

        elif waveform_type == "triangle":
            time_samp = np.linspace(0,num_cycles/freq, num_cycles*2.0 +1)
            traj = np.array([-1,1])
            traj = np.tile(traj,int(num_cycles))
            traj = np.append(traj, traj[0])

        elif waveform_type == "sawtooth-f":
            time_samp_1 = np.linspace(0,num_cycles/freq, num_cycles +1)
            time_samp_2 = np.linspace(1/freq -0.01/freq ,num_cycles/freq - 0.01/freq, num_cycles)
            time_samp = np.sort(np.append(time_samp_1, time_samp_2))

            traj = np.array([-1,1])
            traj = np.tile(traj,int(num_cycles))
            traj = np.append(traj, traj[0])

        elif waveform_type == "sawtooth-r":
            time_samp_1 = np.linspace(0,num_cycles/freq, num_cycles +1)
            time_samp_2 = np.linspace(1/freq -0.99/freq ,num_cycles/freq - 0.99/freq, num_cycles)
            time_samp = np.sort(np.append(time_samp_1, time_samp_2))

            traj = np.array([-1,1])
            traj = np.tile(traj,int(num_cycles))
            traj = np.append(traj, traj[0])

        else:
            return


        #Stick everything together
        out_times = np.vstack(time_samp)
        out_traj  = np.matmul(np.asmatrix(traj).T,np.asmatrix(press_amp))
        press_off_all=np.tile(press_off,(len(traj),1))
        out_traj = press_off_all +out_traj


        out_traj_whole = np.append(out_times,out_traj,axis=1)
        
        self.plotTraj(out_traj_whole,prefix,suffix)

        self.saveOut(out_traj_whole.tolist(), prefix, suffix)

        print("Trajectory has %d lines"%(out_traj_whole.shape[0]))

        

    def doInterp(self):
        setpts = self.config.get("setpoints",None)
        traj_setpoints = setpts.get("main",  None)
        prefix = setpts.get("prefix",None)
        suffix = setpts.get("suffix",None)
        interp_type = str(self.config.get("interp_type"))


        if interp_type == "linear":
            t_step = (traj_setpoints[-1][0]-traj_setpoints[0][0])/self.subsample_num
            # Calculate the longer trajectory
            allOut=[]
            for idx in range(0,len(traj_setpoints)-1):
                seg = self.calculateLinSegment(traj_setpoints[idx],traj_setpoints[idx+1],t_step)
                allOut.extend(seg)
            
            # Add the last entry to finish out the trajectory
            allOut.append(traj_setpoints[-1])


        

        elif interp_type == "cubic":
            t_step = (traj_setpoints[-1][0]-traj_setpoints[0][0])/self.subsample_num
            traj_setpoints = np.array(traj_setpoints)
            times=traj_setpoints[:,0]
            pres=traj_setpoints[:,1:]

            # Replace nearest points with the original knot points
            t_intermediate = np.linspace(times[0],times[-1], self.subsample_num+1 )
            idx = self.find_nearest(t_intermediate, times)
            t_intermediate[idx] = times

            # Generate a cubic spline
            cs = CubicSpline(times, pres, bc_type = "periodic")
            traj = cs(t_intermediate)
            allOut = np.insert(traj,0, t_intermediate ,axis = 1)
            allOut = allOut.tolist()

            plt.plot(times,pres,'o')
            plt.plot(t_intermediate,traj)
            plt.show()

        else:
            allOut = traj_setpoints
            

        self.plotTraj(allOut,prefix,suffix)

        self.saveOut(allOut, prefix, suffix)

        print("Trajectory has %d lines"%(len(allOut)))



    def doNone(self):
        pass


    def find_nearest(self, array, values):
        array = np.asarray(array)
        idx=[]
        for val in values:
            idx.append(np.argmin(np.abs(array - val)))
        return idx


    def calculateLinSegment(self,start_point,end_point,t_step):

        # Calculate the linear interpolation time vector
        t_intermediate = np.arange(start_point[0],end_point[0],t_step)
        #print(t_step)
        #print(t_intermediate)
        
        # Turn the incomming setpoints into arrays
        time_vec = np.asarray([end_point[0], start_point[0]])
        state_vec = np.transpose(np.asarray([end_point[1:], start_point[1:]]))
        
        # Create an interpolation function and use it
        fun = sp.interpolate.interp1d(time_vec,state_vec,fill_value="extrapolate")
        seg = np.transpose(fun(t_intermediate))
        
        # put the time back at the beginning of the array
        seg  = np.insert(seg,0,t_intermediate, axis=1)
        
        return seg.tolist()



    def plotTraj(self, main_traj, prefix=None, suffix=None ):
        out_traj_all = np.asarray(main_traj)

        if prefix is not None:
            prefix_arr = np.asarray(prefix)
            # Update the times
            out_traj_all[:,0] = out_traj_all[:,0] + prefix_arr[-1,0]

            # Append to the array
            out_traj_all = np.append(prefix_arr,out_traj_all,axis=0);

        if suffix is not None:
            suffix_arr = np.asarray(suffix)        
            suffix_arr[:,0] = suffix_arr[:,0] + out_traj_all[-1,0]
            out_traj_all = np.append(out_traj_all,suffix_arr,axis=0);

        plt.plot(out_traj_all[:,0],out_traj_all[:,1:])
        plt.xlabel("Time (s)")
        plt.ylabel("Pressure (psi)")
        plt.show()

        


if __name__ == '__main__':
    if len(sys.argv)==2:

        build = trajBuilder(sys.argv[1], traj_folder)
        build.go()

    else:
        print('make sure you give a filename')
        
        
        
        
        
        