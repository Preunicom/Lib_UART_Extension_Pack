#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "ExtPack_U_Reset.h"
#include "ExtPack_U_GPIO.h"
#include "ExtPack_U_Timer.h"
#include "ExtPack.h"
#include "Dynamic_Delay.h"

#define RESET_UNIT unit_U00
#define GPIO_UNIT unit_U03
#define TIMER_UNIT unit_U05

void reset_unit_custom_ISR(unit_t, uint8_t);
void timer_unit_custom_ISR(unit_t, uint8_t);

int main() {
#ifndef __AVR_ATmega328P__
    uint8_t temp = CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm; // Disable global clk Prescaler
    CCP = 0xD8; // Disable change protection of Prescaler to write data
    CLKCTRL.MCLKCTRLB = temp;
#endif
    // Initialize the UART Extension Pack
    init_ExtPack(NULL, NULL, NULL);
    init_ExtPack_Unit(TIMER_UNIT, EXTPACK_TIMER_UNIT, timer_unit_custom_ISR);
    /*
     * 50 KHz
     * 250 values are a good divider.
     * --> start value: 2^8-200 = 56
     * --> prescaler: 250
     * ----> pre scaled: 50KHz / 250 = 200 Hz
     * ----> counted: 200KHz / 200 = 1 Hz
     */
    reset_ExtPack();
    _delay_us(100);
    set_ExtPack_custom_ISR(RESET_UNIT, reset_unit_custom_ISR);
    configure_ExtPack_timer(TIMER_UNIT, 250, 56);
    while (1);
}

void reset_unit_custom_ISR(unit_t unit, uint8_t data) {
    // ExtPack was reset
    if ((uint8_t)data == 0xFF) {
#if defined(__AVR_ATmega328P__)
        // Reset controller
        __asm__ __volatile__("jmp 0x0000");
#else
        CCP = 0xD8; // Unprotection
        RSTCTRL.SWRR = RSTCTRL_SWRE_bm; // Reset
#endif
    }
}

void timer_unit_custom_ISR(unit_t unit, uint8_t data) {
    // Timer interrupt received
    set_ExtPack_gpio_out(GPIO_UNIT, get_ExtPack_data_gpio_out(GPIO_UNIT) ^ 0b10);
}