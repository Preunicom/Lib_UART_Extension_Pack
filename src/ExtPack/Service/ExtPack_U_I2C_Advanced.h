/**
 * @file ExtPack_U_I2C_Advanced.h
 *
 * @brief Advanced I2C communication helpers for the I2C unit in the ExtPack library.
 *
 * This header provides advanced functions to send and receive data via the I2C unit of ExtPack.
 * Includes functions for setting the I2C partner address and sending strings with delays.
 *
 * ## Provided Functions:
 * - receive_ExtPack_I2C_data_from_partner: Request data from a given I2C partner.
 * - send_ExtPack_I2C_String: Send a string via I2C using the current partner configuration.
 * - send_ExtPack_I2C_data_to_partner: Send a single byte to a specific partner address.
 * - send_ExtPack_I2C_String_to_partner: Send a string to a specific partner address with delay.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_U_I2C_ADVANCED_H
#define EXTPACK_U_I2C_ADVANCED_H

#include "ExtPack_Advanced.h"
#include "../Util/ExtPack_U_I2C.h"

/**
 * @defgroup I2C_Unit I2C Unit
 * @brief Functionality of the I2C Unit of ExtPack
 * @{
 */

/**
 * Request a byte from given partner and set the partner in the I2C_Unit of ExtPack.
 *
 * @note The received data will not be returned. Use the custom ISR to work with the received data.
 *
 * @param unit The I2C unit of ExtPack to receive data from.
 * @param partner_adr The partner address to get data from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t receive_ExtPack_I2C_data_from_partner(unit_t unit, uint8_t partner_adr);

/**
 * @brief Sends the given String until '\0' to ExtPack which then sends it over I2C.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @param unit The ExtPack unit to which the data should be sent. Including the correct set access mode for sending.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending an uint8_t because of an error while sending.
 */
static inline ext_pack_error_t send_ExtPack_I2C_String(unit_t unit, const uint8_t* data, uint16_t send_byte_delay_us) {
    return send_String_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data, send_byte_delay_us);
}

/**
 * Sends given data to slave of I2C_Unit of ExtPack by setting the partner adr of the unit followed by sending the data to set slave.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param partner_adr The partner address to send the data to.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_ExtPack_I2C_data_to_partner(unit_t unit, uint8_t partner_adr, uint8_t data);

/**
 * Sends a set_ExtPack_I2C_partner_adr control message followed by the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param partner_adr The partner address to send the data to.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a char because of an error while sending.
 */
ext_pack_error_t send_ExtPack_I2C_String_to_partner(unit_t unit, uint8_t partner_adr, const uint8_t* data, uint16_t send_byte_delay_us);

/** @} */

#endif //EXTPACK_U_I2C_ADVANCED_H