/**
 * @file ExtPack_U_SRAM_Advanced.h
 *
 * @brief Advanced SRAM communication helpers for the SRAM unit in the ExtPack library.
 *
 * @layer Service
 *
 * @details This header provides blocking and non-blocking functions for accessing SRAM via ExtPack.
 * Includes address setting, reading and writing data with optional delays and timeout handling.
 *
 * ## Provided Functions:
 * - set_ExtPack_SRAM_address: Set the address for a SRAM unit.
 * - write_ExtPack_SRAM_data_to_address: Set the address and write a byte to it.
 * - request_ExtPack_SRAM_data_from_address: Request data at an address (non-blocking).
 * - read_ExtPack_SRAM_data_from_address: Read data at an address with timeout.
 * - read_ExtPack_SRAM_data: Read from previously set address with timeout.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_U_SRAM_ADVANCED_H
#define EXTPACK_U_SRAM_ADVANCED_H

#include "../Util/ExtPack_U_SRAM.h"

/**
 * @defgroup SRAM_Unit SRAM Unit
 * @brief Functionality of the ISSI_IS61WV5128BLL_SRAM Unit of ExtPack
 * @{
 */

/**
 * @brief Sets the SRAM access address.
 *
 * @layer Service
 *
 * @param unit The SRAM unit of ExtPack to set the address for.
 * @param address The address to set. (Only uses the lower 19 bit)
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_SRAM_address(unit_t unit, uint32_t address, uint16_t send_byte_delay_us);

/**
 * @brief Sets the address for the SRAM unit and writes the data to it.
 *
 * @layer Service
 *
 * @param unit The SRAM unit of ExtPack to set the address for and write the data to.
 * @param address The address to set and write the data to. (Only uses the lower 19 bit)
 * @param data The data to write.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t write_ExtPack_SRAM_data_to_address(unit_t unit, uint32_t address, uint8_t data, uint16_t send_byte_delay_us);

/**
 * @brief Request the data at the address from the SRAM.
 * The data will be received async via interrupt.
 *
 * @layer Service
 *
 * @param unit The SRAM unit to request the data from.
 * @param address The address of the data to request. (Only uses the lower 19 bit)
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t request_ExtPack_SRAM_data_from_address(unit_t unit, uint32_t address, uint16_t send_byte_delay_us);

/**
 * @brief Reads the data at the address from the SRAM unit.
 * Waits for the result for a maximum time set by timeout.
 * The recv_data value is only valid if the operation terminated successfully.
 *
 * @layer Service
 *
 * @param unit The SRAM unit to read the data from.
 * @param address The address to read the data from.
 * @param recv_data Pointer to store the received data to.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @param timeout_us The maximum time to wait for the data in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
uint8_t read_ExtPack_SRAM_data_from_address(unit_t unit, uint32_t address, uint8_t* recv_data, uint16_t send_byte_delay_us, uint16_t timeout_us);

/**
 * @brief Reads the data from the SRAM unit of the previously set address.
 * Waits for the result for a maximum time set by timeout.
 * The recv_data value is only valid if the operation terminated successfully.
 *
 * @layer Service
 *
 * @param unit The SRAM unit to read the data from.
 * @param recv_data Pointer to store the received data to.
 * @param timeout_us The maximum time to wait for the data in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t read_ExtPack_SRAM_data(unit_t unit, uint8_t* recv_data, uint16_t timeout_us);

/** @} */

#endif //EXTPACK_U_SRAM_ADVANCED_H