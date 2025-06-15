#include "UARTExtPack.h"
#include <stddef.h>
#include <avr/io.h>
#include "avr/interrupt.h"

// -------------------------- Unit data - definition & declaration --------------------------

/**
 * @def USED_UNITS
 * @brief Defines the number of used units. This value should be overwritten later.
 *
 * @note This constant is a placeholder for the actual number of units in use.
 */
#define USED_UNITS 64

/**
 * @def ACC_MODE0_BIT
 * @brief Defines bit 6 of the unit_number package to represent the lower access mode bit.
 */
#define ACC_MODE0_BIT 6

/**
 * @def ACC_MODE1_BIT
 * @brief Defines bit 7 of the unit_number package to represent the higher access mode bit.
 */
#define ACC_MODE1_BIT 7

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

/**
 * @def state_type
 * @brief Type alias for the UART receive state machine state.
 */
#define state_type uint8_t

/**
 * @def RECV_UNIT_NEXT_STATE
 * @brief UART receive state where a unit identifier byte is expected next.
 */
#define RECV_UNIT_NEXT_STATE 0

/**
 * @def RECV_DATA_NEXT_STATE
 * @brief UART receive state where the data byte is expected next.
 */
#define RECV_DATA_NEXT_STATE 1

/**
 * @def RECV_INVALID_UNIT
 * @brief UART receive state indicating an invalid unit identifier was received.
 */
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

// ------------------------------------ Initialisation -------------------------------------

void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t)) {
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
    init_ExtPack_Unit(unit_U02, ACK_Unit, ack_ISR);
}

void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, uint8_t)) {
    units[unit].unit_type = unit_type;
    units[unit].custom_ISR = custom_ISR;
}

void set_ExtPack_custom_ISR(unit_t unit, void (*new_custom_ISR)(unit_t, uint8_t)) {
    units[unit].custom_ISR = new_custom_ISR;
}

// ---------------------------------------- Sending ----------------------------------------

/*
 * Send the data to ExtPack via UART.
 * Returns 0 if successfully, 1 otherwise.
 */
ext_pack_error_t send_to_ExtPack(unit_t unit, uint8_t data) {
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
        if(units[unit].unit_type == GPIO_Unit && !(unit & (1<<ACC_MODE0_BIT))) {
            // Save sent GPIO Outputs
            unit_data[unit].output_values = data;
        } else if (units[unit].unit_type == SPI_Unit && (unit & (1<<ACC_MODE0_BIT))) {
            // Save sent SPI slave id
            unit_data[unit].output_values = data;
        } else if (units[unit].unit_type == I2C_Unit && (unit & (1<<ACC_MODE0_BIT))) {
            // Save sent I2C partner address
            unit_data[unit].output_values = data;
        } else if (units[unit].unit_type == ACK_Unit) {
            // Save ACK state (active/not active)
            unit_data[unit].output_values &= (~(1<<ACK_STATE) | ((data>0)<<ACK_STATE));
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

// --------------------------------------- Receiving ---------------------------------------

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
        if((errors & ((1<<FE0)|(1<<UPE0))) || (received_unit & (1<<ACC_MODE1_BIT)) || (received_unit & (1<<ACC_MODE0_BIT))) {
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
                case I2C_Unit:
                    // Saves the last received values of the unit to be able to get it in the main program flow.
                    unit_data[received_unit].input_values = received_data;
                    break;
                case ACK_Unit:
                    /*
                     * Saves ACK data:
                     *  output_values:
                     *      Bit 0: ACK state (0: not enabled / 1: enabled)
                     *      Bit 1-6: Unused
                     *      Bit 7: ACK received event (0: not set / 1: set)
                     *
                     *  input_values:
                     *      Bit 0-7: data of last received acknowledgment
                     */
                    unit_data[received_unit].input_values = received_data;
                    unit_data[received_unit].output_values |= (1 << ACK_EVENT);
                    break;
            }
            if (custom_ISR != NULL) {
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

// --------------------------------------- Interface ---------------------------------------

// ------------------ Universal interface ------------------

uint8_t get_ExtPack_stored_unit_output_values(unit_t unit) {
    return unit_data[unit].output_values;
}

uint8_t get_ExtPack_stored_unit_input_values(unit_t unit) {
    return unit_data[unit].input_values;
}

ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    int index = 0;
    ext_pack_error_t error;
    while (data[index] != '\0') {
        error = SEND_MAX_ATTEMPTS(send_to_ExtPack(unit, data[index++]), max_attempts, retry_delay_us);
        if(error == EXT_PACK_FAILURE) {
            return EXT_PACK_FAILURE;
        }
        delay_us(send_delay_us);
    }
    return EXT_PACK_SUCCESS;
}

// ------------------ RST_Unit interface -------------------

// ------------------ ACK_Unit interface -------------------

void clear_ExtPack_ack_event() {
    unit_data[unit_U02].output_values &= ~(1<<ACK_EVENT);
}

uint8_t get_ExtPack_ack_event() {
    uint8_t SREG_temp = SREG;
    cli(); // Enter critical zone
    uint8_t event = unit_data[unit_U02].output_values & (1<<ACK_EVENT);
    unit_data[unit_U02].output_values &= ~(1<<ACK_EVENT); // Reset ACK event
    SREG = SREG_temp; // exit critical zone
    return (event > 0);
}

ext_pack_error_t wait_for_ExtPack_ACK_data(uint8_t data, uint16_t timeout_us) {
    for (uint16_t i = 0; i < timeout_us; i++) {
        _delay_us(1);
        if (get_ExtPack_ack_event()) {
            // Acknowledgement received
            if (unit_data[unit_U02].input_values == data) {
                // Matching acknowledgment data
                return EXT_PACK_SUCCESS;
            } else {
                // Wrong acknowledgment data
                return EXT_PACK_FAILURE;
            }
        }
    }
    // Timeout exceeded
    return EXT_PACK_FAILURE;
}

ext_pack_error_t wait_for_ExtPack_ACK(uint16_t timeout_us) {
    for (uint16_t i = 0; i < timeout_us; i++) {
        _delay_us(1);
        if (get_ExtPack_ack_event()) {
            // Acknowledgement received
            return EXT_PACK_SUCCESS;
        }
    }
    // Timeout exceeded
    return EXT_PACK_FAILURE;
}

// ------------------ UART_Unit interface ------------------

// ------------------ SPI_Unit interface -------------------

ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data) {
    if(set_ExtPack_SPI_slave(unit, slave_id) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return send_ExtPack_SPI_data(unit, data);
}

ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    ext_pack_error_t error;
    error = SEND_MAX_ATTEMPTS(set_ExtPack_SPI_slave(unit, slave_id), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    error = send_String_to_ExtPack(unit, data, send_delay_us, max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return EXT_PACK_SUCCESS;
}

// ------------------ I2C_Unit interface -------------------

ext_pack_error_t receive_ExtPack_I2C_data_from_partner(unit_t unit, uint8_t partner_adr) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return receive_ExtPack_I2C_data(unit);
}

ext_pack_error_t send_ExtPack_I2C_data_to_partner(unit_t unit, uint8_t partner_adr, uint8_t data) {
    if(set_ExtPack_I2C_partner_adr(unit, partner_adr) == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return send_ExtPack_I2C_data(unit, data);
}

ext_pack_error_t send_ExtPack_I2C_String_to_partner(unit_t unit, uint8_t partner_adr, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us) {
    ext_pack_error_t error;
    error = SEND_MAX_ATTEMPTS(set_ExtPack_I2C_partner_adr(unit, partner_adr), max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    error = send_String_to_ExtPack(unit, data, send_delay_us, max_attempts, retry_delay_us);
    if(error == EXT_PACK_FAILURE) {
        return EXT_PACK_FAILURE;
    }
    return EXT_PACK_SUCCESS;
}

// ------------------ GPIO_Unit interface ------------------

// -------------------- Timer interface --------------------

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