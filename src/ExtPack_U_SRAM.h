/**
 * @file ExtPack_U_SRAM.h
 *
 * @brief Header file for SRAM access operations of the ExtPack library.
 *
 * ## Features:
 * - Resets the SRAM unit's addressing logic using reset_ExtPack_SRAM_address.
 * - Sets successive address bytes using set_ExtPack_SRAM_next_address_byte.
 * - Sets a full SRAM address using set_ExtPack_SRAM_address.
 * - Writes data to the previously set address using write_ExtPack_SRAM_data.
 * - Writes data directly to a given address using write_ExtPack_SRAM_data_to_address.
 * - Requests data from the previously set address using request_ExtPack_SRAM_data.
 * - Requests data from a specific address using request_ExtPack_SRAM_data_from_address.
 * - Reads data from the previously set address using read_ExtPack_SRAM_data.
 * - Reads data from a specific address using read_ExtPack_SRAM_data_from_address.
 *
 * @author Markus Remy
 * @date 03.08.2025
 */

#ifndef EXTPACK_U_SRAM_H
#define EXTPACK_U_SRAM_H

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Resets the address of the SRAM unit to zero and set the next address block to write to to zero.
 *
 * @param unit The SRAM unit of ExtPack to reset the address from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t reset_ExtPack_SRAM_address(unit_t unit) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b00) , 0x00);
}

/**
 * @brief Sets the next byte of the SRAM access address.
 * The first call sets the LSB. The second one the second byte, etc.
 *
 * @param unit The SRAM unit of ExtPack to set the address for.
 * @param address_byte The next byte of the SRAM address to read/write from/to.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t set_ExtPack_SRAM_next_address_byte(unit_t unit, uint8_t address_byte) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b01), address_byte);
}

/**
 * @brief Sets the SRAM access address.
 *
 * @param unit The SRAM unit of ExtPack to set the address for.
 * @param address The address to set. (Only uses the lower 19 bit)
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_SRAM_address(unit_t unit, uint32_t address, uint16_t send_byte_delay_us);

/**
 * @brief Writes the data to the previously set address in the SRAM.
 *
 * @param unit  The SRAM unit of ExtPack to write the data to.
 * @param data The data to write.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t write_ExtPack_SRAM_data(unit_t unit, uint8_t data) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b11), data);
}

/**
 * @brief Sets the address for the SRAM unit and writes the data to it.
 *
 * @param unit The SRAM unit of ExtPack to set the address for and write the data to.
 * @param address The address to set and write the data to. (Only uses the lower 19 bit)
 * @param data The data to write.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t write_ExtPack_SRAM_data_to_address(unit_t unit, uint32_t address, uint8_t data, uint16_t send_byte_delay_us);

/**
 * @brief Request the data from the SRAM at the previously set address.
 * The data will be received async via interrupt.
 *
 * @param unit The SRAM unit of ExtPack to request the data from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t request_ExtPack_SRAM_data(unit_t unit) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b10), 0x00);
}

/**
 * @brief Request the data at the address from the SRAM.
 * The data will be received async via interrupt.
 *
 * @param unit The SRAM unit to request the data from.
 * @param address The address of the data to request. (Only uses the lower 19 bit)
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t request_ExtPack_SRAM_data_from_address(unit_t unit, uint32_t address, uint16_t send_byte_delay_us);

/**
 * @brief Reads the data from the SRAM unit of the previously set address.
 * Waits for the result for a maximum time set by timeout.
 * The recv_data value is only valid if the operation terminated successfully.
 *
 * @param unit The SRAM unit to read the data from.
 * @param recv_data Pointer to store the received data to.
 * @param timeout_us The maximum time to wait for the data in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t read_ExtPack_SRAM_data(unit_t unit, uint8_t* recv_data, uint16_t timeout_us);

/**
 * @brief Reads the data at the address from the SRAM unit.
 * Waits for the result for a maximum time set by timeout.
 * The recv_data value is only valid if the operation terminated successfully.
 *
 * @param unit The SRAM unit to read the data from.
 * @param address The address to read the data from.
 * @param recv_data Pointer to store the received data to.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @param timeout_us The maximum time to wait for the data in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
uint8_t read_ExtPack_SRAM_data_from_address(unit_t unit, uint32_t address, uint8_t* recv_data, uint16_t send_byte_delay_us, uint16_t timeout_us);

#endif //EXTPACK_U_SRAM_H