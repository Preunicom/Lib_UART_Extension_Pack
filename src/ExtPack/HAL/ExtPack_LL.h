/**
 * @file ExtPack_LL.h
 *
 * @brief Low-Level hardware interaction layer for ExtPack communication.
 *
 * @layer HAL
 *
 * @details This file provides functions for direct communication with ExtPack units via UART,
 * including unvalidated command transmission and interrupt handling.
 *
 * ## Features:
 * - Low-level initialization of hardware resources.
 * - Raw UART command transmission to ExtPack units.
 * - Basic critical section handling using interrupt control.
 *
 * This layer operates without validation or abstraction and is used internally by higher-level ExtPack logic.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_LL_H
#define EXTPACK_LL_H

#include "../Core/ExtPack_Defs.h"

/**
 * @def BAUD_RATE
 *
 * @layer HAL
 *
 * @brief The BAUD rate used to communicate with the ExtPack.
 */
#define BAUD_RATE 1000000

/**
 * @def EXT_PACK_ESTIMATED_SOFTWARE_OVERHEAD_UART_COMMAND_TRANSMISSION_CLOCK_CYCLES
 *
 * @layer HAL
 *
 * @brief Calculates the amount of clock cycles an ExtPack UART transmission needs on top of the UART transmission duration itself.
 */
#define EXT_PACK_ESTIMATED_SOFTWARE_OVERHEAD_UART_COMMAND_TRANSMISSION_CLOCK_CYCLES 25

/**
 * @def EXT_PACK_UART_BITS_PER_COMMAND_PAIR
 *
 * @layer HAL
 *
 * @brief The amount of bits sent via UART per ExtPack command pair.
 * @note start bits + data bits + parity bits + stop bits
 */
#define EXT_PACK_UART_BITS_PER_COMMAND_PAIR 20

/**
 * @brief Initializes the hardware used for interactions with ExtPack.
 *
 * @layer HAL
 */
void init_ExtPack_LL();

/**
 * @brief Sends an ExtPack command via UART.
 * This command is not checked for consistency, syntax or semantic.
 *
 * @layer HAL
 *
 * @param unit The unit number (bit 0-5) and the access mode bits (bit 6-7).
 * @param data The data to send.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_UART_ExtPack_command(unit_t unit, uint8_t data);

/**
 * @brief Saves the interrupt state and disables interrupts.
 *
 * @layer HAL
 */
void enter_critical_zone();

/**
 * @brief Resets the interrupt state to the last saved one from enter_critical_zone.
 *
 * @layer HAL
 */
void exit_critical_zone();

extern void process_received_ExtPack_data(unit_t unit, uint8_t data);

#endif //EXTPACK_LL_H