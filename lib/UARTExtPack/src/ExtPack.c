#include "ExtPack.h"
#include <util/delay.h>

#define NULL (void*)0

#if defined(__AVR_ATmega328P__)
    #include "ExtPack_LL_M328P.h"
#else
    #error Unsupported microcontroller!
#endif

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

/**
 * This function processes the received data from the Low Level (LL) library of ExtPack.
 * The data is being checked for correct unit format and unit number.
 *
 * @note This function has to be handed to the LL library via function pointer when initializing ExtPack.
 *
 * @param unit Received unit.
 * @param data Received data.
 */
void process_received_ExtPack_data(unit_t unit, uint8_t data);

void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t)) {
    init_ExtPack_Unit(unit_U00, EXTPACK_RESET_UNIT, reset_ISR);
    init_ExtPack_Unit(unit_U01, EXTPACK_ERROR_UNIT, error_ISR);
    init_ExtPack_Unit(unit_U02, EXTPACK_ACK_UNIT, ack_ISR);
    init_ExtPack_LL(process_received_ExtPack_data);
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
    if ((unit & 0b00111111) < USED_UNITS) {
        // check if unit number is in used units range
        if (_send_UART_ExtPack_message(unit, data) == EXT_PACK_SUCCESS) {
            if(units[unit].unit_type == EXTPACK_GPIO_UNIT && !(unit & (1<<ACC_MODE0_BIT))) {
                // Save sent GPIO Outputs
                unit_data[unit].output_values = data;
            } else if (units[unit].unit_type == EXTPACK_SPI_UNIT && (unit & (1<<ACC_MODE0_BIT))) {
                // Save sent SPI slave id
                unit_data[unit].output_values = data;
            } else if (units[unit].unit_type == EXTPACK_I2C_UNIT && (unit & (1<<ACC_MODE0_BIT))) {
                // Save sent I2C partner address
                unit_data[unit].output_values = data;
            } else if (units[unit].unit_type == EXTPACK_ACK_UNIT) {
                // Save ACK state (active/not active)
                unit_data[unit].output_values &= (~(1<<ACK_STATE) | ((data>0)<<ACK_STATE));
            }
            return EXT_PACK_SUCCESS;
        }
        // Sending failed
    }
    // Sending failed or unit not in range of used units
    return EXT_PACK_FAILURE;
}

// --------------------------------------- Receiving ---------------------------------------

void process_received_ExtPack_data(unit_t unit, uint8_t data) {
    if(unit <= USED_UNITS
        && !(unit & (1<<ACC_MODE1_BIT))
        && !(unit & (1<<ACC_MODE0_BIT))) {
        // Valid unit and no access mode bit set
        void (*custom_ISR)(unit_t, uint8_t) = units[unit].custom_ISR;
        switch (units[unit].unit_type) {
            case EXTPACK_UNDEFINED:
                return; // Ends receive because no unit type is chosen
            case EXTPACK_GPIO_UNIT:
            case EXTPACK_I2C_UNIT:
                // Saves the last received values of the unit to be able to get it in the main program flow.
                unit_data[unit].input_values = data;
                break;
            case EXTPACK_ACK_UNIT:
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
                unit_data[unit].input_values = data;
                unit_data[unit].output_values |= (1 << ACK_EVENT);
                break;
        }
        if (custom_ISR != NULL) {
            // Calls ISR of unit if set
            custom_ISR(unit, data);
        }
    }
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
    enter_critical_zone();
    uint8_t event = unit_data[unit_U02].output_values & (1<<ACK_EVENT);
    unit_data[unit_U02].output_values &= ~(1<<ACK_EVENT); // Reset ACK event
    exit_critical_zone();
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

// ----------------- ERROR_Unit interface ------------------

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