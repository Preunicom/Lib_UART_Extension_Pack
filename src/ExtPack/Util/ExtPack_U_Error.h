/**
 * @file ExtPack_U_Error.h
 *
 * @brief Header file for the Error unit functions of the ExtPack library.
 *
 * @layer Util
 *
 * ## Features:
 * - Defines error type constants for various ExtPack communication and processing errors.
 * - Provides function for retrieving the last error encountered by the ExtPack.
 *
 * @author Markus Remy
 * @date 20.06.2025
 */

#ifndef EXTPACK_U_ERROR_H
#define EXTPACK_U_ERROR_H

#include "../Core/ExtPack_Defs.h"

/**
 * @defgroup Error_Unit Error Unit
 * @brief Functionality of the Error Unit of ExtPack
 * @{
 */

/**
 * @brief Type alias for ExtPack errors of the ExtPack itself.
 *
 * @layer Util
 *
 * @details 'error_unit_error_type_t' is defined as uint8_t and is used to represent errors sent by the ExtPack itself.
 */
typedef uint8_t error_unit_error_type_t;

/**
 * @def ERROR_UNIT_ERROR_RECEIVING_FROM_HOST
 * @brief Constant value representing an error sent by the ExtPack because of an error while receiving UART data from host.
 * @layer Util
 */
#define ERROR_UNIT_ERROR_RECEIVING_FROM_HOST 1

/**
 * @def ERROR_UNIT_ERROR_SENDING_TO_HOST
 * @brief Constant value representing an error sent by the ExtPack because of an error while sending UART data to host.
 * @layer Util
 */
#define ERROR_UNIT_ERROR_SENDING_TO_HOST 2

/**
 * @def ERROR_UNIT_ERROR_PROCESSING
 * @brief Constant value representing an error sent by the ExtPack because of an error while processing a command/data.
 * @layer Util
 */
#define ERROR_UNIT_ERROR_PROCESSING 4

/**
 * @brief Returns the last error received from ExtPack.
 *
 * @layer Util
 *
 * @details The returned error can be compared with the ERROR_UNIT_ERROR_<TYPE> macro to determine the type of the error.
 *
 * @return The last error.
 */
error_unit_error_type_t get_ExtPack_error();

/** @} */

#endif //EXTPACK_U_ERROR_H
