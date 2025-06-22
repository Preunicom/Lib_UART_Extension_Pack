#include "ExtPack.h"
#include "ExtPack_Internal.h"
#include "Dynamic_Delay.h"
#include "ExtPack_Events.h"

#define NULL (void*)0

struct unit units[USED_UNITS] = {0};

struct unit_data_storage unit_data[USED_UNITS] = {0};

void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t)) {
    _init_ExtPack_LL();
    init_ExtPack_Unit(unit_U00, EXTPACK_RESET_UNIT, reset_ISR);
    init_ExtPack_Unit(unit_U01, EXTPACK_ERROR_UNIT, error_ISR);
    init_ExtPack_Unit(unit_U02, EXTPACK_ACK_UNIT, ack_ISR);
}

void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, uint8_t)) {
    units[unit].unit_type = unit_type;
    units[unit].custom_ISR = custom_ISR;
}

void set_ExtPack_custom_ISR(unit_t unit, void (*new_custom_ISR)(unit_t, uint8_t)) {
    units[unit].custom_ISR = new_custom_ISR;
}

void process_received_ExtPack_data(unit_t unit, uint8_t data) {
    if(unit <= USED_UNITS
        && !(unit & (1<<ACC_MODE1_BIT))
        && !(unit & (1<<ACC_MODE0_BIT))) {
        // Valid unit and no access mode bit set
        void (*custom_ISR)(unit_t, uint8_t) = units[unit].custom_ISR;
        switch (units[unit].unit_type) {
            case EXTPACK_UNDEFINED:
                return; // Ends receive because no unit type is chosen
            default:
                unit_data[unit].input_values = data;
                set_ExtPack_event(unit);
        }
        if (custom_ISR != NULL) {
            // Calls ISR of unit if set
            custom_ISR(unit, data);
        }
        }
}

/*
 * Send the data to ExtPack via UART.
 * Returns 0 if successfully, 1 otherwise.
 */
ext_pack_error_t _send_to_ExtPack(unit_t unit, uint8_t data) {
    // check if unit number is in used units range
    if ((unit & 0b00111111) < USED_UNITS) {
        return _send_UART_ExtPack_message(unit, data);
    }
    // Sending failed or unit not in range of used units
    return EXT_PACK_FAILURE;
}

ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint8_t send_byte_delay_us) {
    int index = 0;
    while (data[index] != '\0') {
        if(_send_to_ExtPack(unit, data[index++]) == EXT_PACK_FAILURE) {
            return EXT_PACK_FAILURE;
        }
        delay_us(send_byte_delay_us);
    }
    return EXT_PACK_SUCCESS;
}

uint8_t get_ExtPack_send_duration_us() {
    /*
     * UART transmission itself:
     * duration (us) = (AMOUNT_BITS (bit) * 1,000,000(us/s)) / BAUD_RATE (bit/s)
     *
     * + some buffer for setup time of UART and time to execute the second byte send operation.
     * --> Take constant from LL
     * duration(us) = (ISR_Overhead (cc) * 1,000,000(us/s)) / F_CPU (cc/s)
     */
    return ((EXT_PACK_UART_BITS_PER_COMMAND_PAIR*1000000) / (uint32_t)BAUD_RATE) +
        ((EXT_PACK_SOFTWARE_OVERHEAD_UART_TRANSMISSION_CLOCK_CYCLES*1000000)/(uint32_t)F_CPU);
}