/**
 * @file ExtPack_U_Timer_Advanced.h
 *
 * @brief Advanced timer configuration helpers for the Timer unit in the ExtPack library.
 *
 * This header provides functions to configure timer units with specific prescaler and start values,
 * including restarting and enabling the timer in one call.
 *
 * ## Provided Functions:
 * - configure_ExtPack_timer: Configures, restarts, and enables the timer with given settings.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_U_TIMER_ADVANCED_H
#define EXTPACK_U_TIMER_ADVANCED_H

#include "../Util/ExtPack_U_Timer.h"

/**
 * @defgroup Timer_Unit Timer Unit
 * @brief Functionality of the Timer Unit of ExtPack
 * @{
 */

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

/** @} */

#endif //EXTPACK_U_TIMER_ADVANCED_H