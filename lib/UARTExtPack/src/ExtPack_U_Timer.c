#include "ExtPack_U_Timer.h"
#include "ExtPack_Internal.h"

ext_pack_error_t configure_ExtPack_timer(unit_t unit, uint8_t prescaler_divisor, uint8_t start_value, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    ext_pack_error_t error;
    error = SEND_MAX_ATTEMPTS(set_ExtPack_timer_enable(unit, 0), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    error = SEND_MAX_ATTEMPTS(set_ExtPack_timer_prescaler(unit, prescaler_divisor), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    error = SEND_MAX_ATTEMPTS(set_ExtPack_timer_start_value(unit, start_value), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    error = SEND_MAX_ATTEMPTS(restart_ExtPack_timer(unit), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    error = SEND_MAX_ATTEMPTS(set_ExtPack_timer_enable(unit, 1), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return EXT_PACK_SUCCESS;
}