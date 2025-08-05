/**
 * @file ExtPack.h
 *
 * @brief Header file for the Extension Pack library.
 *
 * @layer Core
 *
 * @details This file provides function prototypes, macros, and definitions for interfacing
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
#include "ExtPack_Defs.h"

/**
 * @brief Initializes communication with ExtPack over UART.
 *
 * @details This function enables USART0, Timer/Counter0 and global interrupts to be able to establish communication.
 * It also initializes unit_U00 as reset unit, unit_U01 as error unit, and unit_U02 as ACK unit with the given ISRs.
 *
 * @layer Core
 *
 * @param reset_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ExtPack got reset.
 * @param error_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the error unit of the ExtPack sends an error.
 * @param ack_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ACK unit of the ExtPack sends an acknowledgment.
 */
void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t));

/**
 * @brief Initializes the specified ExtPack unit with the given parameters.
 *
 * @layer Core
 *
 * @param unit The ExtPack unit to initialize.
 * @param unit_type The type of the unit.
 * @param custom_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when an interrupt occurs for this unit.
 */
void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, uint8_t));

/**
 * @brief Sets a new custom ISR for the given unit of ExtPack.
 *
 * @layer Core
 *
 * @note Use 'NULL' or 'nullptr' to have no ISR for the unit.
 *
 * @param unit The ExtPack unit which ISR should be set.
 * @param new_custom_ISR The new custom ISR function which is called when an interrupt of the unit occurs.
 */
void set_ExtPack_custom_ISR(unit_t unit, void (*new_custom_ISR)(unit_t, uint8_t));

/**
 * @brief Sets the access mode of the unit to the given one.
 *
 * @layer Core
 *
 * @param unit Unit to set the access mode for.
 * @param access_mode Access mode to set.
 */
inline unit_t _set_ExtPack_access_mode(unit_t unit, uint8_t access_mode) {
    return (unit & 0x3F) | (access_mode << 6);
}

/**
 * @brief Sends the data "as is" to ExtPack via UART.
 *
 * @layer Core
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t _send_to_ExtPack(unit_t unit, uint8_t data);

/**
 * @brief Returns the duration a UART send operation to ExtPack needs to perform in the worst case in us.
 *
 * @layer Core
 *
 * @return The duration in us.
 */
uint8_t get_ExtPack_send_duration_us();

/**
 * @brief This function saves the status register and deactivates interrupts.
 *
 * @layer Core
 *
 * @note This is used to enter a critical zone.
 */
void enter_critical_zone();

/**
 * @brief This function resets the status register to the saved value.
 *
 * @layer Core
 *
 * @details The value was saved when calling enter_critical_zone().
 *
 * @note This is used to exit a critical zone.
 *
 * @warning Do not use before calling enter_critical_zone()!
 */
void exit_critical_zone();

#endif //EXTPACK