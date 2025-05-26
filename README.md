# Lib_UART_Extension_Pack_for_ATMega328P

## Communication

ATMega328P communicates with the UART_Extension_Pack with 8N1 and 1 MBaud.

## Functionality
### Units

This library supports Reset_Unit, Error_Unit, UART_Unit, GPIO_Uni, Timer_Unit and SPI_Unit on the ExtPack.  

### Functions

#### Transmitting

- Reset ExtPack
- Sending UART data
- Setting GPIO output values
- Configure timer
- Sending SPI data

#### Receiving

- custom ISRs for all units can be implemented (act like interrupted from the unit itself)
- GPIO has an extra feature with saving the last in- and output values to have current values accessible
- SPI also stores the last set slave id to be able to minimize control message overhead

## Usage

### Initialisation

1) Initialize ExtPack with __init_ExtPack(reset_ISR, error_ISR, ack_ISR)__
2) Initialize all used units except reset and error units they are already initialized in __init_ExtPack__() with 
   - unit number (__unit_UXX__)
   - unit type (__UART_Unit__, __GPIO_Unit__ or __Timer_Unit__) and
   - your custom ISR (of type __void (*func)(unit_t, char)__)

### reset_ExtPack()

Resets the ExtPack.
**WARNING**: Be cautious when resetting the microcontroller in the custom ISR of the reset unit combined with 
resetting the ExtPack when initializing the microcontroller. This leads to an endless reset loop.  
You can avoid this by setting the custom ISR to "NULL" initially and setting it after resetting the ExtPack at startup.
You can use "set_ExtPack_custom_ISR()" for it.

### set_ExtPack_custom_ISR()

Sets a new custom ISR for the given unit at runtime.

### get_ExtPack_ack_state() & get_ExtPack_ack_event()

This functions return the state or the event of ExtPack ACK unit. 
It can be 0 (not active/event not set) or 1 (active/event set).

### wait_for_ExtPack_ACK_data() & wait_for_ExtPack_ACK()

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
input_values:
- Bit 0: ACK state (0: not enabled / 1: enabled)  
- Bit 1-6: Unused  
- Bit 7: ACK received event (0: not set / 1: set)

output_values:
- Bit 0-7: data of last received acknowledgment  

### Timer configuration

Use __configure_ExtPack_timer()__ with your config values to set the timer up.

### SEND_MAX_ATTEMPTS()

The __SEND_MAX_ATTEMPTS__ macro calls your function max_attempts times until the function returns __EXT_PACK_SUCCESS__.  
This means you can not use the macro on __configure_ExtPack_timer()__ or __send_ExtPack_UART_String()__ as they contain more than one command.  
You can specify how often you want to retry the function with __max_attempts__.  
- 0 : Unlimited retries (Use with caution, could lead to very long blocking loops)
- 1 : Tries only one time to call the function
- \>1 : Retries for maximum x - 1 times

You can also specify the time between two retries with __retry_delay_us__.  
The value is usually in a three-digit range.  
When __max_attempts__ is equal to zero there is no effect of __retry_delay_us__.

Some functions like __send_ExtPack_UART_String()__ use SEND_MAX_ATTEMPTS and take these parameters in their own signature.

### send_ExtPack_UART_String() & send_ExtPack_SPI_String_to_slave

Sends the String to a UART/SPI unit of ExtPack without the trailing '\0'.  
Make sure the string ends with a '\0'!

### set_ExtPack_data_gpio_out() / get_ExtPack_data_gpio_out()

These functions do not communicate with the ExtPack.  
They take the last received/sent data.  
To refresh this data use __refresh_ExtPack_gpio_data__.

### get_ExtPack_data_SPI_current_slave()

This function does not communicate with the ExtPack.
They take the last sent slave id as the SPI Unit stores it for further messages.
**WARNING:** If there is an error while sending the slave id (for example it does not reach the ExtPack or is invalid)
it will be stored either. The slave id has to be set again.

## Available functions

- void __init_ExtPack__(void (*reset_ISR)(unit_t, char), void (*error_ISR)(unit_t, char))
- void __init_ExtPack_Unit__(unit_t, unit_type, void (*custom_ISR)(unit_t, char))
- ext_pack_error_t __UART_ExtPack_send__(unit_t, char)
- void __set_ExtPack_custom_ISR__(unit_t, void (*new_custom_ISR)(unit_t, char))
- ext_pack_error_t __reset_ExtPack__()
- uint8_t __get_ExtPack_ack_state__();
- void __clear_ExtPack_ack_event__();
- uint8_t __get_ExtPack_ack_event__();
- ext_pack_error_t __wait_for_ExtPack_ACK_data__(uint8_t, uint16_t);
- ext_pack_error_t __wait_for_ExtPack_ACK__(uint16_t);
- ext_pack_error_t __send_ExtPack_UART_String__(unit_t, const char*, uint16_t, uint8_t, uint16_t)
- ext_pack_error_t __refresh_ExtPack_gpio_data__(unit_t)
- char __get_ExtPack_data_gpio_in__(unit_t)
- char __get_ExtPack_data_gpio_out__(unit_t)
- ext_pack_error_t __set_ExtPack_timer_enable__(unit_t, uint8_t)
- ext_pack_error_t __restart_ExtPack_timer__(unit_t)
- ext_pack_error_t __set_ExtPack_timer_prescaler__(unit_t, uint8_t);
- ext_pack_error_t __set_ExtPack_timer_start_value__(unit_t, uint8_t);
- ext_pack_error_t __configure_ExtPack_timer__(unit_t, uint8_t, uint8_t, uint16_t, uint8_t, uint16_t)
- ext_pack_error_t __set_ExtPack_SPI_slave__(unit_t, uint8_t)
- ext_pack_error_t __send_ExtPack_SPI_data_to_slave__(unit_t, uint8_t, char)
- ext_pack_error_t __send_ExtPack_SPI_String_to_slave__(unit_t, uint8_t, const char*, uint16_t, uint8_t, uint16_t)
- uint8_t __get_ExtPack_data_SPI_current_slave__(unit_t)

## Available macros

- __SEND_MAX_ATTEMPTS__(func_call, max_attempts, delay_us)
- __get_ExtPack_ack_data__()
- __set_ExtPack_ACK_enable__(enable)
- __set_ExtPack_gpio_out__(unit_t, char)
- __send_ExtPack_UART_data__(unit_t, char)
- __send_ExtPack_SPI_data__(unit_t, char)
- __send_ExtPack_SPI_String__(unit_t, const char*, uint16_t, uint8_t, uint16_t)

## Available types

- unit_t
- unit_type_t
- ext_pack_error_t

## Available values

### unit_t

- unit_U00 (reset unit)
- unit_U01 (error unit)
- ...
- unit_U62
- unit_U63

### unit_type_t

- Reset_Unit
- Error_Unit
- GPIO_Unit
- UART_Unit
- Timer_Unit
- SPI_Unit

### ext_pack_error_t

- EXT_PACK_SUCCESS
- EXT_PACK_FAILURE