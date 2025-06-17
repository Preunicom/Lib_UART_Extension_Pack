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

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Send a reset command to unit zero of ExtPack via UART.
 *
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t reset_ExtPack() {
    return send_to_ExtPack(unit_U00, 0xFF);
}

#endif //EXTPACK_U_RESET_H
