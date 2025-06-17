#include "ExtPack_U_Acknowledge.h"
#include "ExtPack_Internal.h"
#include <util/delay.h>

uint8_t get_ExtPack_ack_state() {
    return get_ExtPack_stored_unit_output_values(unit_U02) & (1<<ACK_STATE);
}

void clear_ExtPack_ack_event() {
    unit_data[unit_U02].output_values &= ~(1<<ACK_EVENT);
}

uint8_t get_ExtPack_ack_event() {
    enter_critical_zone();
    uint8_t event = unit_data[unit_U02].output_values & (1<<ACK_EVENT);
    unit_data[unit_U02].output_values &= ~(1<<ACK_EVENT); // Reset ACK event
    exit_critical_zone();
    return (event > 0);
}

ext_pack_error_t wait_for_ExtPack_ACK_data(uint8_t data, uint16_t timeout_us) {
    for (uint16_t i = 0; i < timeout_us; i++) {
        _delay_us(1);
        if (get_ExtPack_ack_event()) {
            // Acknowledgement received
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
        if (get_ExtPack_ack_event()) {
            // Acknowledgement received
            return EXT_PACK_SUCCESS;
        }
    }
    // Timeout exceeded
    return EXT_PACK_FAILURE;
}

uint8_t get_ExtPack_ack_data() {
    return get_ExtPack_stored_unit_input_values(unit_U02);
}