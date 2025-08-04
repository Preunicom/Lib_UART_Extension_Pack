/**
 * @file ExtPack_U_Reset.h
 *
 * @brief Header file for Reset unit functions of the ExtPack library.
 *
 * ## Features:
 * - Send a reset command to reset unit using reset_ExtPack.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_RESET_H
#define EXTPACK_U_RESET_H

#include "../Core/ExtPack.h"

/**
 * @defgroup Reset_Unit Reset Unit
 * @brief Functionality of the Reset Unit of ExtPack
 * @{
 */

/**
 * @brief Send a reset command to unit zero of ExtPack via UART.
 *
 * @warning Be cautious when resetting the microcontroller in the custom ISR of the reset unit combined with resetting the ExtPack when initializing the microcontroller.
 * This leads to an endless reset loop.
 * You can avoid this by setting the custom ISR to "NULL" initially and setting it after a short time after resetting the ExtPack at startup.
 *
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t reset_ExtPack() {
    return _send_to_ExtPack(unit_U00, 0xFF);
}

/** @} */

#endif //EXTPACK_U_RESET_H
