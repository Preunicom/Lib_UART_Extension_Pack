/**
 * @file ExtPack_U_SPI.h
 *
 * @brief Header file for SPI unit functions of the ExtPack library.
 *
 * @layer Util
 *
 * ## Features:
 * - Set the SPI slave id with set_ExtPack_SPI_slave.
 * - Send a single byte via SPI using send_ExtPack_SPI_data.
 * - Send data to a specific slave with send_ExtPack_SPI_data_to_slave.
 * - Send null-terminated strings with retry support using send_ExtPack_SPI_String and send_ExtPack_SPI_String_to_slave.
 * - Retrieve the last set SPI slave id with get_ExtPack_data_SPI_current_slave.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_SPI_H
#define EXTPACK_U_SPI_H

#include "../Core/ExtPack.h"

/**
 * @defgroup SPI_Unit SPI Unit
 * @brief Functionality of the SPI Unit of ExtPack
 * @{
 */

/**
 * @brief Sets the slave ID of the given SPI unit of ExtPack via UART control message.
 *
 * @layer Util
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send following data to.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_SPI_slave(unit_t unit, uint8_t slave_id);

/**
 * @brief Sends the data with the SPI unit of ExtPack.
 *
 * @layer Util
 *
 * @param unit The ExtPack SPI unit to send the data.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t send_ExtPack_SPI_data(unit_t unit, uint8_t data) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data);
}

/**
 * @brief Retrieves the last set SPI slave id of the given ExtPack SPI unit.
 *
 * @layer Util
 *
 * @warning When the ExtPack did not get the sent partner address, this function will still return the sent one.
 *
 * @param unit The SPI unit of ExtPack to get the current set slave id from.
 * @return The last sent SPI slave id.
 */
uint8_t get_ExtPack_data_SPI_current_slave(unit_t unit);

/**
 * @brief Function to retrieve the last received SPI data byte of the given ExtPack SPI unit.
 *
 * @layer Util
 *
 * @param unit The unit to get the data from
 * @return The last received SPI data from this unit.
 */
uint8_t get_last_received_ExtPack_SPI_data(unit_t unit);

/** @} */

#endif //EXTPACK_U_SPI_H
