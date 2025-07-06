/**
 * @file ExtPack_U_Timer.h
 *
 * @brief Header file for Timer unit functions of the ExtPack library.
 *
 *
 * ## Features:
 * - Enable or disable a Timer unit using set_ExtPack_timer_enable.
 * - Restart a Timer unit using restart_ExtPack_timer.
 * - Set the prescaler divisor with set_ExtPack_timer_prescaler.
 * - Set the start value with set_ExtPack_timer_start_value.
 * - Configure, restart, and enable a Timer unit with configure_ExtPack_timer.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_TIMER_H
#define EXTPACK_U_TIMER_H

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Sends a command to enable or disable the specified Timer unit of ExtPack.
 *
 * @param unit The Timer unit of ExtPack whose enable state should be set.
 * @param enable Set to 0 to disable, any nonzero value to enable.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t set_ExtPack_timer_enable(unit_t unit, uint8_t enable) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 00), enable);
}

/**
 * @brief Sends a command to restart the specified Timer unit of ExtPack.
 *
 * @param unit The Timer unit of ExtPack to be restarted.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t restart_ExtPack_timer(unit_t unit) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 01), 0x00);
}

/**
 * @brief Sends the timer prescaler divisor to ExtPack for the specified Timer unit.
 *
 * @param unit The Timer unit of ExtPack for which the prescaler divisor is set.
 * @param prescaler_divisor The prescaler divisor value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t set_ExtPack_timer_prescaler(unit_t unit, uint8_t prescaler_divisor) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 10), prescaler_divisor);
}

/**
 * @brief Sends the new start value to ExtPack for the specified Timer unit.
 *
 * @param unit The Timer unit of ExtPack for which the start value is set.
 * @param start_value The start value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t set_ExtPack_timer_start_value(unit_t unit, uint8_t start_value) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 11), start_value);
}

/**
 * Configures the timer with prescaler divisor and start value.
 * Additionally, the timer is restarted and enabled.
 *
 * @param unit The Timer unit of ExtPack which to configure.
 * @param prescaler_divisor The prescaler divisor value to be applied.
 * @param start_value The start value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t configure_ExtPack_timer(unit_t unit, uint8_t prescaler_divisor, uint8_t start_value);

#endif //EXTPACK_U_TIMER_H
