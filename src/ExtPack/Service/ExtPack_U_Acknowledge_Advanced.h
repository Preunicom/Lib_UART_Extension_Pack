/**
 * @file ExtPack_U_Acknowledge_Advanced.h
 *
 * @brief Advanced acknowledgment handling for the ACK unit in the ExtPack library.
 *
 * This header provides blocking functions to wait for acknowledgments from the ACK unit,
 * with optional data checking and timeout handling.
 *
 * ## Provided Functions:
 * - wait_for_ExtPack_ACK_data: Waits for an ACK and verifies the received data.
 * - wait_for_ExtPack_ACK: Waits for any ACK without checking the data.
 *
 * @author Markus Remy
 * @date 04.08.2025
 */

#ifndef EXTPACK_U_ACKNOWLEDGE_ADVANCED_H
#define EXTPACK_U_ACKNOWLEDGE_ADVANCED_H

#include "../Util/ExtPack_U_Acknowledge.h"

/**
 * @defgroup ACK_Unit Acknowledge Unit
 * @brief Functionality of the Acknowledge Unit of ExtPack
 * @{
 */

/**
 * @brief Blocks until an acknowledgment of the ACK unit (unit_U02) of ExtPack gets received or the timeout is over.
 * Also checks ACK data.
 * Resets ACK event to not set.
 *
 * @note The ACK_unit ISR is executed _before_ the wait_for_ExtPack_ACK() processes the acknowledgement.
 *
 * @param data The data the acknowledgement is expected to be.
 * @param timeout_us Maximum time awaited in us until the function returns an error.
 * @return EXT_PACK_SUCCESS if the expected acknowledgement is received before timeout,
 *         EXT_PACK_FAILURE if the acknowledgement has wrong data or the timeout is reached.
 */
ext_pack_error_t wait_for_ExtPack_ACK_data(uint8_t data, uint16_t timeout_us);

/**
 * @brief Blocks until an acknowledgment of the ACK unit (unit_U02) of ExtPack gets received or the timeout is over
 * Resets ACK event to not set.
 * Does not compare ACK data.
 *
 * @note The ACK_unit ISR is executed _before_ the wait_for_ExtPack_ACK() processes the acknowledgement.
 *
 * @param timeout_us Maximum time awaited in us until the function returns an error.
 * @return EXT_PACK_SUCCESS if an acknowledgement for the given unit is received before timeout,
 *         EXT_PACK_FAILURE if the timeout is reached.
 */
ext_pack_error_t wait_for_ExtPack_ACK(uint16_t timeout_us);

/** @} */

#endif //EXTPACK_U_ACKNOWLEDGE_ADVANCED_H