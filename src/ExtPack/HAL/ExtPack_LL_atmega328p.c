#include "ExtPack_LL.h"
#include "avr/io.h"
#include "avr/interrupt.h"

/**
 * @def BAUD_CONST
 *
 * @brief The constant value representing the BAUD rate.
 */
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

volatile state_type recv_state = RECV_UNIT_NEXT_STATE;

#ifndef SEND_BUF_LEN
    #warning SEND_BUF_LEN not defined! Setting default value (10).
    #define SEND_BUF_LEN 10
#endif
#if SEND_BUF_LEN % 2 != 0
    #error SEND_BUF_LEN not even or too small!
#endif

#if SEND_BUF_LEN > 0
#include "../Core/ExtPack_Ringbuffer_Internal.h"

volatile ringbuffer_elem_t send_buf[SEND_BUF_LEN];
volatile ringbuffer_metadata_t send_buf_metadata;

volatile uint8_t next_data_to_send_is_buffer_pair = 1;
#endif

volatile uint8_t next_data_to_send;
volatile unit_t received_unit;

volatile uint8_t ExtPack_LL_SREG_save;

// ----------------------------------------- Init ------------------------------------------

void init_ExtPack_LL() {
#if SEND_BUF_LEN > 0
    // ------- Init ringbuffer -------
    init_ringbuffer_metadata(send_buf, SEND_BUF_LEN, &send_buf_metadata);
#endif
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
}

// ---------------------------------------- Sending ----------------------------------------

ext_pack_error_t send_UART_ExtPack_command(unit_t unit, uint8_t data) {
#if SEND_BUF_LEN > 0
    cli();
    uint8_t is_first_command = is_buf_empty(&send_buf_metadata);
    // Add to buffer
    uint16_t buf_data = ((uint16_t)unit<<8) | data;
    uint8_t ret = write_buf(&send_buf_metadata, buf_data);
    if (is_first_command && ret == EXT_PACK_SUCCESS) {
        // Activate data register empty interrupt
        UCSR0B |= (1 << UDRIE0);
    }
    sei();
    return ret;
#else
    cli();
    // Enter critical zone
    // Send data if:
    // - UART data register is empty
    // - Data register empty interrupt is not active (so no data is in queue to be sent)
    if(
        (UCSR0A & (1<<UDRE0)) // Check if data register empty is set --> UART buffer empty
        && (~(UCSR0B) & (1<<UDRIE0)) // Check if data register empty interrupts are enabled --> buffer is reserved for following unit data
        ) {
        // UART data register empty, no data in queue to be sent and unit number valid
        UDR0 = unit;
        next_data_to_send = data;
        // Activate data register empty interrupt
        UCSR0B |= (1 << UDRIE0);
        sei(); // Exit critical zone
        return EXT_PACK_SUCCESS;
        } else {
            // Not ready to send data pair
            sei(); // Exit critical zone
            return EXT_PACK_FAILURE;
        }
#endif
}

/*
 * Sends next buffer data pair or second part of data pair
 */
ISR(USART_UDRE_vect) {
#if SEND_BUF_LEN > 0
    // UART data register empty
    if(next_data_to_send_is_buffer_pair) {
        // Send buffer data
        uint16_t data;
        if(read_buf(&send_buf_metadata, &data) == EXT_PACK_SUCCESS) {
            UDR0 = (uint8_t)(data >> 8);
            next_data_to_send = (uint8_t)data;
            next_data_to_send_is_buffer_pair = 0;
        } else {
            // Deactivate data register empty interrupt as buffer is empty
            UCSR0B &= ~(1<<UDRIE0);
        }
    } else {
        next_data_to_send_is_buffer_pair = 1;
        // Send data part of message
        UDR0 = next_data_to_send;
        // Check if command in buffer needs to be sent
        if (is_buf_empty(&send_buf_metadata)) {
            // Deactivate data register empty interrupt as no data in queue
            UCSR0B &= ~(1<<UDRIE0);
        }
    }
#else
    // UART data register empty
    UDR0 = next_data_to_send;
    // Deactivate data register empty interrupt
    UCSR0B &= ~(1<<UDRIE0);
#endif

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
        if(errors & ((1<<FE0)|(1<<UPE0))) {
            // Frame or Parity error
            recv_state = RECV_INVALID_UNIT;
        } else {
            // No error
            recv_state = RECV_DATA_NEXT_STATE;
        }
        // Enables reset state machine timer
        TIFR0 |= (1 << TOV0); // Reset interrupt flags
        TCNT0 = 190;
        TIMSK0 |= (1 << TOIE0);
    } else if(recv_state == RECV_DATA_NEXT_STATE) {
        if(!(errors & ((1<<FE0)|(1<<UPE0)))) {
            // No Frame or Parity Error
            // Valid Syntax of UART data
            // Received unit data
            // Disables state machine reset timer
            TIMSK0 &= ~(1 << TOIE0);
            recv_state = RECV_UNIT_NEXT_STATE;
            process_received_ExtPack_data(received_unit, received_data);
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

// ---------------------------------------- Utility ----------------------------------------

void enter_critical_zone() {
    ExtPack_LL_SREG_save = SREG;
    cli();
}

void exit_critical_zone() {
    SREG = ExtPack_LL_SREG_save;
}