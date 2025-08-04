#include "ExtPack_U_UART.h"
#include "../Core/ExtPack_Internal.h"

uint8_t get_last_received_ExtPack_UART_data(unit_t unit) {
    return get_ExtPack_stored_unit_input_values(unit);
}