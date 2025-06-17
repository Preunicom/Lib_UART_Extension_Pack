/**
 * @file ExtPack.h
 *
 * @brief Header file for the Extension Pack library.
 *
 * This file provides function prototypes, macros, and definitions for interfacing
 * with the ExtPack system. The library prepares communication for
 * various unit types, including GPIO, UART, Timer, SPI and I2C units.
 *
 * ## Features:
 * - Initialization functions for the ExtPack system and individual units.
 * - GPIO interface for retrieving input and output states.
 * - Timer control functions, including enabling, restarting, and configuring timers.
 * - SPI interface functions for managing SPI communication (setting slave, sending/receiving data).
 * - I2C interface functions for managing I2C communication (setting partner, sending/receiving data).
 * - Macro-based aliases for simplified function calls.
 *
 * @author Markus Remy
 * @date 15.06.2025
 */

#ifndef EXTPACK_H
#define EXTPACK_H

#include <stdint.h>
#include "Dynamic_Delay.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

#if defined(__AVR_ATmega328P__)
    #include "ExtPack_LL_M328P.h"
#else
    #error Unsupported microcontroller!
#endif

/**
 * Initializes communication with ExtPack over UART.
 * This function enables USART0, Timer/Counter0 and global interrupts to be able to establish communication.
 * It also initializes unit_U00 as reset unit, unit_U01 as error unit, and unit_U02 as ACK unit with the given ISRs.
 * @param reset_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ExtPack got reset.
 * @param error_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the error unit of the ExtPack sends an error.
 * @param ack_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ACK unit of the ExtPack sends an acknowledgment.
 */
void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t));

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
 * Sets a new custom ISR for the given unit of ExtPack.
 *
 * @note Use 'NULL' to have no ISR for the unit.
 *
 * @param unit The ExtPack unit which ISR should be set.
 * @param new_custom_ISR The new custom ISR function which is called when an interrupt of the unit occurs.
 */
void set_ExtPack_custom_ISR(unit_t unit, void (*new_custom_ISR)(unit_t, uint8_t));

/**
 * @def SEND_MAX_ATTEMPTS
 * @brief Calls the specified ExtPack send function repeatedly until the data is successfully sent or the maximum number of attempts is reached.
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

/**
 * @brief Sets the access mode of the unit to the given one.
 *
 * @param unit Unit to set the access mode for.
 * @param access_mode Access mode to set.
 */
inline unit_t _set_ExtPack_access_mode(unit_t unit, uint8_t access_mode) {
    return (unit & 0x3F) | (access_mode << 6);
}

/**
 * Sends the data "as is" to ExtPack via UART.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_to_ExtPack(unit_t unit, uint8_t data);

/**
 * Sends the given String until '\0' to ExtPack.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a normal send operation via send_to_ExtPack per byte.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent. Including the correct set access mode for sending.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_delay_us The delay waited between two sent chars. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a char. 0 for unlimited retries.
 * @param retry_delay_us The delay between send char attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending an uint8_t because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

#endif //EXTPACK