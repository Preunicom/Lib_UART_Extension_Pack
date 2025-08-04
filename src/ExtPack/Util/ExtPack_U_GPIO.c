#include "ExtPack_U_GPIO.h"
#include "../Core/ExtPack_Internal.h"

uint8_t get_ExtPack_data_gpio_in(unit_t unit) {
    return get_ExtPack_stored_unit_input_values(unit);
}

uint8_t get_ExtPack_data_gpio_out(unit_t unit) {
    return get_ExtPack_stored_unit_output_values(unit);
}

ext_pack_error_t set_ExtPack_gpio_out(unit_t unit, uint8_t data) {
    unit_data[unit].output_values = data; // Save set data locally
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data);
}