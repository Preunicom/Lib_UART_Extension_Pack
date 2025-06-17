/**
 * @file ExtPack_LL_M328P.h
 *
 * @brief Low-level UART communication support for the ExtPack on ATMega328P.
 *
 * This header declares initialization and message-sending functions and
 * provides critical section macros to ensure safe UART communication
 * with the ExtPack hardware by saving and restoring the status register.
 *
 * ## Features:
 * - UART communication functions for sending and receiving data.
 * - Enter / Exit critical zones
 *
 * ## Internal Hardware Components Used:
 * - **USART0** (TX: PD1, RX: PD0)
 * - **Timer/Counter0**
 *
 * @author Markus Remy
 * @date 16.06.2025
 */

#ifndef EXTPACK_LL_M328P_H
#define EXTPACK_LL_M328P_H

#include "ExtPack_Defs.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * Saves the status register while in a critical zone
 */
uint8_t ExtPack_LL_SREG_save;

/**
 * Initializes the communication hardware and prepare the receive callback function.
 *
 * @param receive_callback Callback to be called when data from the ExtPack was received via UART.
 */
void _init_ExtPack_LL(void (*receive_callback)(unit_t, uint8_t));

/**
 * Sends the message to the ExtPack via UART.
 * Does not check the message or unit.
 *
 * @param unit The first byte to be sent.
 * @param data The second byte to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if UART is not ready.
 */
ext_pack_error_t _send_UART_ExtPack_message(unit_t unit, uint8_t data);

/**
 * @def enter_critical_zone()
 *
 * This macro saves the status register in the global variable ExtPack_LL_SREG_save and deactivates interrupts.
 * This is used to enter a critical zone.
 */
#define enter_critical_zone() \
    do { \
        ExtPack_LL_SREG_save = SREG; \
        cli(); \
    } while (0)

/**
 * @def exit_critical_zone()
 *
 * This macro resets the status register to the saved value in ExtPack_LL_SREG_save.
 * The value was saved when calling enter_critical_zone().
 * This is used to exit a critical zone.
 *
 * @warning Do not use before calling enter_critical_zone()!
 */
#define exit_critical_zone() \
    do { \
        SREG = ExtPack_LL_SREG_save; \
    } while (0)

#endif //EXTPACK_LL_M328P_H
