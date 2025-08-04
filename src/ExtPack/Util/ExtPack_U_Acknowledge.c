#include "ExtPack_U_Acknowledge.h"
#include "../Core/ExtPack_Internal.h"
#include "../Core/ExtPack_Events.h"
#include <util/delay.h>

uint8_t get_ExtPack_ack_state() {
    return get_ExtPack_stored_unit_output_values(unit_U02) > 0;
}

void clear_ExtPack_ack_event() {
    clear_ExtPack_event(unit_U02);
}

uint8_t get_ExtPack_ack_event() {
    uint8_t is_event_set = get_ExtPack_event(unit_U02);
    clear_ExtPack_event(unit_U02);
    return is_event_set > 0;
}

uint8_t get_ExtPack_ack_data() {
    return get_ExtPack_stored_unit_input_values(unit_U02);
}

ext_pack_error_t set_ExtPack_ACK_enable(uint8_t enable) {
    unit_data[unit_U02].output_values = enable;
    return _send_to_ExtPack(_set_ExtPack_access_mode(unit_U02, 00), enable);
}