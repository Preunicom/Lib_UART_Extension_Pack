/**
 * @file ExtPack_Defs.h
 *
 * @brief Unit definitions for the UART Extension Pack.
 *
 * @layer Core
 *
 * @details This file defines domain identifiers for the ExtPack system.
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

#ifndef USED_UNITS
    /**
     * @def USED_UNITS
     * @brief Defines the amount of units used by the ExtPack.
     *
     * This is needed to take the correct amount of storage for the unit data.
     */
    #define USED_UNITS 64 //Default value if no compiler flag is set
#endif

/**
 * @defgroup ExtPack_Unit_Types ExtPack Unit Type Definitions
 * @brief Definitions of unit types.
 * @{
 */

/**
 * @typedef unit_type_t
 * Type alias for unit type identifier.
 *
 * @details 'unit_type_t' is defined as uint8_t and is used to represent different types of units,
 * such as GPIO, UART, and Timer. It helps categorize units within the system and is passed to
 * functions that require the identification of unit types.
 */
typedef uint8_t unit_type_t;

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
 * @def EXTPACK_SRAM_UNIT
 * @brief Constant value representing the SRAM (static random access memory) unit type.
 * SRAM_Unit is short for ISSI_IS61WV5128BLL_SRAM_Unit
 */
#define EXTPACK_SRAM_UNIT 9

/** @} */

/**
 * @defgroup ExtPack_Errors ExtPack Error Definitions
 * @brief Definitions of general ExtPack library errors and error types.
 * @{
 */

/**
 * @typedef ext_pack_error_t
 * @brief Type alias for ExtPack function errors.
 *
 * @details 'ext_pack_error_t' is defined as uint8_t and is used to represent ExtPack function errors.
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

/** @} */

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
 * @typedef unit_t
 * Type alias for unit identifier.
 *
 * @details 'unit_t' is defined as uint8_t and is used to represent the unique identifiers
 * for various units within the system. It is passed to functions that require a specific unit
 * identifier for communication or management.
 */
typedef uint8_t unit_t;

/**
 * @def unit_U00
 * @brief Identifier for unit 00.
 */
#define unit_U00 0
/**
 * @def unit_U01
 * @brief Identifier for unit 01.
 */
#define unit_U01 1
/**
 * @def unit_U02
 * @brief Identifier for unit 02.
 */
#define unit_U02 2
/**
 * @def unit_U03
 * @brief Identifier for unit 03.
 */
#define unit_U03 3
/**
 * @def unit_U04
 * @brief Identifier for unit 04.
 */
#define unit_U04 4
/**
 * @def unit_U05
 * @brief Identifier for unit 05.
 */
#define unit_U05 5
/**
 * @def unit_U06
 * @brief Identifier for unit 06.
 */
#define unit_U06 6
/**
 * @def unit_U07
 * @brief Identifier for unit 07.
 */
#define unit_U07 7
/**
 * @def unit_U08
 * @brief Identifier for unit 08.
 */
#define unit_U08 8
/**
 * @def unit_U09
 * @brief Identifier for unit 09.
 */
#define unit_U09 9
/**
 * @def unit_U10
 * @brief Identifier for unit 10.
 */
#define unit_U10 10
/**
 * @def unit_U11
 * @brief Identifier for unit 11.
 */
#define unit_U11 11
/**
 * @def unit_U12
 * @brief Identifier for unit 12.
 */
#define unit_U12 12
/**
 * @def unit_U13
 * @brief Identifier for unit 13.
 */
#define unit_U13 13
/**
 * @def unit_U14
 * @brief Identifier for unit 14.
 */
#define unit_U14 14
/**
 * @def unit_U15
 * @brief Identifier for unit 15.
 */
#define unit_U15 15
/**
 * @def unit_U16
 * @brief Identifier for unit 16.
 */
#define unit_U16 16
/**
 * @def unit_U17
 * @brief Identifier for unit 17.
 */
#define unit_U17 17
/**
 * @def unit_U18
 * @brief Identifier for unit 18.
 */
#define unit_U18 18
/**
 * @def unit_U19
 * @brief Identifier for unit 19.
 */
#define unit_U19 19
/**
 * @def unit_U20
 * @brief Identifier for unit 20.
 */
#define unit_U20 20
/**
 * @def unit_U21
 * @brief Identifier for unit 21.
 */
#define unit_U21 21
/**
 * @def unit_U22
 * @brief Identifier for unit 22.
 */
#define unit_U22 22
/**
 * @def unit_U23
 * @brief Identifier for unit 23.
 */
#define unit_U23 23
/**
 * @def unit_U24
 * @brief Identifier for unit 24.
 */
#define unit_U24 24
/**
 * @def unit_U25
 * @brief Identifier for unit 25.
 */
#define unit_U25 25
/**
 * @def unit_U26
 * @brief Identifier for unit 26.
 */
#define unit_U26 26
/**
 * @def unit_U27
 * @brief Identifier for unit 27.
 */
#define unit_U27 27
/**
 * @def unit_U28
 * @brief Identifier for unit 28.
 */
#define unit_U28 28
/**
 * @def unit_U29
 * @brief Identifier for unit 29.
 */
#define unit_U29 29
/**
 * @def unit_U30
 * @brief Identifier for unit 30.
 */
#define unit_U30 30
/**
 * @def unit_U31
 * @brief Identifier for unit 31.
 */
#define unit_U31 31
/**
 * @def unit_U32
 * @brief Identifier for unit 32.
 */
#define unit_U32 32
/**
 * @def unit_U33
 * @brief Identifier for unit 33.
 */
#define unit_U33 33
/**
 * @def unit_U34
 * @brief Identifier for unit 34.
 */
#define unit_U34 34
/**
 * @def unit_U35
 * @brief Identifier for unit 35.
 */
#define unit_U35 35
/**
 * @def unit_U36
 * @brief Identifier for unit 36.
 */
#define unit_U36 36
/**
 * @def unit_U37
 * @brief Identifier for unit 37.
 */
#define unit_U37 37
/**
 * @def unit_U38
 * @brief Identifier for unit 38.
 */
#define unit_U38 38
/**
 * @def unit_U39
 * @brief Identifier for unit 39.
 */
#define unit_U39 39
/**
 * @def unit_U40
 * @brief Identifier for unit 40.
 */
#define unit_U40 40
/**
 * @def unit_U41
 * @brief Identifier for unit 41.
 */
#define unit_U41 41
/**
 * @def unit_U42
 * @brief Identifier for unit 42.
 */
#define unit_U42 42
/**
 * @def unit_U43
 * @brief Identifier for unit 43.
 */
#define unit_U43 43
/**
 * @def unit_U44
 * @brief Identifier for unit 44.
 */
#define unit_U44 44
/**
 * @def unit_U45
 * @brief Identifier for unit 45.
 */
#define unit_U45 45
/**
 * @def unit_U46
 * @brief Identifier for unit 46.
 */
#define unit_U46 46
/**
 * @def unit_U47
 * @brief Identifier for unit 47.
 */
#define unit_U47 47
/**
 * @def unit_U48
 * @brief Identifier for unit 48.
 */
#define unit_U48 48
/**
 * @def unit_U49
 * @brief Identifier for unit 49.
 */
#define unit_U49 49
/**
 * @def unit_U50
 * @brief Identifier for unit 50.
 */
#define unit_U50 50
/**
 * @def unit_U51
 * @brief Identifier for unit 51.
 */
#define unit_U51 51
/**
 * @def unit_U52
 * @brief Identifier for unit 52.
 */
#define unit_U52 52
/**
 * @def unit_U53
 * @brief Identifier for unit 53.
 */
#define unit_U53 53
/**
 * @def unit_U54
 * @brief Identifier for unit 54.
 */
#define unit_U54 54
/**
 * @def unit_U55
 * @brief Identifier for unit 55.
 */
#define unit_U55 55
/**
 * @def unit_U56
 * @brief Identifier for unit 56.
 */
#define unit_U56 56
/**
 * @def unit_U57
 * @brief Identifier for unit 57.
 */
#define unit_U57 57
/**
 * @def unit_U58
 * @brief Identifier for unit 58.
 */
#define unit_U58 58
/**
 * @def unit_U59
 * @brief Identifier for unit 59.
 */
#define unit_U59 59
/**
 * @def unit_U60
 * @brief Identifier for unit 60.
 */
#define unit_U60 60
/**
 * @def unit_U61
 * @brief Identifier for unit 61.
 */
#define unit_U61 61
/**
 * @def unit_U62
 * @brief Identifier for unit 62.
 */
#define unit_U62 62
/**
 * @def unit_U63
 * @brief Identifier for unit 63.
 */
#define unit_U63 63

/** @} */  // End of ExtPack_Units group

#endif //EXTPACK_DEFS_H