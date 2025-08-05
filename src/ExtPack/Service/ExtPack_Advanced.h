/**
 * @file ExtPack_Advanced.h
 *
 * @brief Advanced extension functions for the ExtPack library.
 *
 * @layer Service
 *
 * @details This header provides higher-level helper functions for ExtPack units,
 * including utilities to send strings over ExtPack with defined byte delays.
 *
 * ## Provided Functions:
 * - send_String_to_ExtPack: Send null-terminated strings with a specified delay between bytes.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_ADVANCED_H
#define EXTPACK_ADVANCED_H

#include "../Core/ExtPack.h" // Including the basic functionality

/**
 * @brief Sends the given String until '\0' to ExtPack.
 * If a send char operation fails the function aborts and returns an error.
 *
 * @layer Service
 *
 * @param unit The ExtPack unit to which the data should be sent. Including the correct set access mode for sending.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_byte_delay_us The delay between sending two bytes in us.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending an uint8_t because of an error while sending.
 */
ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint16_t send_byte_delay_us);

#endif //EXTPACK_ADVANCED_H
