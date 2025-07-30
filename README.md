# Lib_UART_Extension_Pack

## Building

The Makefile is tested on macOS Sequoia (15.5).  
It builds the static library `libExtPack.a` in the `build/` folder.  
To build it the Makefile has to be called in a specific way:  
`make MCU_AVR_GCC=atxxxxYYY F_CPU=YYYYYYYUL [DEFINES="[USED_UNITS=Y] [SEND_BUF_LEN=Y]"] [V=1]`  
Where x stands for a string, Y for a number and [...] means optional.
The numbers (Y) and strings (x) can have another width than the pattern.
The V flags stands for verbose and if set it will show all commands.  
The Makefile uses `lto` to build the library. 
It is therefore recommended to use the flag also to link the library.  
**Note**: Make sure the library is built with the correct parameters named above matching your specific project.
**Note**: If you are using a not unix based system you have to edit the `MKDIR_P ?=` and `RM_RF ?=`to something working on your system.

## Build examples

To build the examples you can use the `examples` target of the makefile:  
`make examples MCU_AVR_GCC=atxxxxYYY F_CPU=YYYYYYYUL [DEFINES="[USED_UNITS=Y] [SEND_BUF_LEN=Y]"] [V=1]`  
This creates a hex file for every example in the `build/examples` folder.
This hex file can be flashed on the controller via for example avrdude.

## Communication

The microcontroller communicates with the Extension_Pack with 8N1 and 1 MBaud UART.

## Functionality
### Units

This library supports Reset_Unit, Error_Unit, ACK_Unit, UART_Unit, GPIO_Uni, Timer_Unit, SPI_Unit and I2C_Unit on the ExtPack.  

### Functions

#### Transmitting

- Reset ExtPack
- Enable/Disable ACK
- Setting GPIO output values
- Request GPIO input pin data
- Configure timer (enable, restart, prescaler, start value)
- Sending UART data
- Sending SPI data
- Setting SPI slave id
- Sending I2C data
- Setting I2C partner address

#### Receiving

- All units have events which are set if the unit received something
- custom ISRs for all units can be implemented (act like interrupted from the unit itself)
- GPIO has an extra feature with saving the last in- and output values to have current values accessible
- SPI also stores the last set slave id to be able to minimize control message overhead
- I2C stores the last set partner address and the last received data byte.
- Acknowledge management of ACK_Unit (wait for ACK, ACK event set/clear, read ACK data)

## Usage

### Initialisation

1) Initialize ExtPack with __init_ExtPack(reset_ISR, error_ISR, ack_ISR)__
2) Initialize all used units except reset, error and ack units (they are already initialized in __init_ExtPack__()) with 
   - unit number (__unit_UXX__)
   - unit type (__UART_Unit__, __GPIO_Unit__ or __Timer_Unit__) and
   - your custom ISR (of type __void (*func)(unit_t, char)__)

**NOTE:** You are able to define your amount of used units used by the ExtPack to minimize memory usage by adding the compiler flag:
`-DUSED_UNITS=<Amount>`  
**NOTE:** You are able to set the size of the UART send ringbuffer by setting the compiler flag:
`-DSEND_BUF_LEN=<Amount commands>`  
The default value depends on the used microcontroller.  
You are also able to deactivate the whole ring buffer by setting the size to 0.
This will reduce the used memory for the library.

### Reset_Unit

#### reset_ExtPack()

Resets the ExtPack.
**WARNING**: Be cautious when resetting the microcontroller in the custom ISR of the reset unit combined with
resetting the ExtPack when initializing the microcontroller. This leads to an endless reset loop.  
You can avoid this by setting the custom ISR to "NULL" initially and setting it after resetting the ExtPack at startup.
You can use "set_ExtPack_custom_ISR()" for it.

### Error_Unit

This unit receives errors of ExtPack.
The ExtPack sends status messages about errors.
The exact interpretation of bits in the received error data can be found in the ExtPack documentation:
***
The message structure is shown in the following:
- Bit 0: Indicates a UART error when receiving UART data from the host.
- Bit 1: Indicates an error of any unit while sending data to host. (for example because of too slow scheduling)
- Bit 2: Indicates an error of any unit while processing data from the host. (for example when the UART Unit can not send data as the unit still processes the last data)
  All other bits are zero.
***
If you wish to process the received data in detail you have to use this information and process it.

### ACK_Unit

#### set_ExtPack_ACK_enable()

Sends a enable/disable message.  
**Note:** This message is acknowledged by the ExtPack.

#### get_ExtPack_ack_state() & get_ExtPack_ack_event() & clear_ExtPack_ack_event()

This functions return the state or the event of ExtPack ACK unit.
It can be 0 (not active/event not set) or 1 (active/event set).

#### get_ExtPack_ack_data()

Returns the data which was acknowledged by the ExtPack.

#### wait_for_ExtPack_ACK_data() & wait_for_ExtPack_ACK()

These functions blocks until either the timout is reached or an acknowledgement is received.  
They return EXT_PACK_SUCCESS or EXT_PACK_FAILURE.  
The difference between the two functions is that the data function also checks the data of the ACK.
The other one takes every ACK for the given unit.  
**Note**: The ACK_unit ISR is executed _before_ the wait_for_ExtPack_ACK() processes the acknowledgement.  
**Note**: Only one function can use "get_ExtPack_ack_event()" per ACK event because it resets the event.
Both functions use "get_ExtPack_ack_event()" and can therefore not be combined.  
**WARNING**: Do not send data in the ACK ISR as is will result in an endless loop of sending
because of an ACK and getting ACKs for it which trigger sending again.

The unit_data of ACK unit is special:  
output_values:
- Bit 0: ACK state (0: not enabled / 1: enabled)
- Bit 1-6: Unused
- Bit 7: ACK received event (0: not set / 1: set)

input_values:
- Bit 0-7: data of last received acknowledgment

### GPIO_Unit

#### get_ExtPack_data_gpio_in() / get_ExtPack_data_gpio_out()

These functions do not communicate with the ExtPack.  
They take the last received/sent data.  
To refresh this data use __refresh_ExtPack_gpio_data__.

#### refresh_ExtPack_gpio_data()

Requests the current input data of the GPIO_Unit from the ExtPack.

#### set_ExtPack_gio_out()

Sets new GPIO output values at the ExtPack.

### Communication Units (UART_Unit, SPI_Unit, I2C_Unit)

#### send_ExtPack_UART_data() & send_ExtPack_SPI_data() & send_ExtPack_I2C_data()

Sends a single byte of data to the specified communication unit (UART, SPI, or I2C) of the Extension Pack.

#### send_ExtPack_UART_String() & send_ExtPack_SPI_String() & send_ExtPack_I2C_String()

Sends the String to a UART/SPI unit of ExtPack without the trailing '\0'.  
Make sure the string ends with a '\0'!  
The delay between two chars is given in us and can not exceed 65535.

#### set_ExtPack_SPI_slave() & set_ExtPack_I2C_partner_adr()

Configures the SPI unit’s slave ID or the I2C unit’s partner address for subsequent communication with the Extension Pack.

#### get_ExtPack_data_SPI_current_slave() & get_ExtPack_data_I2C_current_partner_adr()

This function does not communicate with the ExtPack.
They take the last sent slave id as the SPI/I2C Unit stores it for further messages.
**WARNING:** If there is an error while sending the slave id/partner address (for example it does not reach the ExtPack or is invalid)
it will be stored either. The slave id/partner address has to be set again.

#### send_ExtPack_SPI_String_to_slave() & send_ExtPack_I2C_String_to_partner()

Selects the specified SPI slave or I2C partner and sends a string without the trailing \0 to it by sennding it byte by byte. 
The string must include terminating '\0'!

#### send_ExtPack_SPI_data_to_slave() & send_ExtPack_I2C_data_to_partner()

Sets the target SPI slave or I2C partner and transmits a single byte of data to it as part of one combined command sequence.

#### get_ExtPack_data_I2C_last_received_data()

Retrieves the last received data byte stored by the I2C unit.
This function does not initiate a new transfer.

#### receive_ExtPack_I2C_data()

Requests one byte of data from the currently set I2C partner.
The received byte will be returned via the custom ISR and stored internally.

#### receive_ExtPack_I2C_data_from_partner()

Sets the I2C partner address and requests a single data byte.
The result is provided through the custom ISR and stored internally.

#### Notes: I2C

The last received byte is stored to be able to processed in the main program flow.
To archive multiple bytes in one transaction without repeated start the bytes have to be sent with the same access mode (send/receive)
to the same partner address. Additionally, the following byte has to be sent while the one before is being transmitted by the ExtPack.
With more than two bytes there has to be a more complex program logic.
You have to send the next byte repeatedly until you do not receive an error because the ExtPack was not ready.

### Timer_Unit

#### configure_ExtPack_timer()

Sets the timer up in the recommended order with your config values.

#### set_ExtPack_timer_prescaler()

Sends the prescaler divisor to the specified Timer unit to adjust its counting speed.

#### set_ExtPack_enable()

Enables or disables the specified Timer unit; enabling (Param: >1) starts the timer counting, disabling (Param: 0) stops it.

#### set_ExtPack_start_value()

Sets the initial count value for the specified Timer unit.

#### restart_ExtPack_timer()

Restarts the specified Timer unit by resetting its counter to the configured start value.

### General information

#### set_ExtPack_custom_ISR()

Sets a new custom ISR for the given unit at runtime.

## Available types

- unit_t
- unit_type_t
- ext_pack_error_t

## Available values

### unit_t

- unit_U00 (reset unit)
- unit_U01 (error unit)
- unitU02 (ack unit)
- ...
- unit_U62
- unit_U63

### unit_type_t

- EXTPACK_RESET_UNIT
- EXTPACK_ERROR_UNIT
- EXTPACK_ACK_UNIT
- EXTPACK_GPIO_UNIT
- EXTPACK_UART_UNIT
- EXTPACK_TIMER_UNIT
- EXTPACK_SPI_UNIT
- EXTPACK_I2C_UNIT

### ext_pack_error_t

- EXT_PACK_SUCCESS
- EXT_PACK_FAILURE