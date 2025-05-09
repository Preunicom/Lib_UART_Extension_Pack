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
    void (*custom_ISR)(unit_t, uint8_t);
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
 * @struct unit_data_storage
 * Structure representing input and output values of a units.
 *
 * @brief This structure holds the input and output values for a unit. It is used to
 *        store the current states of the unit.
 *
 * @note The structure contains two fields:
 *       - `input_values`: Stores the incoming values of the unit.
 *       - `output_values`: Stores the outgoing values of the unit.
 */
struct unit_data_storage {
    uint8_t input_values;   /**< Incoming values of the unit */
    uint8_t output_values;  /**< Outgoing values of the unit */
};

/**
 * @var unit_data
 * Array of unit data structures for multiple units.
 *
 * @brief This array stores the incoming and outgoing data for each unit. Each element in the array
 *        corresponds to a specific unit and stores its incoming and outgoing states, provided that
 *        the unit uses the storage.
 *
 * @note The array is initialized with zeros, and its size is determined by the `USED_UNITS` constant.
 *       It is used to manage and store the data for all active units which needs it within the system.
 */
struct unit_data_storage unit_data[USED_UNITS] = {0};

// ------------------- UART communication - data definition & declaration -------------------

#define BAUD_RATE 1000000
#define BAUD_CONST (((F_CPU/(BAUD_RATE*16UL)))-1)

#define state_type uint8_t

#define RECV_UNIT_NEXT_STATE 0
#define RECV_DATA_NEXT_STATE 1
#define RECV_INVALID_UNIT 2

state_type recv_state = RECV_UNIT_NEXT_STATE;

uint8_t next_data_to_send;

unit_t received_unit;

// -------------------------------------  Auxiliary functions -------------------------------------

void delay_us(unsigned int __us) {
    for (unsigned int i = 0; i < __us; i++) {
        _delay_us(1);
    }
}

void delay_ms(unsigned int __ms) {
    for (unsigned int i = 0; i < __ms; i++) {
        _delay_ms(1);
    }
}

// ---------------------------------------- Initialisation ----------------------------------------

void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t)) {
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
     * /8 prescaler --> 16 MHz / 8 = 2 MHz
     * --> Every UART bit is 2 clock cycles
     *  --> 2 UART messages with 8N1 are 10 bit each --> 20 bits
     *   --> At least 40 clock cycles
     * --> start_value = 190 (--> 66 clock cycles (40 + 26 buffer)
     */
    // Normal mode is default --> No change needed
    // No compares used --> No change needed
    // Set prescaler to /8
    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
    TCCR0B |= ( 1 << CS01);
    // Enable global interrupt
    sei();
    init_ExtPack_Unit(unit_U00, Reset_Unit, reset_ISR);
    init_ExtPack_Unit(unit_U01, Error_Unit, error_ISR);
}

void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, uint8_t)) {
    units[unit].unit_type = unit_type;
    units[unit].custom_ISR = custom_ISR;
}

// ---------------------------------------- Sending ----------------------------------------

/*
 * Send the data to ExtPack via UART.
 * Returns 0 if successfully, 1 otherwise.
 */
ext_pack_error_t UART_ExtPack_send(unit_t unit, uint8_t data) {
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
        } else if (units[unit].unit_type == SPI_Unit && (unit & (1<<ACC_MODE0))) {
            // Save sent SPI slave id
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
        // Enables reset state machine timer
        TIFR0 |= (1 << TOV0);
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
            void (*custom_ISR)(unit_t, uint8_t) = units[received_unit].custom_ISR;
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
    recv_state = RECV_UNIT_NEXT_STATE;
    // Disables timer interrupts
    TIMSK0 &= ~(1 << TOIE0);
}

// ---------------------------------------- Interface ----------------------------------------

// ------------------ RST_Unit interface -------------------

ext_pack_error_t reset_ExtPack() {
    return UART_ExtPack_send(unit_U00, 0xFF);
}

// ------------------ UART_Unit interface ------------------

ext_pack_error_t send_ExtPack_UART_String(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    int index = 0;
    ext_pack_error_t error;
    while (data[index] != '\0') {
        error = SEND_MAX_ATTEMPTS(send_ExtPack_UART_data(unit, data[index++]), max_attempts, retry_delay_us);
        if(error == EXT_PACK_FAILURE) {
            return EXT_PACK_FAILURE;
        }
        delay_us(send_delay_us);
    }
    return EXT_PACK_SUCCESS;
}

// ------------------ SPI_Unit interface -------------------

ext_pack_error_t set_ExtPack_SPI_slave(unit_t unit, uint8_t slave_id) {
    return send_ExtPack_SPI_data(unit, slave_id);
}

ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data) {
    if(set_ExtPack_SPI_slave(unit, slave_id) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return send_ExtPack_SPI_data(unit, data);
}

ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    int index = 0;
    ext_pack_error_t error;
    error = SEND_MAX_ATTEMPTS(set_ExtPack_SPI_slave(unit, slave_id), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    while (data[index] != '\0') {
        error = SEND_MAX_ATTEMPTS(send_ExtPack_SPI_data(unit, data[index++]), max_attempts, retry_delay_us);
        if(error == EXT_PACK_FAILURE) {
            return EXT_PACK_FAILURE;
        }
        delay_us(send_delay_us);
    }
    return EXT_PACK_SUCCESS;
}

uint8_t get_ExtPack_data_SPI_current_slave(unit_t unit) {
    return (uint8_t)unit_data[unit].output_values;
}

// ------------------ GPIO_Unit interface ------------------

ext_pack_error_t refresh_ExtPack_gpio_data(unit_t unit) {
    // Set access_mode to "01"
    uint8_t unit_number = (1<<ACC_MODE0) | unit;
    return UART_ExtPack_send(unit_number, 0x00);
}

uint8_t get_ExtPack_data_gpio_in(unit_t unit) {
    return unit_data[unit].input_values;
}

uint8_t get_ExtPack_data_gpio_out(unit_t unit) {
    return unit_data[unit].output_values;
}

// -------------------- Timer interface --------------------

ext_pack_error_t set_ExtPack_timer_enable(unit_t unit, uint8_t enable) {
    // Set access_mode to "00"
    uint8_t unit_number = ~((1<<ACC_MODE1) | (1<<ACC_MODE0)) & unit;
    return UART_ExtPack_send(unit_number, enable);
}

ext_pack_error_t restart_ExtPack_timer(unit_t unit) {
    // Set access_mode to "01"
    uint8_t unit_number = ~(1<<ACC_MODE1) & ((1<<ACC_MODE0) | unit);
    return UART_ExtPack_send(unit_number, 0x00);
}

ext_pack_error_t set_ExtPack_timer_prescaler(unit_t unit, uint8_t prescaler_divisor) {
    // Set access_mode to "10"
    uint8_t unit_number = ~(1<<ACC_MODE0) & ((1<<ACC_MODE1) | unit);
    return UART_ExtPack_send(unit_number, prescaler_divisor);
}

ext_pack_error_t set_ExtPack_timer_start_value(unit_t unit, uint8_t start_value) {
    // Set access_mode to "11"
    uint8_t unit_number = (1<<ACC_MODE1) | (1<<ACC_MODE0) | unit;
    return UART_ExtPack_send(unit_number, start_value);
}

ext_pack_error_t configure_ExtPack_timer(unit_t unit, uint8_t prescaler_divisor, uint8_t start_value, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    ext_pack_error_t error =  SEND_MAX_ATTEMPTS(set_ExtPack_timer_enable(unit, 0), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    delay_us(send_delay_us);
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