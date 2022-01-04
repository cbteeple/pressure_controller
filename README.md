# Ctrl-P 2.0: Arduino Firmware
The Arduino-based firmware for Ctrl-P pressure control systems.

## Versions
You can always keep up to date by using the version in the master branch. For previous stable versions, checkout the [releases](https://github.com/cbteeple/pressure_controller/releases).

## Dependencies
 - [EEPROMAnything Library](https://github.com/collin80/EEPROMAnything) by collin80
 - [Button Library](https://github.com/madleech/Button) by madleech
 - [EasyLCD Library](https://github.com/cbteeple/EasyLCD) by cbteeple (that's me!)
   - [LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) by fdebrabander
   

## Usage:
[Instructions in the documentation](https://ctrl-p.cbteeple.com/firmware)


## About Ctrl-P
The Ctrl-P project is a full-stack pneumatic control system featuring smooth control of pressure at a high bandwidth.

Ctrl-P has four parts:
- [Arduino-Based Firmware](https://github.com/cbteeple/pressure_controller): Contains the low-level pressure control
- [Python Hardware Interface](https://github.com/cbteeple/ctrlp): The low-level device drivers and command handling via serial comms
- [Python Control Interface](https://github.com/cbteeple/pressure_control_interface): High-level handling of pressure trajectories in raw python
- [ROS Driver](https://github.com/cbteeple/pressure_control_cbt): High-level handling of pressure trajectories in ROS

Related Packages:
- [Pressure Controller Skills](https://github.com/cbteeple/pressure_controller_skills): Build complex parametric skills using straightforward definition files.
- [Visual Servoing](https://github.com/cbteeple/ihm_servoing): Example of setting up a realtime feedback controller.


## Updates from version 1.x

### Prefix/suffix support for pre-built trajectories
- separate trajectory into "prefix", "suffix", and "looping" parts.
- update firmware to take in start indices for each part + # of cycles (with -1 = inf cycles, 0 = skip loop, 1--N = # of cycles)
- add dynamic "# of cycles" function
- add dynamic speed multiplier function (with error checking to make sure we never divide by 0 or have traj. with 0 time)

