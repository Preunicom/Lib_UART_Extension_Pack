/**
 * @file ExtPack_U_SRAM.h
 *
 * @brief Header file for SRAM access operations of the ExtPack library.
 *
 * @layer Util
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

#include "../Core/ExtPack.h"

/**
 * @defgroup SRAM_Unit SRAM Unit
 * @brief Functionality of the ISSI_IS61WV5128BLL_SRAM Unit of ExtPack
 * @{
 */

/**
 * @brief Resets the address of the SRAM unit to zero and set the next address block to write to zero.
 *
 * @layer Util
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
 * @layer Util
 *
 * @param unit The SRAM unit of ExtPack to set the address for.
 * @param address_byte The next byte of the SRAM address to read/write from/to.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t set_ExtPack_SRAM_next_address_byte(unit_t unit, uint8_t address_byte) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b01), address_byte);
}

/**
 * @brief Writes the data to the previously set address in the SRAM.
 *
 * @layer Util
 *
 * @param unit  The SRAM unit of ExtPack to write the data to.
 * @param data The data to write.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t write_ExtPack_SRAM_data(unit_t unit, uint8_t data) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b11), data);
}

/**
 * @brief Request the data from the SRAM at the previously set address.
 * The data will be received async via interrupt.
 *
 * @layer Util
 *
 * @param unit The SRAM unit of ExtPack to request the data from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t request_ExtPack_SRAM_data(unit_t unit) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 0b10), 0x00);
}

/**
 * @brief Function to retrieve the last received SRAM data byte of the given ExtPack SRAM unit.
 *
 * @layer Util
 *
 * @param unit The unit to get the data from
 * @return The last received SRAM data from this unit.
 */
uint8_t get_last_received_ExtPack_SRAM_data(unit_t unit);

/** @} */

#endif //EXTPACK_U_SRAM_H