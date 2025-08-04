#include "ExtPack_U_SPI_Advanced.h"
#include "../Util/Dynamic_Delay.h"

ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data) {
    if(set_ExtPack_SPI_slave(unit, slave_id) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(get_ExtPack_send_duration_us());
    return send_ExtPack_SPI_data(unit, data);
}

ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_byte_delay_us) {
    if(set_ExtPack_SPI_slave(unit, slave_id) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(get_ExtPack_send_duration_us()); // Not affected by bitrate between ExtPack and partner
    return send_String_to_ExtPack(_set_ExtPack_access_mode(unit, 00), data, send_byte_delay_us);
}