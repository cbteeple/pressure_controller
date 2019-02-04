#!/usr/bin/env python

import yaml
import numpy as np
import scipy as sp
import scipy.interpolate
import sys
import os

trajFolder = "trajectories"


def calculateSegment(start_point,end_point,t_step):

    # Calculate the linear interpolation time vector
    t_intermediate = np.arange(start_point[0],end_point[0],t_step)
    print(t_step)
    print(t_intermediate)
    
    # Turn the incomming setpoints into arrays
    time_vec = np.asarray([end_point[0], start_point[0]])
    state_vec = np.transpose(np.asarray([end_point[1:], start_point[1:]]))
    
    # Create an interpolation function and use it
    fun = sp.interpolate.interp1d(time_vec,state_vec,fill_value="extrapolate")
    seg = np.transpose(fun(t_intermediate))
    
    # put the time back at the beginning of the array
    seg  = np.insert(seg,0,t_intermediate, axis=1)
    
    return seg





def main(filename):
    # Read in the setpoint file
    inFile=os.path.join(trajFolder,filename+".yaml")
    with open(inFile) as f:
        # use safe_load instead of load
        trajIn = yaml.safe_load(f)
        f.close()


    # Get data from the file
    traj_settings = trajIn.get("settings")
    traj_setpoints = trajIn.get("setpoints")
    t_step = traj_settings.get("time_increment")

    # Calculate the longer trajectory
    traj=np.empty([0,5])

    for idx in range(0,len(traj_setpoints)-1):
        seg = calculateSegment(traj_setpoints[idx],traj_setpoints[idx+1],t_step)
        traj = np.concatenate((traj,seg),axis=0)
    
    # Add the last entry to finish out the trajectory
    traj = np.concatenate((traj,np.asarray([traj_setpoints[-1]])),axis=0)
    
    # Save the trajectory
    outFile=os.path.join(trajFolder,filename+".traj")
    np.savetxt(outFile, traj, delimiter='\t', fmt='%0.4f') 


if __name__ == '__main__':
    if len(sys.argv)==2:
        main(sys.argv[1])
    else:
        print('make sure you give a filename')
        
        
        
        
        
        