
/**
 * @file ExtPack_U_I2C.h
 *
 * @brief Header file for I2C unit functions of the ExtPack library.
 *
 * ## Features:
 * - Retrieve the current I2C partner address using get_ExtPack_data_I2C_current_partner_adr.
 * - Retrieve the last received I2C data byte using get_ExtPack_data_I2C_last_received_data.
 * - Set the I2C partner address with set_ExtPack_I2C_partner_adr.
 * - Request and receive a byte from the current or specified partner using receive_ExtPack_I2C_data and receive_ExtPack_I2C_data_from_partner.
 * - Send a byte via I2C using send_ExtPack_I2C_data.
 * - Send null-terminated strings with retry support using send_ExtPack_I2C_String and send_ExtPack_I2C_String_to_partner.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_I2C_H
#define EXTPACK_U_I2C_H

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Retrieves the last set I2C partner address of the given ExtPack I2C unit.
 *
 * @param unit The I2C unit of ExtPack to get the current set partner address from.
 * @return The last sent I2C slave id.
 */
uint8_t get_ExtPack_data_I2C_current_partner_adr(unit_t unit);

/**
 * @brief Alias to retrieve the last received I2C data byte of the given ExtPack I2C unit.
 *
 * @param unit The I2C unit to get the data from.
 * @return The last received I2C data from this unit.
 */
uint8_t get_ExtPack_data_I2C_last_received_data(unit_t unit);

/**
 * @brief Alias to set the partner address of the given I2C unit of ExtPack.
 *
 * @param unit The unit to set the I2C partner address for.
 * @param slave_id The slave id to set.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_I2C_partner_adr(unit_t unit, uint8_t slave_id);

/**
 * @brief Requests a byte from the currently set partner of the I2C unit of ExtPack.
 *
 * @note The received data will not be returned. Use the custom ISR to work with the received data.
 *
 * @param unit The I2C unit of ExtPack to receive data from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t receive_ExtPack_I2C_data(unit_t unit) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 10), 0x00);
}

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
 * @brief Send a byte via a I2C Unit of ExtPack.
 *
 * @param unit The I2C unit to send the data with.
 * @param data The data to send.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t send_ExtPack_I2C_data(unit_t unit, uint8_t data) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data);
}

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

#endif //EXTPACK_U_I2C_H