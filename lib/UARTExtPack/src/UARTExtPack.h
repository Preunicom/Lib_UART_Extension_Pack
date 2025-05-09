/**
 * @file UARTExtPack.h
 *
 * @brief Header file for the UART Extension Pack library for the ATmega328P.
 *
 * This file provides function prototypes, macros, and definitions for interfacing
 * with the ExtPack system over UART. The library enables communication with
 * various unit types, including GPIO, UART, and Timer units.
 *
 * ## Features:
 * - Initialization functions for the ExtPack system and individual units.
 * - UART communication functions for sending and receiving data.
 * - GPIO interface for retrieving input and output states.
 * - Timer control functions, including enabling, restarting, and configuring timers.
 * - Macro-based aliases for simplified function calls.
 *
 * ## Internal Hardware Components Used:
 * - **USART0** (TX: PD1, RX: PD0)
 * - **Timer/Counter0**
 *
 * @author Markus Remy
 * @date 24.03.2025
 */

#ifndef LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_H
#define LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_H

#include <stdint.h>
#include <util/delay.h>

// --------------------------------------- Definition of Units ---------------------------------------

#include "UARTExtPack_UnitDefs.h"  // Include the header file for unit definitions

/**
 * @typedef unit_t
 * Type alias for unit identifier.
 *
 * @details 'unit_t' is defined as uint8_t and is used to represent the unique identifiers
 * for various units within the system. It is passed to functions that require a specific unit
 * identifier for communication or management.
 */
typedef uint8_t unit_t;

// ------------------------------------- Definitions of Unit Types -------------------------------------

/**
 * @typedef unit_type_t
 * Type alias for unit type identifier.
 *
 * @details 'unit_type_t' is defined as uint8_t and is used to represent different types of units,
 * such as GPIO, UART, and Timer. It helps categorize units within the system and is passed to
 * functions that require the identification of unit types.
 */
typedef uint8_t unit_type_t;

/**
 * @def UNDEFINED
 * Constant value representing an undefined unit type.
 */
#define UNDEFINED 0

/**
 * @def Timer_Unit
 * Constant value representing the Timer unit type.
 */
#define Reset_Unit 1

/**
 * @def Timer_Unit
 * Constant value representing the Timer unit type.
 */
#define Error_Unit 2

/**
 * @def GPIO_Unit
 * Constant value representing the GPIO (General Purpose Input/Output) unit type.
 */
#define GPIO_Unit 3

/**
 * @def UART_Unit
 * Constant value representing the UART (Universal Asynchronous Receiver/Transmitter) unit type.
 */
#define UART_Unit 4

/**
 * @def Timer_Unit
 * Constant value representing the Timer unit type.
 */
#define Timer_Unit 5

/**
 * @typedef ext_pack_error_t
 * Type alias for ExtPack errors.
 *
 * @details 'ext_pack_error_t' is defined as uint8_t and is used to represent ExtPack errors.
 */
typedef uint8_t ext_pack_error_t;

/**
 * @def EXT_PACK_SUCCESS
 * @brief Success value for ExtPack library functions.
 *
 * This value is returned by functions to indicate that the operation was successful.
 */
#define EXT_PACK_SUCCESS 0

/**
 * @def EXT_PACK_FAILURE
 * @brief Failure value for ExtPack library functions.
 *
 * This value is returned by functions when an error has occurred.
 */
#define EXT_PACK_FAILURE 1

// --------------------------------  Definition of auxiliary functions -------------------------------

/**
 * Delays for approximately the given time through busy waiting.
 * Calls _delay_us(1) for delay_us times to bypass the problem to have compile-time constant vales for _delay_us.
 *
 * @param delay_us Delay time in us.
 */
void delay_us(unsigned int delay_us);

/**
*  Delays for approximately the given time through busy waiting.
 * Calls _delay_ms(1) for delay_ms times to bypass the problem to have compile-time constant vales for _delay_ms.
 *
 * @param delay_ms Delay time in ms.
 */
void delay_ms(unsigned int delay_ms);

// -------------------------------------  Definition of Interface -------------------------------------

/**
 * Initializes communication with ExtPack over UART.
 * This function enables USART0, Timer/Counter0 and global interrupts to be able to establish communication.
 * It also initializes unit_U00 as reset unit and unit_U01 as error unit with given ISRs.
 *
* @param reset_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ExtPack got reset.
* @param error_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the error unit of the ExtPack sends an error.
 */
void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t));

/**
 * Initializes the specified ExtPack unit with the given parameters.
 *
 * @param unit The ExtPack unit to initialize.
 * @param unit_type The type of the unit.
 * @param custom_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when an interrupt occurs for this unit.
 */
void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, uint8_t));

/**
 * @def SEND_MAX_ATTEMPTS
 * Calls the specified ExtPack send function repeatedly until the data is successfully sent or the maximum number of attempts is reached.
 *
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Could lead to errors in condition. Call before and save return value in a variable instead.
 *
 * @param func_call The send function to be called. It should return `EXT_PACK_SUCCESS` or `EXT_PACK_FAILURE`.
 * @param max_attempts The maximum number of attempts to retry the function call. If set to 0 the specified ExtPack send function is called repeatedly until the data is successfully sent.
 * @param delay_us The delay in microseconds applied before each retry.
 */
#define SEND_MAX_ATTEMPTS(func_call, max_attempts, retry_delay_us) \
    ({ \
        int attempts = 0; \
        int result = EXT_PACK_SUCCESS; \
        while ((func_call) == EXT_PACK_FAILURE) { \
            attempts++; \
            if (attempts >= (max_attempts) && (max_attempts) != 0) { \
                result = EXT_PACK_FAILURE; \
                break; \
            } \
            delay_us(retry_delay_us); \
        } \
        result; \
    })

// ------------------- General interface -------------------

/**
 * Sends the data "as is" to ExtPack via UART.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t UART_ExtPack_send(unit_t unit, uint8_t data);

// ------------------- RST_Unit interface ------------------

/**
 * Send a reset command to unit zero of ExtPack via UART.
 *
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t reset_ExtPack();

// ------------------ UART_Unit interface ------------------

/**
 * Sends the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a normal send operation per UART_ExtPack_send.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent as String with terminating '\0'.
 * @param delay_us The delay waited between two sent chars. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a char. 0 for unlimited retries.
 * @param retry_delay_us The delay between send char attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a uint8_t because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t send_ExtPack_UART_String(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

// ------------------ GPIO_Unit interface ------------------

/**
 * Requests the current input pin states of a GPIO unit.
 *
 * @param unit The target GPIO unit.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t refresh_ExtPack_gpio_data(unit_t unit);

/**
 * Retrieves the last received values of the given ExtPack GPIO unit's input pins.
 *
 * @param unit The GPIO unit of ExtPack from which the values are retrieved.
 * @return The last received input values of the GPIO unit.
 */
uint8_t get_ExtPack_data_gpio_in(unit_t unit);

/**
 * Retrieves the last sent values of the given ExtPack GPIO unit's output pins.
 *
 * @param unit The GPIO unit of ExtPack from which the values are retrieved.
 * @return The last sent output values of the GPIO unit.
 */
uint8_t get_ExtPack_data_gpio_out(unit_t unit);

// -------------------- Timer interface --------------------

/**
 * Sends a command to enable or disable the specified Timer unit of ExtPack.
 *
 * @param unit The Timer unit of ExtPack whose enable state should be set.
 * @param enable Set to 0 to disable, any nonzero value to enable.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_timer_enable(unit_t unit, uint8_t enable);

/**
 * Sends a command to restart the specified Timer unit of ExtPack.
 *
 * @param unit The Timer unit of ExtPack to be restarted.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t restart_ExtPack_timer(unit_t unit);

/**
 * Sends the timer prescaler divisor to ExtPack for the specified Timer unit.
 *
 * @param unit The Timer unit of ExtPack for which the prescaler divisor is set.
 * @param prescaler_divisor The prescaler divisor value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_timer_prescaler(unit_t unit, uint8_t prescaler_divisor);

/**
 * Sends the new start value to ExtPack for the specified Timer unit.
 *
 * @param unit The Timer unit of ExtPack for which the start value is set.
 * @param start_value The start value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_timer_start_value(unit_t unit, uint8_t start_value);

/**
 * Configures the timer with prescaler divisor and start value.
 * Additionally, the timer is restarted and enabled.
 *
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The Timer unit of ExtPack which to configure.
 * @param prescaler_divisor The prescaler divisor value to be applied.
 * @param start_value The start value to be applied.
 * @param delay_us The delay waited between two sent commands. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a command. 0 for unlimited retries.
 * @param retry_delay_us The delay between two send command attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t configure_ExtPack_timer(unit_t unit, uint8_t prescaler_divisor, uint8_t start_value, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

// -------------------------------------- Aliases for Interfaces --------------------------------------

/**
 * @def send_ExtPack_generic_data
 * Alias to send unmodified data to the specified unit of ExtPack.
 *
 * This macro invokes `UART_ExtPack_send` to transmit the data.
 */
#define send_ExtPack_generic_data UART_ExtPack_send

/**
 * @def set_ExtPack_gpio_out
 * Alias to send output pin values to the specified GPIO unit of ExtPack.
 *
 * This macro invokes `UART_ExtPack_send` to transmit the data.
 */
#define set_ExtPack_gpio_out UART_ExtPack_send

/**
 * @def send_ExtPack_UART_data
 * Alias to send UART data to the specified UART unit of ExtPack.
 *
 * This macro invokes `UART_ExtPack_send` to transmit the data.
 */
#define send_ExtPack_UART_data UART_ExtPack_send

#endif //LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_H