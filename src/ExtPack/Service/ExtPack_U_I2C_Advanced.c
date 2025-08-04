#include "ExtPack_U_I2C_Advanced.h"
#include "../Util/Dynamic_Delay.h"

ext_pack_error_t receive_ExtPack_I2C_data_from_partner(unit_t unit, uint8_t partner_adr) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(get_ExtPack_send_duration_us());
    return receive_ExtPack_I2C_data(unit);
}

ext_pack_error_t send_ExtPack_I2C_data_to_partner(unit_t unit, uint8_t partner_adr, uint8_t data) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(get_ExtPack_send_duration_us());
    return send_ExtPack_I2C_data(unit, data);
}

ext_pack_error_t send_ExtPack_I2C_String_to_partner(unit_t unit, uint8_t partner_adr, const uint8_t* data, uint16_t send_byte_delay_us) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(get_ExtPack_send_duration_us()); // Not affected by bitrate between ExtPack and partner
    return send_String_to_ExtPack(unit, data, send_byte_delay_us);
}