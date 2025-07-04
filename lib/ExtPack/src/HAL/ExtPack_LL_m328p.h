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
 * - **Timer/CounterA**
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
 * @def BAUD_RATE
 *
 * @brief The BAUD rate used to communicate with the ExtPack.
 */
#define BAUD_RATE 1000000

/**
 * @def EXT_PACK_SOFTWARE_OVERHEAD_UART_TRANSMISSION_CLOCK_CYCLES
 *
 * @brief Calculates the amount of clock cycles an ExtPack UART transmission needs on top of the UART transmission duration itself.
 */
#define EXT_PACK_SOFTWARE_OVERHEAD_UART_TRANSMISSION_CLOCK_CYCLES 25

/**
 * @def EXT_PACK_UART_BITS_PER_COMMAND_PAIR
 *
 * @brief The amount of bits sent via UART per ExtPack command pair.
 * @note start bits + data bits + parity bits + stop bits
 */
#define EXT_PACK_UART_BITS_PER_COMMAND_PAIR 20

/**
 * Saves the status register while in a critical zone
 */
extern volatile uint8_t ExtPack_LL_SREG_save;

/**
 * Initializes the communication hardware.
 */
void _init_ExtPack_LL();

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

/**
 * @def enable_global_interrupts
 *
 * @brief This macro enables global interrupts.
 * @note Use when entering long callback ISR unit functions
 */
#define enable_global_interrupts() sei()

#endif //EXTPACK_LL_M328P_H
