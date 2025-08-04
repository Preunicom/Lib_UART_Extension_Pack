/**
 * @file ExtPack_Internal.h
 *
 * @brief Header file for internal definitions of the ExtPack library.
 *
 * This file provides internal macros, data structures, and inline functions
 * for managing units and their data storage within the ExtPack system.
 *
 * @warning This file is only for access for ExtPack library functions. The user should not directly use this header file.
 *
 * ## Features:
 * - Defines ACC_MODE0_BIT and ACC_MODE1_BIT macros for access mode bits.
 * - Defines ACK_EVENT and ACK_STATE macros for ACK unit events and states.
 * - Declares the `unit` structure and `units` array for unit configurations.
 * - Declares the `unit_data_storage` structure and `unit_data` array for I/O storage.
 * - Provides inline getters for stored input and output values: get_ExtPack_stored_unit_input_values and get_ExtPack_stored_unit_output_values.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_INTERNAL_H
#define EXTPACK_INTERNAL_H

#include "ExtPack.h"

/**
 * @def ACC_MODE0_BIT
 * @brief Defines bit 6 of the unit_number package to represent the lower access mode bit.
 */
#define ACC_MODE0_BIT 6

/**
 * @def ACC_MODE1_BIT
 * @brief Defines bit 7 of the unit_number package to represent the higher access mode bit.
 */
#define ACC_MODE1_BIT 7

/**
 * @struct unit
 * Structure representing a unit in ExtPack.
 *
 * @param unit_type The type of the unit (e.g., GPIO, UART, Timer).
 * @param custom_ISR The custom interrupt service routine (ISR) function
 *        that the unit will call if an interrupt occurs.
 */
struct unit {
    /**
     * @brief Type of the unit (e.g., GPIO, UART, Timer).
     */
    unit_type_t unit_type;
    /**
     * @brief Pointer to a custom interrupt service routine (ISR).
     *
     * This function is called by the system when the unit triggers an interrupt as callback function.
     * The parameters are the unit identifier and the associated data byte.
     *
     * @warning Interrupts will not be enabled before calling the custom_ISR.
     */
    void (*custom_ISR)(unit_t, uint8_t);
};

/**
 * @var units
 * Array of unit structures.
 *
 * @brief This array holds the unit structures used in the system. The array is initialized
 *        with zero values and its size is determined by the macro 'USED_UNITS'.
 *
 * @note Each element of the array represents a unit within the system. The number of units
 *       in use is defined by the 'USED_UNITS' macro, and the array is pre-initialized to ensure
 *       all unit fields are set to zero on startup.
 */
extern struct unit units[USED_UNITS];

/**
 * @struct unit_data_storage
 * Structure representing input and output values of a units.
 *
 * @brief This structure holds the input and output values for a unit. It is used to
 *        store the current states of the unit.
 *
 * @note The structure contains two fields:
 *       - `input_values`: Stores the incoming values of the unit.
 *       - `output_values`: Stores the outgoing values of the unit.
 *
 * @details The output_values of the ACK unit (unit_U02) is special:
 *          - Bit 0: ACK state (0: not enabled / 1: enabled)
 *          - Bit 1-6: Unused
 *          - Bit 7: ACK received event (0: not set / 1: set)
 */
struct unit_data_storage {
    uint8_t input_values;   /**< Incoming values of the unit */
    uint8_t output_values;  /**< Outgoing values of the unit */
};

/**
 * @var unit_data
 * Array of unit data structures for multiple units.
 *
 * @brief This array stores the incoming and outgoing data for each unit. Each element in the array
 *        corresponds to a specific unit and stores its incoming and outgoing states, provided that
 *        the unit uses the storage.
 *
 * @note The array is initialized with zeros, and its size is determined by the `USED_UNITS` constant.
 *       It is used to manage and store the data for all active units which needs it within the system.
 */
extern struct unit_data_storage unit_data[USED_UNITS];

/**
 * Returns the stored output data of the given unit of ExtPack.
 * The data has to be interpreted depending on the unit type.
 *
 * @param unit The ExtPack unit.
 * @return The stored output data.
 */
inline uint8_t get_ExtPack_stored_unit_output_values(unit_t unit) {
    return unit_data[unit].output_values;
}

/**
 * Returns the stored input data of the given unit of ExtPack.
 * The data has to be interpreted depending on the unit type.
 *
 * @param unit The ExtPack unit.
 * @return The stored input data.
 */
inline uint8_t get_ExtPack_stored_unit_input_values(unit_t unit) {
    return unit_data[unit].input_values;
}

/**
 * Processes the data received from ExtPAck via UART.
 *
 * @param unit The received unit byte.
 * @param data The received data byte
 */
void process_received_ExtPack_data(unit_t unit, uint8_t data);

#endif //EXTPACK_INTERNAL_H
