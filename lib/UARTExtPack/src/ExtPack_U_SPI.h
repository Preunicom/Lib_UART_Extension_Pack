/**
 * @file ExtPack_U_SPI.h
 *
 * @brief Header file for SPI unit functions of the ExtPack library.
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

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Sets the slave ID of the given SPI unit of ExtPack via UART control message.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send following data to.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
inline ext_pack_error_t set_ExtPack_SPI_slave(unit_t unit, uint8_t slave_id) {
    return send_to_ExtPack(_set_ExtPack_access_mode(unit, 01), slave_id);
}

/**
 * @brief Sends the data with the SPI unit of ExtPack.
 *
 * @param unit The ExtPack SPI unit to send the data.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
inline ext_pack_error_t send_ExtPack_SPI_data(unit_t unit, uint8_t data) {
    return send_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data);
}

/**
 * Sends given data to slave of SPI_Unit of ExtPack by setting the slave id of the unit followed by sending the data to set slave.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send the data to.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data);

/**
 * Sends a set_ExtPack_SPI_slave control message followed by the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a set_ExtPack_SPI_slave or a normal send operation per send_ExtPack_SPI_data.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send the data to.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_delay_us The delay waited between two sent messages. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a message. 0 for unlimited retries.
 * @param retry_delay_us The delay between send message attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a char because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

/**
 * @brief Retrieves the last set SPI slave id of the given ExtPack SPI unit.
 *
 * @param unit The SPI unit of ExtPack to get the current set slave id from.
 * @return The last sent SPI slave id.
 */
uint8_t get_ExtPack_data_SPI_current_slave(unit_t unit);

/**
 * @brief Sends the given String until '\0' to ExtPack which then sends it over SPI.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a normal send operation via send_to_ExtPack per byte.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent. Including the correct set access mode for sending.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_delay_us The delay waited between two sent chars. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a char. 0 for unlimited retries.
 * @param retry_delay_us The delay between send char attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending an uint8_t because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
static inline ext_pack_error_t send_ExtPack_SPI_String(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    return send_String_to_ExtPack(unit, data, send_delay_us, max_attempts, retry_delay_us);
}


#endif //EXTPACK_U_SPI_H
