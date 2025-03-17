/**
 * @file UARTExtPack_UnitDefs.h
 *
 * @brief Unit definitions for the UART Extension Pack.
 *
 * This file defines unit identifiers for the ExtPack system, allowing up to 64
 * distinct units to be addressed. Each unit is assigned a unique numerical value.
 *
 * ## Features:
 * - Defines unit constants from `unit_U00` to `unit_U63`.
 * - Provides a standardized way to reference ExtPack units in the code.
 * - Used in conjunction with the main UART Extension Pack library.
 *
 * @author Markus Remy
 * @date 16.03.2025
 */

#ifndef LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_UNITDEFS_H
#define LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_UNITDEFS_H

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

#endif //LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_UNITDEFS_H
