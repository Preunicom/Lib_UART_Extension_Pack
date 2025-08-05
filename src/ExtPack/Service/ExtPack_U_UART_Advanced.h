/**
 * @file ExtPack_U_UART_Advanced.h
 *
 * @brief Advanced UART communication helper for the UART unit in the ExtPack library.
 *
 * @layer Service
 *
 * @details This header provides a wrapper function for sending null-terminated strings over UART
 * with optional delay between bytes.
 *
 * ## Provided Functions:
 * - send_ExtPack_UART_String: Sends a string over UART with delay, aborts on failure.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_U_UART_ADVANCED_H
#define EXTPACK_U_UART_ADVANCED_H

#include "../Util/ExtPack_U_UART.h"
#include "ExtPack_Advanced.h"

/**
 * @defgroup UART_Unit UART Unit
 * @brief Functionality of the UART Unit of ExtPack
 * @{
 */

/**
 * @brief Alias to send a null-terminated string to the specified UART unit of ExtPack.
 *
 * Sends the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @layer Service
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a uint8_t because of an error while sending.
 */
static inline ext_pack_error_t send_ExtPack_UART_String(unit_t unit, const uint8_t* data, uint16_t send_byte_delay_us) {
    return send_String_to_ExtPack(unit, data, send_byte_delay_us);
}

/** @} */

#endif //EXTPACK_U_UART_ADVANCED_H