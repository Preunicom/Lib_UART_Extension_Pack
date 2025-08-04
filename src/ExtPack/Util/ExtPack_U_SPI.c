#include "ExtPack_U_SPI.h"
#include "../Core/ExtPack_Internal.h"

ext_pack_error_t set_ExtPack_SPI_slave(unit_t unit, uint8_t slave_id) {
    unit_data[unit].output_values = slave_id;
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit, 01), slave_id);
}

uint8_t get_ExtPack_data_SPI_current_slave(unit_t unit) {
    return get_ExtPack_stored_unit_output_values(unit);
}

uint8_t get_last_received_ExtPack_SPI_data(unit_t unit) {
    return get_ExtPack_stored_unit_input_values(unit);
}