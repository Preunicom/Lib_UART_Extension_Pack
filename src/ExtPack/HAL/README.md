# HAL - Hardware Abstraction Layer

This folder contains the low level implementations of the hardware dependant ExtPack functionality for different microcontrollers/microcontroller families.

## Structure

Every controller has one source file.
If there is a controller family using the exact same code for more than one microcontroller there is one family source file. (ExtPack_LL_<gcc_mcu>.c)
The controllers in a family include the implementation source file of the family.
 
The Build system only compiles one of the controller source files.