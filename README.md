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

# Examples

The examples can be found in the `examples` folder.
The naming is based on: `<ShortDescription>_Example__<UsedUnit1>_<UsedUnit2>_<...>_<UsedUnitN>.c`

## Build examples

To build the examples you can use the `examples` target of the makefile:  
`make examples MCU_AVR_GCC=atxxxxYYY F_CPU=YYYYYYYUL [DEFINES="[USED_UNITS=Y] [SEND_BUF_LEN=Y]"] [V=1]`  
This creates a hex file for every example in the `build/examples` folder.
This hex file can be flashed on the controller via for example avrdude.

## Communication

The microcontroller communicates with the Extension_Pack with 8N1 and 1 MBaud UART.

## Structure

The project is divided in 4 layers:
- HAL (ExtPack/HAL)
- Core (ExtPack/Core)
- Util (ExtPack/Util)
- Service (ExtPack/Service)

Every layer adds abstraction to the functionality.
The HAL layer handles the communication on hardware layer.
The Core layer is a general interface between hardware layer and units.
The Util layer adds the basic operations of the units.
The Service layer adds more complex operations of the units based on combining basic operations.
Therefore, the service layer can be cut off to be able to run the framework on smaller microcontrollers with less flash.

|       Component       |     HAL      |    Core layer    |    Util layer     |       Service layer        |  Unit type macro   |
|:---------------------:|:------------:|:----------------:|:-----------------:|:--------------------------:|:------------------:|
| ExtPack Communication | ExtPack_LL.h |    ExtPack.h     |         -         |     ExtPack_Advanced.h     |         -          |
|  Domain definitions   |      -       |  ExtPack_Defs.h  |         -         |             -              |         -          |
|        Events         |      -       | ExtPack_Events.h |         -         |             -              |         -          |
|    Dynamic delays     |      -       |        -         |  Dynamic_Delay.h  |             -              |         -          |
|      Reset Unit       |      -       |        -         | ExtPack_U_Reset.h |             -              | EXTPACK_RESET_UNIT |
|      Error Unit       |      -       |        -         | ExtPack_U_Error.h |             -              | EXTPACK_ERROR_UNIT |
|   Acknowledge Unit    |      -       |        -         |  ExtPack_U_ACK.h  |  ExtPack_U_ACK_Advanced.h  |  EXTPACK_ACK_UNIT  |
|       GPIO Unit       |      -       |        -         | ExtPack_U_GPIO.h  |             -              | EXTPACK_GPIO_UNIT  |
|       UART Unit       |      -       |        -         | ExtPack_U_UART.h  | ExtPack_U_UART_Advanced.h  | EXTPACK_UART_UNIT  |
|      Timer Unit       |      -       |        -         | ExtPack_U_Timer.h | ExtPack_U_Timer_Advanced.h | EXTPACK_TIMER_UNIT |
|       SPI Unit        |      -       |        -         |  ExtPack_U_SPI.h  |  ExtPack_U_SPI_Advanced.h  |  EXTPACK_SPI_UNIT  |
|       I2C Unit        |      -       |        -         |  ExtPack_U_I2C.h  |  ExtPack_U_I2C_Advanced.h  |  EXTPACK_I2C_UNIT  |
|       SRAM Unit       |      -       |        -         | ExtPack_U_SRAM.h  | ExtPack_U_SRAM_Advanced.h  | EXTPACK_SRAM_UNIT  |

## Functionality

### Units

This library supports Reset_Unit, Error_Unit, ACK_Unit, UART_Unit, GPIO_Uni, Timer_Unit, SPI_Unit, I2C_Unit and SRAM_Unit on the ExtPack.  

### Events
All units have events which are set if the unit received something.

### custom_ISR callbacks
custom ISRs for all units can be implemented.
They act like interrupted from the unit itself.
Interrupts are not enabled before calling the callback custom ISR.

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

## Further documentation

See the Doxygen documentation.  
Use `make docs` to create it.  
Then open `docs/index.html` in a browser.