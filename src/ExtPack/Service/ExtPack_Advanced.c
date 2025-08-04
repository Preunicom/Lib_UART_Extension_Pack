#include "ExtPack_Advanced.h"
#include "../Util/Dynamic_Delay.h"

ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint16_t send_byte_delay_us) {
    int index = 0;
    while (data[index] != '\0') {
        if(_send_to_ExtPack(unit, data[index++]) == EXT_PACK_FAILURE) {
            return EXT_PACK_FAILURE;
        }
        delay_us(send_byte_delay_us);
    }
    return EXT_PACK_SUCCESS;
}