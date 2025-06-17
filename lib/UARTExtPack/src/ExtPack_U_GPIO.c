#include "ExtPack_U_GPIO.h"
#include "ExtPack_Internal.h"

uint8_t get_ExtPack_data_gpio_in(unit_t unit) {
    return get_ExtPack_stored_unit_input_values(unit);
}

uint8_t get_ExtPack_data_gpio_out(unit_t unit) {
    return get_ExtPack_stored_unit_output_values(unit);
}