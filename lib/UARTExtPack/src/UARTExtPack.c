#include "UARTExtPack.h"
#include <avr/io.h>
#include "avr/interrupt.h"

// -------------------------- Unit data - definition & declaration --------------------------

#define USED_UNITS 64
#define ACC_MODE0 6
#define ACC_MODE1 7

struct unit {
    unit_type_t unit_type;
    void (*custom_ISR)(unit_t, char);
};

struct unit units[USED_UNITS] = {0};

struct gpio_data {
    char input_values;
    char output_values;
};

struct gpio_data unit_data[USED_UNITS] = {0};

// ------------------- UART communication - data definition & declaration -------------------

#define BAUD_RATE 1000000
#define BAUD_CONST (((F_CPU/(BAUD_RATE*16UL)))-1)

#define state_type uint8_t

#define RECV_UNIT_NEXT_STATE 0
#define RECV_DATA_NEXT_STATE 1
#define RECV_INVALID_UNIT 2

state_type recv_state = RECV_UNIT_NEXT_STATE;

char next_data_to_send;

unit_t received_unit;

// ---------------------------------------- Initialisation ----------------------------------------

/*
 * Inits UART unit
 */
void init_ExtPack() {
    //Set BAUD rate
    UBRR0H = (BAUD_CONST >> 8);
    UBRR0L = BAUD_CONST;
    //Set RX and TX to enabled and enable interrupt RX Complete
    UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
    //Set 8 bit data
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    // Enable global interrupt
    sei();
}

/*
 * Inits a ExtPack unit.
 */
void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, char)) {
    units[unit].unit_type = unit_type;
    units[unit].custom_ISR = custom_ISR;
}

// ---------------------------------------- Sending ----------------------------------------

/*
 * Send the data to ExtPack via UART.
 * Returns 0 if successfully, 1 otherwise.
 */
uint8_t UART_ExtPack_send(unit_t unit, char data) {
    uint8_t status_reg = SREG;
    sei();
    // Send data if:
    // - UART data register is empty
    // - Data register empty interrupt is not active (so no data is in queue to be sent)
    if(UCSR0A & (1<<UDRE0) && ~(UCSR0B) & (1<<UDRIE0)) {
        // UART data register empty and no data in queue to be sent
        UDR0 = unit;
        next_data_to_send = data;
        if(units[unit].unit_type == GPIO_Unit && !(unit & (1<<ACC_MODE0))) {
            // Save sent GPIO Outputs
            unit_data[unit].output_values = data;
        }
        // Activate data register empty interrupt
        UCSR0B |= (1<<UDRIE0);
        //Stay with sei() active to be able to send second part of UART message
        return 0;
    } else {
        // Not ready to send data pair
        // Reset SREG to reset sei() state
        SREG = status_reg;
        return 1;
    }
}

/*
 * Sends second part of data pair
 */
ISR(USART_UDRE_vect) {
    // UART data register empty
    UDR0 = next_data_to_send;
    // Deactivate data register empty interrupt
    UCSR0B &= ~(1<<UDRIE0);
}

// ---------------------------------------- Receiving ----------------------------------------

/*
 * Receives data from ExtPack via UART and triggers custom ISRs of Units.
 * Also manages received data for units.
 */
ISR(USART_RX_vect) {
    uint8_t errors = UCSR0A;
    uint8_t received_data = UDR0;
    if(recv_state == RECV_UNIT_NEXT_STATE) {
        // Received unit number
        received_unit = received_data;
        if(errors & ((1<<FE0)|(1<<UPE0))) {
            // Frame or Parity error
            recv_state = RECV_INVALID_UNIT;
        } else {
            // No error
            recv_state = RECV_DATA_NEXT_STATE;
        }
    } else if(recv_state == RECV_DATA_NEXT_STATE) {
        // Received unit data
        recv_state = RECV_UNIT_NEXT_STATE;
        if((errors & ((1<<FE0)|(1<<UPE0))) || received_unit >= USED_UNITS){
            // Frame or Parity Error or unit not in range of used units
            ;
        } else {
            // No error
            switch (units[received_unit].unit_type) {
                case GPIO_Unit:
                    units[received_unit].custom_ISR(received_unit, received_data);
                    // Sets GPIO in values of unit after customISR call to be able to determine interrupted pin in customISR
                    unit_data[received_unit].input_values = received_data;
                    break;
                case UART_Unit:
                    units[received_unit].custom_ISR(received_unit, received_data);
                    break;
            }
        }
    } else if(recv_state == RECV_INVALID_UNIT) {
        // Received unit had an error --> ignore
        recv_state = RECV_UNIT_NEXT_STATE;
    }
}

// ---------------------------------------- Interface ----------------------------------------

/*
 * Sends data to ExtPack.
 * Returns 0 if successfully, 1 otherwise.
 */
uint8_t send_ExtPack_data_nonblocking(unit_t unit, char data) {
    return UART_ExtPack_send(unit, data);
}

/*
 * Sends data to ExtPack.
 * Using while loop to block the program until the request is sent.
 */
void send_ExtPack_data_blocking(unit_t unit, char data) {
    while(UART_ExtPack_send(unit, data));
}

/*
 * Returns current input data of given ExtPack GPIO Unit
 */
char get_ExtPack_data_gpio_in(unit_t unit) {
    return unit_data[unit].input_values;
}

/*
 * Returns current output data of given ExtPack GPIO Unit
 */
char get_ExtPack_data_gpio_out(unit_t unit) {
    return unit_data[unit].output_values;
}

/*
 * Sends refresh request to ExtPack for given GPIO Unit.
 * Returns 0 if successfully, 1 otherwise.
 */
uint8_t refresh_ExtPack_gpio_data_nonblocking(unit_t unit) {
    uint8_t unit_number = (1<<ACC_MODE0) | unit;
    return UART_ExtPack_send(unit_number, 0x00);
}

/*
 * Sends refresh request to ExtPack for given GPIO Unit.
 * Using while loop to block the program until the request is sent.
 */
void refresh_ExtPack_gpio_data_blocking(unit_t unit) {
    uint8_t unit_number = (1<<ACC_MODE0) | unit;
    while(UART_ExtPack_send(unit_number, 0x00));
}