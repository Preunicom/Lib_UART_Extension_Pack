/**
 * @file ExtPack_U_GPIO.h
 *
 * @brief Header file for GPIO unit functions of the ExtPack library.
 *
 * ## Features:
 * - Request input pin states using refresh_ExtPack_gpio_data.
 * - Retrieve last input values using get_ExtPack_data_gpio_in.
 * - Retrieve last output values using get_ExtPack_data_gpio_out.
 * - Set output pin values using set_ExtPack_gpio_out.
 *
 * @author Markus Remy
 * @date 17.06.2025
 */

#ifndef EXTPACK_U_GPIO_H
#define EXTPACK_U_GPIO_H

#include "../Core/ExtPack.h"

/**
 * @defgroup GPIO_Unit GPIO Unit
 * @brief Functionality of the GPIO Unit of ExtPack
 * @{
 */

/**
 * @brief Requests the current input pin states of a GPIO unit.
 *
 * @param unit The target GPIO unit.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
static inline ext_pack_error_t refresh_ExtPack_gpio_data(unit_t unit) {
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 01), 0x00);
}

/**
 * @brief Retrieves the last received values of the given ExtPack GPIO unit's input pins.
 *
 * @details No communication with ExtPack is initiated as the values were saved at the last data change.
 *
 * @param unit The GPIO unit of ExtPack from which the values are retrieved.
 * @return The last received input values of the GPIO unit.
 */
uint8_t get_ExtPack_data_gpio_in(unit_t unit);

/**
 * @brief Retrieves the last sent values of the given ExtPack GPIO unit's output pins.
 *
 * @details No communication with ExtPack is initiated as the values were saved at the last data change.
 *
 * @param unit The GPIO unit of ExtPack from which the values are retrieved.
 * @return The last sent output values of the GPIO unit.
 */
uint8_t get_ExtPack_data_gpio_out(unit_t unit);

/**
 * @brief Sets the GPIO output pins of the ExtPack unit to the data value.
 *
 * @param unit The unit to set the GPIO pins for.
 * @param data The GPIO pin register value to set.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t set_ExtPack_gpio_out(unit_t unit, uint8_t data);

/** @} */

#endif //EXTPACK_U_GPIO_H
