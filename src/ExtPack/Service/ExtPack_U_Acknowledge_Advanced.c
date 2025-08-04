#include "ExtPack_U_Acknowledge_Advanced.h"
#include "../Core/ExtPack_Internal.h"
#include "../Core/ExtPack_Events.h"
#include <util/delay.h>

ext_pack_error_t wait_for_ExtPack_ACK_data(uint8_t data, uint16_t timeout_us) {
    for (uint16_t i = 0; i < timeout_us; i++) {
        _delay_us(1);
        if (get_ExtPack_event(unit_U02)) {
            // Acknowledgement received
            clear_ExtPack_event(unit_U02);
            if (unit_data[unit_U02].input_values == data) {
                // Matching acknowledgment data
                return EXT_PACK_SUCCESS;
            } else {
                // Wrong acknowledgment data
                return EXT_PACK_FAILURE;
            }
        }
    }
    // Timeout exceeded
    return EXT_PACK_FAILURE;
}

ext_pack_error_t wait_for_ExtPack_ACK(uint16_t timeout_us) {
    for (uint16_t i = 0; i < timeout_us; i++) {
        _delay_us(1);
        if (get_ExtPack_event(unit_U02)) {
            // Acknowledgement received
            clear_ExtPack_event(unit_U02);
            return EXT_PACK_SUCCESS;
        }
    }
    // Timeout exceeded
    return EXT_PACK_FAILURE;
}