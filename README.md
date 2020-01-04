# Ctrl-P 2.0
An object-oriented approach to doing pressure control. Hopefully things are modular and can be swapped in an out for different controllers and valve setups.

## Official Documentation
[Ctrl-P Documentation](https://cbteeple.github.io/pressure_controller_docs/)


## Dependencies

### Firmware
 - [EEPROMAnything Library](https://github.com/collin80/EEPROMAnything) by collin80
 - [Button Library](https://github.com/madleech/Button) by madleech
 - [EasyLCD Library](https://github.com/cbteeple/EasyLCD) by cbteeple (that's me!)
   - [LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) by fdebrabander
   
### Control Application
- Various python libraries:
	- [scipy](https://www.scipy.org/) (`pip install scipy`)
	- [numpy](https://www.numpy.org/) (`pip install numpy`)
	- [numbers](https://docs.python.org/2/library/numbers.html) (`pip install numbers`)
	- [matplotlib](https://matplotlib.org/) (`pip install matplotlib`)
	- [pynput](https://pypi.org/project/pynput/) (`pip install pynput`)
	- [yaml](https://pyyaml.org/wiki/PyYAMLDocumentation) (`pip install pyyaml`)

## How to use the code:
[Instructions in the documentation](https://cbteeple.github.io/pressure_controller_docs/)

## Updates from version 1.x

### Prefix/suffix support for pre-built trajectories

1. Firmware
	- separate trajectory into "prefix", "suffix", and "looping" parts.
	- update firmware to take in start indices for each part + # of cycles (with -1 = inf cycles, 0 = skip loop, 1--N = # of cycles)
	- add dynamic "# of cycles" function
	- add dynamic speed multiplier function (with error checking to make sure we never divide by 0 or have traj. with 0 time)

2. Python:
	- update buidtraj function to save prefix/suffix parts
	- update sendprebuilt function to send relevant parts + indices
	- update runprebuilt to send speed + # cycles.
