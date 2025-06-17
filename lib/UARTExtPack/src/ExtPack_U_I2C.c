#include "ExtPack_U_I2C.h"
#include "ExtPack_Internal.h"

uint8_t get_ExtPack_data_I2C_current_partner_adr(unit_t unit) {
    return 0b01111111 & get_ExtPack_stored_unit_output_values(unit);
}

uint8_t get_ExtPack_data_I2C_last_received_data(unit_t unit) {
    return get_ExtPack_stored_unit_input_values(unit);
}

ext_pack_error_t receive_ExtPack_I2C_data_from_partner(unit_t unit, uint8_t partner_adr) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return receive_ExtPack_I2C_data(unit);
}

ext_pack_error_t send_ExtPack_I2C_data_to_partner(unit_t unit, uint8_t partner_adr, uint8_t data) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return send_ExtPack_I2C_data(unit, data);
}

ext_pack_error_t send_ExtPack_I2C_String_to_partner(unit_t unit, uint8_t partner_adr, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    ext_pack_error_t error;
    error = SEND_MAX_ATTEMPTS(set_ExtPack_I2C_partner_adr(unit, partner_adr), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    error = send_String_to_ExtPack(unit, data, send_delay_us, max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return EXT_PACK_SUCCESS;
}
