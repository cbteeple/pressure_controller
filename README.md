# Ctrl-P 2.0: Arduino Firmware
The Arduino-based firmware for Ctrl-P pressure control systems.


## Dependencies
 - [EEPROMAnything Library](https://github.com/collin80/EEPROMAnything) by collin80
 - [Button Library](https://github.com/madleech/Button) by madleech
 - [EasyLCD Library](https://github.com/cbteeple/EasyLCD) by cbteeple (that's me!)
   - [LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) by fdebrabander
   

## Usage:
[Instructions in the documentation](https://ctrl-p.cbteeple.com/firmware)


## About Ctrl-P
The Ctrl-P project is a full-stack pneumatic control system featuring smooth control of pressure at a high bandwidth.

Ctrl-P has three parts:
- [Arduino-Based Firmware](https://github.com/cbteeple/pressure_controller): Contains the low-level control
- [Python Control Interface](https://github.com/cbteeple/pressure_control_interface): Controls pressure via serial comms
- [ROS Driver](https://github.com/cbteeple/pressure_control_cbt): Controls pressure via serial or RawUSB

Related Packages:
- [Pressure Controller Skills](https://github.com/cbteeple/pressure_controller_skills): Build complex parametric skills using straightforward definition files.
- [Visual Servoing](https://github.com/cbteeple/ihm_servoing): Example of setting up a realtime feedback controller.


## Updates from version 1.x

### Prefix/suffix support for pre-built trajectories
- separate trajectory into "prefix", "suffix", and "looping" parts.
- update firmware to take in start indices for each part + # of cycles (with -1 = inf cycles, 0 = skip loop, 1--N = # of cycles)
- add dynamic "# of cycles" function
- add dynamic speed multiplier function (with error checking to make sure we never divide by 0 or have traj. with 0 time)

