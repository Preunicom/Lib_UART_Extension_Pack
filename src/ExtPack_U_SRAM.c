#include "ExtPack_U_SRAM.h"
#include <util/delay.h>
#include "ExtPack_Internal.h"
#include "Dynamic_Delay.h"
#include "ExtPack_Events.h"

ext_pack_error_t set_ExtPack_SRAM_address(unit_t unit, uint32_t address, uint16_t send_byte_delay_us) {
    if (reset_ExtPack_SRAM_address(unit) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    uint8_t address_string[5];
    address_string[0] = address;
    address_string[1] = address >> 8;
    address_string[2] = address >> 16;
    address_string[3] = address >> 24;
    address_string[4] = '\0'; // String end
    return send_String_to_ExtPack(unit, address_string, send_byte_delay_us);
}

ext_pack_error_t write_ExtPack_SRAM_data_to_address(unit_t unit, uint32_t address, uint8_t data, uint16_t send_byte_delay_us) {
    if (set_ExtPack_SRAM_address(unit, address, send_byte_delay_us) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_byte_delay_us);
    return write_ExtPack_SRAM_data(unit, data);
}

ext_pack_error_t request_ExtPack_SRAM_data_from_address(unit_t unit, uint32_t address, uint16_t send_byte_delay_us) {
    if (set_ExtPack_SRAM_address(unit, address, send_byte_delay_us) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_byte_delay_us);
    return request_ExtPack_SRAM_data(unit);
}

ext_pack_error_t read_ExtPack_SRAM_data(unit_t unit, uint8_t* recv_data, uint16_t timeout_us) {
    for (uint16_t i = 0; i < timeout_us; i++) {
        _delay_us(1);
        if (get_ExtPack_event(unit)) {
            clear_ExtPack_event(unit);
            *recv_data = unit_data[unit].input_values;
            return EXT_PACK_SUCCESS;
        }
    }
    // Timeout exceeded
    return EXT_PACK_FAILURE;
}

uint8_t read_ExtPack_SRAM_data_from_address(unit_t unit, uint32_t address, uint8_t* recv_data, uint16_t send_byte_delay_us, uint16_t timeout_us) {
    if (set_ExtPack_SRAM_address(unit, address, send_byte_delay_us) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_byte_delay_us);
    return read_ExtPack_SRAM_data(unit, recv_data, timeout_us);
}