/**
 * @file ExtPack_U_UART.h
 *
 * @brief Header file for UART unit functions of the ExtPack library.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_UART_H
#define EXTPACK_U_UART_H

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Sends a byte via the UART unit of ExtPack.
 *
 * @param unit The UART unit to send with.
 * @param data The data to send.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t send_ExtPack_UART_data(unit_t unit, uint8_t data) {
    return _send_to_ExtPack(unit, data);
}

/**
 * @brief Alias to send a null-terminated string to the specified UART unit of ExtPack.
 *
 * Sends the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a uint8_t because of an error while sending.
 */
static inline ext_pack_error_t send_ExtPack_UART_String(unit_t unit, const uint8_t* data, uint16_t send_byte_delay_us) {
    return send_String_to_ExtPack(unit, data, send_byte_delay_us);
}

#endif //EXTPACK_U_UART_H
