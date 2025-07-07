#if defined(__AVR_ATmega808__) || defined(__AVR_ATmega1608__) || defined(__AVR_ATmega3208__) || defined(__AVR_ATmega4808__) || \
    defined(__AVR_ATmega809__) || defined(__AVR_ATmega1609__) || defined(__AVR_ATmega3209__) || defined(__AVR_ATmega4809__)

#include "../ExtPack_Internal.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#if SEND_BUF_LEN > 0
#include "../ExtPack_Ringbuffer_Internal.h"
#endif

/**
 * @def BAUD_CONST
 *
 * @brief The constant value representing the BAUD rate.
 */
#define BAUD_CONST ((64UL*F_CPU)/(BAUD_RATE*16UL))

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
volatile ringbuffer_elem_t send_buf[SEND_BUF_LEN];
volatile ringbuffer_metadata_t send_buf_metadata;

volatile uint8_t next_data_to_send_is_buffer_pair = 1;
#endif

volatile uint8_t next_data_to_send;
volatile unit_t received_unit;

volatile uint8_t ExtPack_LL_SREG_save;

// ----------------------------------------- Init ------------------------------------------

void init_ExtPack_LL() {
    CPUINT_LVL1VEC = USART0_DRE_vect_num; // Set UART data register empty interrupt to higher priority as receive interrupt (Otherwise sending will be interrupted by receiving which leads to malformed command pairs)
#if SEND_BUF_LEN > 0
    // ------- Init ringbuffer -------
    init_ringbuffer_metadata(send_buf, SEND_BUF_LEN, &send_buf_metadata);
#endif
    /*
     * ---------- Init UART ----------
     * UART packages: 8N1 with 1 MBAUD
     */
    cli();
    //Set BAUD rate
    USART0.BAUD = BAUD_CONST;
    //Set 8-bit data
    USART0.CTRLC |= USART_CHSIZE_8BIT_gc;
    //Set RX and TX to enabled
    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
#if defined(__AVR_ATmega808__) || defined(__AVR_ATmega1608__)
    PORTA_DIRSET = PIN0_bm; //Set TX to output
    PORTA_DIRCLR = PIN1_bm; //Set RX to input
#else
    #error Implementation for TX and RX pin initialisation missing for this microcontroller. Add it above to fix.
#endif
    //Enable interrupt RX Complete
    USART0.CTRLA |= USART_RXCIE_bm;
    /*
     * ---------- Init Timer ----------
     * /8 prescaler --> 20 MHz / 8 = 2.5 MHz
     * --> Every UART bit is 2.5 clock cycles
     *  --> 2 UART messages with 8N1 are 10 bit each --> 20 bits
     *   --> At least 50 clock cycles
     * --> start_value = 65460 (--> 66 clock cycles (50 + 26 buffer)
     */
    // Normal mode is default --> No change needed
    // No compares used --> No change needed
    // Set prescaler to /8
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm;
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
        USART0.CTRLA |= USART_DREIE_bm;
    }
    sei();
    return ret;
#else
    cli();
    // Enter critical zone
    // Send data if:
    // - UART data register is empty
    // - Data register empty interrupt is not active (so no data is in queue to be sent)
    if((USART0.STATUS & USART_DREIF_bm) // Check if data register empty is set --> UART buffer empty
        && !(USART0.CTRLA & USART_DREIE_bm)) // Check if data register empty interrupts are enabled --> buffer is reserved for following unit data
    {
        // UART data register empty, no data in queue to be sent and unit number valid
        USART0.TXDATAL = unit;
        next_data_to_send = data;
        // Activate data register empty interrupt
        USART0.CTRLA |= USART_DREIE_bm;
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
ISR(USART0_DRE_vect) {
#if SEND_BUF_LEN > 0
    // UART data register empty
    if(next_data_to_send_is_buffer_pair) {
        // Send buffer data
        uint16_t data;
        if(read_buf(&send_buf_metadata, &data) == EXT_PACK_SUCCESS) {
            USART0.TXDATAL = (uint8_t)(data >> 8);
            next_data_to_send = (uint8_t)data;
            next_data_to_send_is_buffer_pair = 0;
        } else {
            // Deactivate data register empty interrupt as buffer is empty
            USART0.CTRLA &= ~USART_DREIE_bm;
        }
    } else {
        next_data_to_send_is_buffer_pair = 1;
        // Send data part of message
        USART0.TXDATAL = next_data_to_send;
        // Check if command in buffer needs to be sent
        if (is_buf_empty(&send_buf_metadata)) {
            // Deactivate data register empty interrupt as no data in queue
            USART0.CTRLA &= ~USART_DREIE_bm;
        }
    }
#else
    // UART data register empty
    USART0.TXDATAL = next_data_to_send;
    // Deactivate data register empty interrupt
    USART0.CTRLA &= ~USART_DREIE_bm;
#endif
}

// --------------------------------------- Receiving ---------------------------------------

/*
 * Receives data from ExtPack via UART and triggers custom ISRs of Units.
 * Also manages received data for units.
 */
ISR(USART0_RXC_vect) {
    uint8_t errors = USART0.RXDATAH;
    uint8_t received_data = USART0.RXDATAL;
    if(recv_state == RECV_UNIT_NEXT_STATE) {
        // Received unit number
        received_unit = received_data;
        if(errors & (USART_FERR_bm | USART_PERR_bm)) {
            // Frame or Parity error
            recv_state = RECV_INVALID_UNIT;
        } else {
            // No error
            recv_state = RECV_DATA_NEXT_STATE;
        }
        // Enables reset state machine timer
        TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm; // Reset interrupt flags
        TCA0.SINGLE.CNT = 65460;
        TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
    } else if(recv_state == RECV_DATA_NEXT_STATE) {
        if(!(errors & (USART_FERR_bm | USART_PERR_bm))) {
            // No Frame or Parity Error
            // Valid Syntax of UART data
            // Received unit data
            // Disables state machine reset timer
            TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
            recv_state = RECV_UNIT_NEXT_STATE;
            process_received_ExtPack_data(received_unit, received_data);
        }
    } else if(recv_state == RECV_INVALID_UNIT) {
        // Received unit had an error --> ignore unit data
        recv_state = RECV_UNIT_NEXT_STATE;
        // Disables state machine reset timer
        TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
    }
}

/*
 * Resets state machine when timer/counter0 has an overflow
 */
ISR(TCA0_OVF_vect) {
    //Reset state machine
    recv_state = RECV_UNIT_NEXT_STATE;
    // Disables timer interrupts
    TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
}

// ---------------------------------------- Utility ----------------------------------------

void enter_critical_zone() {
    ExtPack_LL_SREG_save = CPU_SREG;
    cli();
}

void exit_critical_zone() {
    CPU_SREG = ExtPack_LL_SREG_save;
}

#endif