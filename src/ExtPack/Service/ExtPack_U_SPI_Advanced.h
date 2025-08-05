/**
 * @file ExtPack_U_SPI_Advanced.h
 *
 * @brief Advanced SPI communication helpers for the SPI unit in the ExtPack library.
 *
 * @layer Service
 *
 * @details This header provides functions for sending data and strings over SPI using the ExtPack framework.
 * It includes functionality for selecting the SPI slave and inserting delays between transmitted bytes.
 *
 * ## Provided Functions:
 * - send_ExtPack_SPI_data_to_slave: Sends one byte to a specific slave device.
 * - send_ExtPack_SPI_String_to_slave: Sends a string with delay to a specific slave device.
 * - send_ExtPack_SPI_String: Sends a string with delay using a previously configured access mode.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */
#ifndef EXTPACK_U_SPI_ADVANCED_H
#define EXTPACK_U_SPI_ADVANCED_H

#include "ExtPack_Advanced.h"
#include "../Util/ExtPack_U_SPI.h"

/**
 * @defgroup SPI_Unit SPI Unit
 * @brief Functionality of the SPI Unit of ExtPack
 * @{
 */

/**
 * @brief Sends given data to slave of SPI_Unit of ExtPack by setting the slave id of the unit followed by sending the data to set slave.
 *
 * @layer Service
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send the data to.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data);

/**
 * @brief Sends a set_ExtPack_SPI_slave control message followed by the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @layer Service
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send the data to.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a char because of an error while sending.
 */
ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_byte_delay_us);

/**
 * @brief Sends the given String until '\0' to ExtPack which then sends it over SPI.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @layer Service
 *
 * @param unit The ExtPack unit to which the data should be sent. Including the correct set access mode for sending.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending an uint8_t because of an error while sending.
 */
static inline ext_pack_error_t send_ExtPack_SPI_String(unit_t unit, const uint8_t* data, uint16_t send_byte_delay_us) {
    return send_String_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data, send_byte_delay_us);
}

/** @} */

#endif //EXTPACK_U_SPI_ADVANCED_H