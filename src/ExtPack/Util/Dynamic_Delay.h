/**
 * @file Dynamic_Delay.h
 *
 * @brief Header file for dynamic delay functions of the ExtPack library.
 *
 * @layer Util
 *
 * @details This file provides busy-wait delay functions using _delay_us and _delay_ms
 * for dynamically customizable microsecond and millisecond delays.
 *
 * ## Features:
 * - delay_us: busy waits for a specified number of microseconds.
 * - delay_ms: busy waits for a specified number of milliseconds.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef DYNAMIC_DELAY_H
#define DYNAMIC_DELAY_H

// --------------------------------  Definition of auxiliary functions -------------------------------

/**
 * @brief Delays for approximately the given time through busy waiting.
 * Calls _delay_us(1) for delay_us times to bypass the problem to have compile-time constant values for _delay_us.
 *
 * @layer Util
 *
 * @param delay_us Delay time in us.
 */
void delay_us(unsigned int delay_us);

/**
 * @brief Delays for approximately the given time through busy waiting.
 * Calls _delay_ms(1) for delay_ms times to bypass the problem to have compile-time constant values for _delay_ms.
 *
 * @layer Util
 *
 * @param delay_ms Delay time in ms.
 */
void delay_ms(unsigned int delay_ms);

#endif //DYNAMIC_DELAY_H