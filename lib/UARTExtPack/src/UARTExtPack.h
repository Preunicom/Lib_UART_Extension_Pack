#ifndef LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_H
#define LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_H

#include <stdint.h>

// --------------------------------------- Definition of Units ---------------------------------------

#include "UARTExtPack_UnitDefs.h"
#define unit_t uint8_t

// ------------------------------------- Definitions of Unit Types -------------------------------------
#define unit_type_t uint8_t

#define UNDEFINED 0
#define GPIO_Unit 1
#define UART_Unit 2

// -------------------------------------  Definition of Interface -------------------------------------
void init_ExtPack();
void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, char));

uint8_t send_ExtPack_data_nonblocking(unit_t unit, char data);
void send_ExtPack_data_blocking(unit_t unit, char data);
char get_ExtPack_data_gpio_in(unit_t unit);
char get_ExtPack_data_gpio_out(unit_t unit);
uint8_t refresh_ExtPack_gpio_data_nonblocking(unit_t unit);
void refresh_ExtPack_gpio_data_blocking(unit_t unit);

// -------------------------------------- Aliases for Interfaces --------------------------------------

#define set_ExPack_gpio_out_nonblocking send_ExtPack_data_nonblocking
#define set_ExPack_gpio_out_blocking send_ExtPack_data_blocking

#endif //LIB_UART_EXTENSION_PACK_FOR_ATMEGA328P_UARTEXTPACK_H
