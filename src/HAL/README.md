# HAL - Hardware Abstraction Layer

This folder contains the low level implementations of the hardware dependant ExtPack functionality for different microcontrollers/microcontroller families.

## Structure

Every controller / controller family has one source file.
The files are all protected by preprocessor guards which checks for the controllers.
This ensures that only the one low level implementation matching to the chosen microcontroller is used.

To use these functionality you have to forward declare these function as external.
This should only be used by features in the core layer.

## Functionality
Every microcontroller (family) has the following functionalities implemented:

### void init_ExtPack_LL()

This function is used to initialize the hardware component like timer and UART.
It is called once at startup.

### ext_pack_error_t send_UART_ExtPack_command(unit_t, uint8_t)

This function sends the given command pair to the ExtPack via UART.

### void enter_critical_zone()

This function is used to save the interrupt state and disables the interrupts.

### void exit_critical_zone()

This function resets the interrupt state to the saved one from enter_critical_zone().

### Interrupt driven receiving

Receives ExtPack commands (2 UART packages) and calls process_received_ExtPack_data(unit_t, uint8_t) with the received data.
You have to include ExtPack_Internal.h to access the function.

## Adding microcontrollers

To add a new microcontroller (family) you have to implement the functionalities above (with the same function names).
Additionally, you have to use a preprocessor guard to ensure your file will only by used when using your controller.  
Every controller is only allowed to be used in one low level file preprocessor guard.
