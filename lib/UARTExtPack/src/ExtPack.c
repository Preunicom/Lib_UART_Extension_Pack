#include "ExtPack.h"
#include "ExtPack_Internal.h"
#include "Dynamic_Delay.h"

#define NULL (void*)0

struct unit units[USED_UNITS] = {0};

struct unit_data_storage unit_data[USED_UNITS] = {0};

/**
 * This function processes the received data from the Low Level (LL) library of ExtPack.
 * The data is being checked for correct unit format and unit number.
 *
 * @note This function has to be handed to the LL library via function pointer when initializing ExtPack.
 *
 * @param unit Received unit.
 * @param data Received data.
 */
void process_received_ExtPack_data(unit_t unit, uint8_t data);

void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t)) {
    init_ExtPack_Unit(unit_U00, EXTPACK_RESET_UNIT, reset_ISR);
    init_ExtPack_Unit(unit_U01, EXTPACK_ERROR_UNIT, error_ISR);
    init_ExtPack_Unit(unit_U02, EXTPACK_ACK_UNIT, ack_ISR);
    _init_ExtPack_LL(process_received_ExtPack_data);
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
            case EXTPACK_GPIO_UNIT:
            case EXTPACK_I2C_UNIT:
                // Saves the last received values of the unit to be able to get it in the main program flow.
                unit_data[unit].input_values = data;
                break;
            case EXTPACK_ACK_UNIT:
                /*
                 * Saves ACK data:
                 *  output_values:
                 *      Bit 0: ACK state (0: not enabled / 1: enabled)
                 *      Bit 1-6: Unused
                 *      Bit 7: ACK received event (0: not set / 1: set)
                 *
                 *  input_values:
                 *      Bit 0-7: data of last received acknowledgment
                 */
                unit_data[unit].input_values = data;
                unit_data[unit].output_values |= (1 << ACK_EVENT);
                break;
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
ext_pack_error_t send_to_ExtPack(unit_t unit, uint8_t data) {
    if ((unit & 0b00111111) < USED_UNITS) {
        // check if unit number is in used units range
        if (_send_UART_ExtPack_message(unit, data) == EXT_PACK_SUCCESS) {
            if(units[unit].unit_type == EXTPACK_GPIO_UNIT && !(unit & (1<<ACC_MODE0_BIT))) {
                // Save sent GPIO Outputs
                unit_data[unit].output_values = data;
            } else if (units[unit].unit_type == EXTPACK_SPI_UNIT && (unit & (1<<ACC_MODE0_BIT))) {
                // Save sent SPI slave id
                unit_data[unit].output_values = data;
            } else if (units[unit].unit_type == EXTPACK_I2C_UNIT && (unit & (1<<ACC_MODE0_BIT))) {
                // Save sent I2C partner address
                unit_data[unit].output_values = data;
            } else if (units[unit].unit_type == EXTPACK_ACK_UNIT) {
                // Save ACK state (active/not active)
                unit_data[unit].output_values &= (~(1<<ACK_STATE) | ((data>0)<<ACK_STATE));
            }
            return EXT_PACK_SUCCESS;
        }
        // Sending failed
    }
    // Sending failed or unit not in range of used units
    return EXT_PACK_FAILURE;
}

ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    int index = 0;
    ext_pack_error_t error;
    while (data[index] != '\0') {
        error = SEND_MAX_ATTEMPTS(send_to_ExtPack(unit, data[index++]), max_attempts, retry_delay_us);
        if(error == EXT_PACK_FAILURE) {
            return EXT_PACK_FAILURE;
        }
        delay_us(send_delay_us);
    }
    return EXT_PACK_SUCCESS;
}