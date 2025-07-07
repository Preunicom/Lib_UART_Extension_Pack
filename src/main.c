#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "ExtPack_U_Reset.h"
#include "ExtPack_U_Error.h"
#include "ExtPack_U_Acknowledge.h"
#include "ExtPack_U_GPIO.h"
#include "ExtPack_U_I2C.h"
#include "ExtPack_U_SPI.h"
#include "ExtPack_U_UART.h"
#include "ExtPack_U_Timer.h"
#include "ExtPack.h"
#include "Dynamic_Delay.h"

#define unit_U00_RST unit_U00
#define unit_U01_ERR unit_U01
#define unit_U02_ACK unit_U02
#define unit_U03_UART unit_U03
#define unit_U04_GPIO unit_U04
#define unit_U05_TIME unit_U05
#define unit_U06_SPI unit_U06
#define unit_U07_I2C unit_U07

void unit_U00_custom_ISR(unit_t, uint8_t);
void unit_U01_custom_ISR(unit_t, uint8_t);
void unit_U02_custom_ISR(unit_t, uint8_t);
void unit_U03_custom_ISR(unit_t, uint8_t);
void unit_U04_custom_ISR(unit_t, uint8_t);
void unit_U05_custom_ISR(unit_t, uint8_t);
void unit_U06_custom_ISR(unit_t, uint8_t);
void unit_U07_custom_ISR(unit_t, uint8_t);

int main() {
#ifndef __AVR_ATmega328P__
    uint8_t temp = CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm; // Disable global clk Prescaler
    CCP = 0xD8; // Disable change protection of Prescaler to write data
    CLKCTRL.MCLKCTRLB = temp;
#endif
    // Initialize the UART Extension Pack
    init_ExtPack(NULL, unit_U01_custom_ISR, unit_U02_custom_ISR);
    init_ExtPack_Unit(unit_U03_UART, EXTPACK_UART_UNIT, unit_U03_custom_ISR);
    init_ExtPack_Unit(unit_U04_GPIO, EXTPACK_GPIO_UNIT, unit_U04_custom_ISR);
    init_ExtPack_Unit(unit_U05_TIME, EXTPACK_TIMER_UNIT, unit_U05_custom_ISR);
    init_ExtPack_Unit(unit_U06_SPI, EXTPACK_SPI_UNIT, unit_U06_custom_ISR);
    init_ExtPack_Unit(unit_U07_I2C, EXTPACK_I2C_UNIT, unit_U07_custom_ISR);
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
    set_ExtPack_custom_ISR(unit_U00_RST, unit_U00_custom_ISR);
    configure_ExtPack_timer(unit_U05_TIME, 250, 56);
/*
    clear_ExtPack_ack_event();
    do {
        set_ExtPack_ACK_enable(1);
    } while (wait_for_ExtPack_ACK_data(1, 100) != EXT_PACK_SUCCESS);
    do {
        set_ExtPack_I2C_partner_adr(unit_U07_I2C, 0x68);
    } while (wait_for_ExtPack_ACK_data(0x68, 100) != EXT_PACK_SUCCESS);
    while(1) {
        // Read RTC
        for (uint8_t temp = 0; temp < 7; temp++) {
            clear_ExtPack_ack_event(); // Interrupt sent messages does not clear ACK event
            do {
                send_ExtPack_I2C_data(unit_U07_I2C, temp);
            } while (wait_for_ExtPack_ACK_data(temp, 100) != EXT_PACK_SUCCESS);
            clear_ExtPack_ack_event();
            do {
                receive_ExtPack_I2C_data(unit_U07_I2C);
            } while (wait_for_ExtPack_ACK_data(0x00, 100) != EXT_PACK_SUCCESS);
            delay_us(200);
        }
        delay_ms(1000);
    }
*/
    delay_ms(2500);
    while (1);
}

void unit_U00_custom_ISR(unit_t unit, uint8_t data) {
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

void unit_U01_custom_ISR(unit_t unit, uint8_t data) {
    // An error occurred
    set_ExtPack_gpio_out(unit_U04_GPIO, get_ExtPack_data_gpio_out(unit_U04_GPIO) ^ 0b01);
}

void unit_U02_custom_ISR(unit_t unit, uint8_t data) {
    // An ACK got received
    ;
}

void unit_U03_custom_ISR(unit_t unit, uint8_t data) {
    sei();
    // UART Unit data received
    send_ExtPack_UART_data(unit_U03_UART, data);
}

void unit_U04_custom_ISR(unit_t unit, uint8_t data) {
    sei();
    // GPIO interrupt received
    if(data == 1) {
        uint8_t string[14] = "Hello World!\n";
        send_ExtPack_UART_String(unit_U03_UART, string, 100);
        //send_ExtPack_SPI_String(unit_U06_SPI, string, 50);
        //send_ExtPack_I2C_String_to_partner(unit_U07_I2C, 0x68, string, 57);
    } else {
        uint8_t string[12] = "Bye World!\n";
        send_ExtPack_UART_String(unit_U03_UART, string, 100);
    }
}

void unit_U05_custom_ISR(unit_t unit, uint8_t data) {
    sei();
    // Timer interrupt received
    set_ExtPack_gpio_out(unit_U04_GPIO, get_ExtPack_data_gpio_out(unit_U04_GPIO) ^ 0b10);
}

void unit_U06_custom_ISR(unit_t unit, uint8_t data) {
    sei();
    // SPI message received
    if (data != 0) {
        // Got content
        ;
    }
}

void unit_U07_custom_ISR(unit_t unit, uint8_t data) {
    sei();
    // I2C message received
    ;
}