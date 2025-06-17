#include "ExtPack_U_SPI.h"
#include "ExtPack_Internal.h"

ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data) {
    if(set_ExtPack_SPI_slave(unit, slave_id) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return send_ExtPack_SPI_data(unit, data);
}

ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    ext_pack_error_t error;
    error = SEND_MAX_ATTEMPTS(set_ExtPack_SPI_slave(unit, slave_id), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    error = send_String_to_ExtPack(unit, data, send_delay_us, max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return EXT_PACK_SUCCESS;
}

uint8_t get_ExtPack_data_SPI_current_slave(unit_t unit) {
    return get_ExtPack_stored_unit_output_values(unit);
}