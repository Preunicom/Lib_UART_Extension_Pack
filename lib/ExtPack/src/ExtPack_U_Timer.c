#include "ExtPack_U_Timer.h"
#include "Dynamic_Delay.h"

ext_pack_error_t configure_ExtPack_timer(unit_t unit, uint8_t prescaler_divisor, uint8_t start_value) {
    uint8_t send_delay_us = get_ExtPack_send_duration_us();
    if(set_ExtPack_timer_enable(unit, 0) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    if(set_ExtPack_timer_prescaler(unit, prescaler_divisor) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    if(set_ExtPack_timer_start_value(unit, start_value) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    if(restart_ExtPack_timer(unit) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
    return set_ExtPack_timer_enable(unit, 1);
}