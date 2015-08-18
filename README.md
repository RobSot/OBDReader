# OBDReader
OBD to USB interface development

The purpose of this development is to bring an open source OBD-USB interface to the comunity. Whith it you can develop your own applications above it. 
It has CAN connectivity via the OBD port and USB serial conectivity via the USB port.

*** The source code is organized like this:

1.- The folder src/Hardware_Source_code/ contains two folders. Both are MplabX 2.26 projects, one of them is the firmware and the other is the utility car emulator.
2.- The folder src/Software_Source_code/ contains one folder. This is the Qt 5 project and is the GUI application.
3.- In the src/ folder exist an .hex file containing the latest version of the OBDReader firmware ready to flash on the device.


Hope you enjoy it!!