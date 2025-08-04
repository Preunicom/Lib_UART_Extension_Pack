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

#include "../Core/ExtPack.h"

/**
 * @defgroup UART_Unit UART Unit
 * @brief Functionality of the ISSI_IS61WV5128BLL_SRAM Unit of ExtPack
 * @{
 */

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
 * @brief Function to retrieve the last received UART data byte of the given ExtPack UART unit.
 *
 * @param unit The unit to get the data from
 * @return The last received UART data from this unit.
 */
uint8_t get_last_received_ExtPack_UART_data(unit_t unit);

/** @} */

#endif //EXTPACK_U_UART_H
