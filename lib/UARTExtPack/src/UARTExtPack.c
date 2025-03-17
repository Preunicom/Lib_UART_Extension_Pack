#include "UARTExtPack.h"
#include <stddef.h>
#include <avr/io.h>
#include "avr/interrupt.h"
// -------------------------- Unit data - definition & declaration --------------------------

/**
 * @def USED_UNITS
 * Defines the number of used units. This value should be overwritten later.
 *
 * @note This constant is a placeholder for the actual number of units in use.
 */
#define USED_UNITS 64

/**
 * @def ACC_MODE0
 * Defines bit 6 of the unit_number package to represent access mode 0.
 */
#define ACC_MODE0 6

/**
 * @def ACC_MODE1
 * Defines bit 7 of the unit_number package to represent access mode 1.
 */
#define ACC_MODE1 7

/**
 * @struct unit
 * Structure representing a unit in ExtPack.
 *
 * @param unit_type The type of the unit (e.g., GPIO, UART, Timer).
 * @param custom_ISR The custom interrupt service routine (ISR) function
 *        that the unit will call if an interrupt occurs.
 */
struct unit {
    unit_type_t unit_type;
    void (*custom_ISR)(unit_t, char);
};

/**
 * @var units
 * Array of unit structures.
 *
 * @brief This array holds the unit structures used in the system. The array is initialized
 *        with zero values and its size is determined by the macro 'USED_UNITS'.
 *
 * @note Each element of the array represents a unit within the system. The number of units
 *       in use is defined by the 'USED_UNITS' macro, and the array is pre-initialized to ensure
 *       all unit fields are set to zero on startup.
 */
struct unit units[USED_UNITS] = {0};

/**
 * @struct gpio_data
 * Structure representing the GPIO input and output values.
 *
 * @brief This structure holds the input and output pin values for a GPIO unit. It is used to
 *        store the current states of the GPIO unit's pins.
 *
 * @note The structure contains two fields:
 *       - `input_values`: Stores the input pin values of the GPIO unit.
 *       - `output_values`: Stores the output pin values of the GPIO unit.
 */
struct gpio_data {
    char input_values;   /**< Input pin values of the GPIO unit */
    char output_values;  /**< Output pin values of the GPIO unit */
};

/**
 * @var unit_data
 * Array of GPIO data structures for multiple units.
 *
 * @brief This array stores the GPIO input and output data for each unit. Each element in the array
 *        corresponds to a specific unit and stores its input and output pin states, provided that
 *        the unit is a GPIO unit.
 *
 * @note The array is initialized with zeros, and its size is determined by the `USED_UNITS` constant.
 *       It is used to manage and store the GPIO data for all active GPIO units within the system.
 */
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

void init_ExtPack() {
    /*
     * ---------- Init UART ----------
     * UART packages: 8N1 with 1 MBAUD
     */
    //Set BAUD rate
    UBRR0H = (BAUD_CONST >> 8);
    UBRR0L = BAUD_CONST;
    //Set RX and TX to enabled and enable interrupt RX Complete
    UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
    //Set 8-bit data
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    /*
     * ---------- Init Timer ----------
     * /8 prescaler, 66 cycles --> 33 UART bits
     */
    // Normal mode is default --> No change needed
    // No compares used --> No change needed
    // Set prescaler to /8
    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
    TCCR0B |= ( 1 << CS01);
    // Enable global interrupt
    sei();
}

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
    cli(); // Enter critical zone
    // Send data if:
    // - UART data register is empty
    // - Data register empty interrupt is not active (so no data is in queue to be sent)
    // - unit number is declared as used
    if(
       (UCSR0A & (1<<UDRE0)) // Check if data register empty is set --> UART buffer empty
       && (~(UCSR0B) & (1<<UDRIE0)) // Check if data register empty interrupts are enabled --> buffer is reserved for following unit data
       && ((unit & 0b00111111) < USED_UNITS) // check if unit number is used
       ) {
        // UART data register empty, no data in queue to be sent and unit number valid
        UDR0 = unit;
        next_data_to_send = data;
        if(units[unit].unit_type == GPIO_Unit && !(unit & (1<<ACC_MODE0))) {
            // Save sent GPIO Outputs
            unit_data[unit].output_values = data;
        }
        // Activate data register empty interrupt
        UCSR0B |= (1<<UDRIE0);
        sei(); // Exit critical zone
        return EXT_PACK_SUCCESS;
    } else {
        // Not ready to send data pair
        sei(); // Exit critical zone
        return EXT_PACK_FAILURE;
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
        if((errors & ((1<<FE0)|(1<<UPE0))) || (received_unit & (1<<ACC_MODE1)) || (received_unit & (1<<ACC_MODE0))) {
            // Frame or Parity error OR an access_mode bit is set
            recv_state = RECV_INVALID_UNIT;
        } else {
            // No error
            recv_state = RECV_DATA_NEXT_STATE;
        }
        // Enables reset state machine timer with 66 clock cycles (at 2 MHz, /8 prescaler)
        TCNT0 = 190;
        TIMSK0 |= (1 << TOIE0);
    } else if(recv_state == RECV_DATA_NEXT_STATE) {
        // Received unit data
        // Disables state machine reset timer
        TIMSK0 &= ~(1 << TOIE0);
        recv_state = RECV_UNIT_NEXT_STATE;
        if((errors & ((1<<FE0)|(1<<UPE0))) || received_unit >= USED_UNITS){
            // Frame or Parity Error or unit not in range of used units
            ;
        } else {
            // No error
            void (*custom_ISR)(unit_t, char) = units[received_unit].custom_ISR;
            switch (units[received_unit].unit_type) {
                case UNDEFINED:
                    return; // Ends receive because no unit type is chosen
                case GPIO_Unit:
                    // Sets GPIO in values of unit after customISR call to be able to determine interrupted pin in customISR
                    unit_data[received_unit].input_values = received_data;
                    break;
            }
            if(custom_ISR != NULL) {
                // Calls ISR of unit if set
                custom_ISR(received_unit, received_data);
            }
        }
    } else if(recv_state == RECV_INVALID_UNIT) {
        // Received unit had an error --> ignore unit data
        recv_state = RECV_UNIT_NEXT_STATE;
        // Disables state machine reset timer
        TIMSK0 &= ~(1 << TOIE0);
    }
}

/*
 * Resets state machine when timer/counter0 has an overflow
 */
ISR(TIMER0_OVF_vect) {
    //Reset state machine
    RECV_UNIT_NEXT_STATE;
    // Disables timer interrupts
    TIMSK0 &= ~(1 << TOIE0);
}

// ---------------------------------------- Interface ----------------------------------------

// ------------------ GPIO_Unit interface ------------------

uint8_t refresh_ExtPack_gpio_data(unit_t unit) {
    // Set access_mode to "01"
    uint8_t unit_number = (1<<ACC_MODE0) | unit;
    return UART_ExtPack_send(unit_number, 0x00);
}

char get_ExtPack_data_gpio_in(unit_t unit) {
    return unit_data[unit].input_values;
}

char get_ExtPack_data_gpio_out(unit_t unit) {
    return unit_data[unit].output_values;
}

// -------------------- Timer interface --------------------

uint8_t set_ExtPack_timer_enable(unit_t unit, uint8_t enable) {
    // Set access_mode to "00"
    uint8_t unit_number = ~((1<<ACC_MODE1) | (1<<ACC_MODE0)) & unit;
    return UART_ExtPack_send(unit_number, enable);
}

uint8_t restart_ExtPack_timer(unit_t unit) {
    // Set access_mode to "01"
    uint8_t unit_number = ~(1<<ACC_MODE1) & ((1<<ACC_MODE0) | unit);
    return UART_ExtPack_send(unit_number, 0x00);
}

uint8_t set_ExtPack_timer_prescaler(unit_t unit, uint8_t prescaler_divisor) {
    // Set access_mode to "10"
    uint8_t unit_number = ~(1<<ACC_MODE0) & ((1<<ACC_MODE1) | unit);
    return UART_ExtPack_send(unit_number, prescaler_divisor);
}

uint8_t set_ExtPack_timer_start_value(unit_t unit, uint8_t start_value) {
    // Set access_mode to "11"
    uint8_t unit_number = (1<<ACC_MODE1) | (1<<ACC_MODE0) | unit;
    return UART_ExtPack_send(unit_number, start_value);
}