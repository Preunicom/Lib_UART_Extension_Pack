/**
 * @file ExtPack_U_Acknowledge.h
 *
 * @brief Header file for Acknowledge unit functions of the ExtPack library.
 *
 * ## Features:
 * - Retrieve the ACK unit state with get_ExtPack_ack_state.
 * - Clear the ACK event with clear_ExtPack_ack_event.
 * - Retrieve and reset the ACK event with get_ExtPack_ack_event.
 * - Wait for specific ACK data with wait_for_ExtPack_ACK_data.
 * - Wait for any ACK with wait_for_ExtPack_ACK.
 * - Retrieve the last ACK data with get_ExtPack_ack_data.
 * - Enable or disable the ACK unit with set_ExtPack_ACK_enable.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_ACKNOWLEDGE_H
#define EXTPACK_U_ACKNOWLEDGE_H

#include "ExtPack.h"
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

/**
 * @brief Retrieves the status of the ExtPack ACK unit (unit_U02).
 *
 * @return active (1) or inactive (0)
 */
uint8_t get_ExtPack_ack_state();

/**
 * @brief Clears the acknowledgement received event of the ExtPack ACK unit (unit_U02).
 */
void clear_ExtPack_ack_event();

/**
 * @brief Retrieves the acknowledge received event of the ExtPack ACK unit (unit_U02).
 * Additionally, it clears the event.
 *
 * @return event set (1) or event not set (0)
 */
uint8_t get_ExtPack_ack_event();

/**
 * @brief Blocks until an acknowledgment of the ACK unit (unit_U02) of ExtPack gets received or the timeout is over.
 * Also checks ACK data.
 * Resets ACK event to not set.
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
 * @param timeout_us Maximum time awaited in us until the function returns an error.
 * @return EXT_PACK_SUCCESS if an acknowledgement for the given unit is received before timeout,
 *         EXT_PACK_FAILURE if the timeout is reached.
 */
ext_pack_error_t wait_for_ExtPack_ACK(uint16_t timeout_us);

/**
 * @brief Alias to retrieve the acknowledgment received data of the ExtPack ACK unit.
 *
 * @return The data the last received ack was for.
 */
uint8_t get_ExtPack_ack_data();

/**
 * @brief Alias to send enable or disable command to the ExtPack ACK unit.
 *
 * @param enable Enable (>=1) or Disable (0) the unit.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_ACK_enable(uint8_t enable);

#endif //EXTPACK_U_ACKNOWLEDGE_H
