/**
 * @file ExtPack_Defs.h
 *
 * @brief Unit definitions for the UART Extension Pack.
 *
 * This file defines domain identifiers for the ExtPack system.
 * Including up to 64 distinct units to be addressed. Each unit is assigned a unique numerical value.
 *
 * ## Features:
 * - Defines some general types like unit types, etc.
 * - Defines unit constants from `unit_U00` to `unit_U63`.
 * - Provides a standardized way to reference ExtPack units in the code.
 * - Used in conjunction with the main UART Extension Pack library.
 *
 * @author Markus Remy
 * @date 16.06.2025
 */

#ifndef EXTPACK_DEFS_H
#define EXTPACK_DEFS_H

#include <stdint.h>

/**
 * @typedef unit_type_t
 * Type alias for unit type identifier.
 *
 * @details 'unit_type_t' is defined as uint8_t and is used to represent different types of units,
 * such as GPIO, UART, and Timer. It helps categorize units within the system and is passed to
 * functions that require the identification of unit types.
 */
typedef uint8_t unit_type_t;

#ifndef USED_UNITS
    #define USED_UNITS 64 //Default value if no compiler flag is set
#endif

/**
 * @def EXTPACK_UNDEFINED
 * @brief Constant value representing an undefined unit type.
 */
#define EXTPACK_UNDEFINED 0

/**
 * @def EXTPACK_RESET_UNIT
 * @brief Constant value representing the Reset unit type.
 */
#define EXTPACK_RESET_UNIT 1

/**
 * @def EXTPACK_ERROR_UNIT
 * @brief Constant value representing the Error unit type.
 */
#define EXTPACK_ERROR_UNIT 2

/**
 * @def EXTPACK_ACK_UNIT
 * @brief Constant value representing the Acknowledge unit type.
 */
#define EXTPACK_ACK_UNIT 3

/**
 * @def EXTPACK_GPIO_UNIT
 * @brief Constant value representing the GPIO (General Purpose Input/Output) unit type.
 */
#define EXTPACK_GPIO_UNIT 4

/**
 * @def EXTPACK_UART_UNIT
 * @brief Constant value representing the UART (Universal Asynchronous Receiver/Transmitter) unit type.
 */
#define EXTPACK_UART_UNIT 5

/**
 * @def EXTPACK_TIMER_UNIT
 * @brief Constant value representing the Timer unit type.
 */
#define EXTPACK_TIMER_UNIT 6

/**
 * @def EXTPACK_SPI_UNIT
 * @brief Constant value representing the SPI (Serial Peripheral Interface) unit type.
 */
#define EXTPACK_SPI_UNIT 7

/**
 * @def EXTPACK_I2C_UNIT
 * @brief Constant value representing the I2C/TWI (Inter-Integrated Circuit/Two Wire Interface) unit type.
 */
#define EXTPACK_I2C_UNIT 8

/**
 * @typedef ext_pack_error_t
 * Type alias for ExtPack errors.
 *
 * @details 'ext_pack_error_t' is defined as uint8_t and is used to represent ExtPack errors.
 */
typedef uint8_t ext_pack_error_t;

/**
 * @def EXT_PACK_SUCCESS
 * @brief Success value for ExtPack library functions.
 *
 * This value is returned by functions to indicate that the operation was successful.
 */
#define EXT_PACK_SUCCESS 0

/**
 * @def EXT_PACK_FAILURE
 * @brief Failure value for ExtPack library functions.
 *
 * This value is returned by functions when an error has occurred.
 */
#define EXT_PACK_FAILURE 1

/**
 * @typedef unit_t
 * Type alias for unit identifier.
 *
 * @details 'unit_t' is defined as uint8_t and is used to represent the unique identifiers
 * for various units within the system. It is passed to functions that require a specific unit
 * identifier for communication or management.
 */
typedef uint8_t unit_t;

/**
 * @defgroup ExtPack_Units ExtPack Unit Definitions
 * @brief Defines unique identifiers for ExtPack units.
 *
 * Each unit is assigned a unique numerical identifier from 0 to 63.
 * These constants are used throughout the UART Extension Pack to refer to
 * specific hardware units.
 *
 * Example usage:
 * @code
 * unit_t my_unit = unit_U05;
 * @endcode
 * @{
 */

/**
 * @def unit_UXX
 * @brief Unique identifier for an ExtPack unit.
 *
 * These macros define unit numbers ranging from 0 to 63.
 */
#define unit_U00 0
#define unit_U01 1
#define unit_U02 2
#define unit_U03 3
#define unit_U04 4
#define unit_U05 5
#define unit_U06 6
#define unit_U07 7
#define unit_U08 8
#define unit_U09 9
#define unit_U10 10
#define unit_U11 11
#define unit_U12 12
#define unit_U13 13
#define unit_U14 14
#define unit_U15 15
#define unit_U16 16
#define unit_U17 17
#define unit_U18 18
#define unit_U19 19
#define unit_U20 20
#define unit_U21 21
#define unit_U22 22
#define unit_U23 23
#define unit_U24 24
#define unit_U25 25
#define unit_U26 26
#define unit_U27 27
#define unit_U28 28
#define unit_U29 29
#define unit_U30 30
#define unit_U31 31
#define unit_U32 32
#define unit_U33 33
#define unit_U34 34
#define unit_U35 35
#define unit_U36 36
#define unit_U37 37
#define unit_U38 38
#define unit_U39 39
#define unit_U40 40
#define unit_U41 41
#define unit_U42 42
#define unit_U43 43
#define unit_U44 44
#define unit_U45 45
#define unit_U46 46
#define unit_U47 47
#define unit_U48 48
#define unit_U49 49
#define unit_U50 50
#define unit_U51 51
#define unit_U52 52
#define unit_U53 53
#define unit_U54 54
#define unit_U55 55
#define unit_U56 56
#define unit_U57 57
#define unit_U58 58
#define unit_U59 59
#define unit_U60 60
#define unit_U61 61
#define unit_U62 62
#define unit_U63 63

/** @} */  // End of ExtPack_Units group

#endif //EXTPACK_DEFS_H
