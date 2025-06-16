/**
 * @file ExtPack.h
 *
 * @brief Header file for the Extension Pack library.
 *
 * This file provides function prototypes, macros, and definitions for interfacing
 * with the ExtPack system. The library prepares communication for
 * various unit types, including GPIO, UART, Timer, SPI and I2C units.
 *
 * ## Features:
 * - Initialization functions for the ExtPack system and individual units.
 * - GPIO interface for retrieving input and output states.
 * - Timer control functions, including enabling, restarting, and configuring timers.
 * - SPI interface functions for managing SPI communication (setting slave, sending/receiving data).
 * - I2C interface functions for managing I2C communication (setting partner, sending/receiving data).
 * - Macro-based aliases for simplified function calls.
 *
 * @author Markus Remy
 * @date 15.06.2025
 */

#ifndef EXTPACK_H
#define EXTPACK_H

#include <stdint.h>
#include "ExtPack_Defs.h"  // Include the header file for constants definitions

// --------------------------------  Definition of auxiliary functions -------------------------------

/**
 * Delays for approximately the given time through busy waiting.
 * Calls _delay_us(1) for delay_us times to bypass the problem to have compile-time constant values for _delay_us.
 *
 * @param delay_us Delay time in us.
 */
void delay_us(unsigned int delay_us);

/**
*  Delays for approximately the given time through busy waiting.
 * Calls _delay_ms(1) for delay_ms times to bypass the problem to have compile-time constant values for _delay_ms.
 *
 * @param delay_ms Delay time in ms.
 */
void delay_ms(unsigned int delay_ms);

// -------------------------------------  Definition of Interface -------------------------------------

/**
 * Initializes communication with ExtPack over UART.
 * This function enables USART0, Timer/Counter0 and global interrupts to be able to establish communication.
 * It also initializes unit_U00 as reset unit, unit_U01 as error unit, and unit_U02 as ACK unit with the given ISRs.
 * @param reset_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ExtPack got reset.
 * @param error_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the error unit of the ExtPack sends an error.
 * @param ack_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when the ACK unit of the ExtPack sends an acknowledgment.
 */
void init_ExtPack(void (*reset_ISR)(unit_t, uint8_t), void (*error_ISR)(unit_t, uint8_t), void (*ack_ISR)(unit_t, uint8_t));

/**
 * Initializes the specified ExtPack unit with the given parameters.
 *
 * @param unit The ExtPack unit to initialize.
 * @param unit_type The type of the unit.
 * @param custom_ISR A pointer to the interrupt service routine (ISR) function
 *                   to be called when an interrupt occurs for this unit.
 */
void init_ExtPack_Unit(unit_t unit, unit_type_t unit_type, void (*custom_ISR)(unit_t, uint8_t));

/**
 * @def SEND_MAX_ATTEMPTS
 * @brief Calls the specified ExtPack send function repeatedly until the data is successfully sent or the maximum number of attempts is reached.
 *
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Could lead to errors in condition. Call before and save return value in a variable instead.
 *
 * @param func_call The send function to be called. It should return `EXT_PACK_SUCCESS` or `EXT_PACK_FAILURE`.
 * @param max_attempts The maximum number of attempts to retry the function call. If set to 0 the specified ExtPack send function is called repeatedly until the data is successfully sent.
 * @param delay_us The delay in microseconds applied before each retry.
 */
#define SEND_MAX_ATTEMPTS(func_call, max_attempts, retry_delay_us) \
    ({ \
        int attempts = 0; \
        int result = EXT_PACK_SUCCESS; \
        while ((func_call) == EXT_PACK_FAILURE) { \
            attempts++; \
            if (attempts >= (max_attempts) && (max_attempts) != 0) { \
                result = EXT_PACK_FAILURE; \
                break; \
            } \
            delay_us(retry_delay_us); \
        } \
        result; \
    })

// ------------------- General interface -------------------

/**
 * @def set_ExtPack_access_mode
 * @brief Sets the access mode of the unit to the given one.
 *
 * @param unit Unit to set the access mode for.
 * @param access_mode Access mode to set.
 */
#define set_ExtPack_access_mode(unit, access_mode) (((unit) & 0x3F) | ((access_mode) << 6))

/**
 * Sends the data "as is" to ExtPack via UART.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_to_ExtPack(unit_t unit, uint8_t data);

/**
 * @def send_ExtPack_generic_data
 * @brief Alias to send unmodified data to the specified unit of ExtPack.
 *
 * This macro invokes `send_to_ExtPack` to transmit the data.
 */
#define send_ExtPack_generic_data send_to_ExtPack

/**
 * Sends the given String until '\0' to ExtPack.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a normal send operation via send_to_ExtPack per byte.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent. Including the correct set access mode for sending.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_delay_us The delay waited between two sent chars. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a char. 0 for unlimited retries.
 * @param retry_delay_us The delay between send char attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending an uint8_t because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t send_String_to_ExtPack(unit_t unit, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

/**
 * Sets a new custom ISR for the given unit of ExtPack.
 *
 * @note Use 'NULL' to have no ISR for the unit.
 *
 * @param unit The ExtPack unit which ISR should be set.
 * @param new_custom_ISR The new custom ISR function which is called when an interrupt of the unit occurs.
 */
void set_ExtPack_custom_ISR(unit_t unit, void (*new_custom_ISR)(unit_t, uint8_t));

/**
 * Returns the stored output data of the given unit of ExtPack.
 * The data has to be interpreted depending on the unit type.
 *
 * @param unit The ExtPack unit.
 * @return The stored output data.
 */
uint8_t get_ExtPack_stored_unit_output_values(unit_t unit);

/**
 * Returns the stored input data of the given unit of ExtPack.
 * The data has to be interpreted depending on the unit type.
 *
 * @param unit The ExtPack unit.
 * @return The stored input data.
 */
uint8_t get_ExtPack_stored_unit_input_values(unit_t unit);

// ------------------- RST_Unit interface ------------------

/**
 * @def reset_ExtPack()
 * @brief Send a reset command to unit zero of ExtPack via UART.
 *
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invoked 'send_to_ExtPack' with 0xFF as data.
 */
#define reset_ExtPack() send_to_ExtPack(unit_U00, 0xFF)

// ------------------- ACK_Unit interface ------------------

/**
 * @def ACK_EVENT
 * @brief Constant value representing the acknowledge received event x-th bit of unit data output for ACK unit. Used to shift bits to mask data to match the acknowledge received event bit.
 */
#define ACK_EVENT 7

/**
 * @def ACK_STATE
 * @brief Constant value representing the status x-th bit of unit data output for ACK unit. Used to shift bits to mask data to match the ACK unit active/not active bit.
 */
#define ACK_STATE 0

/**
 * @def get_ExtPack_ack_state()
 * @brief Retrieves the status of the ExtPack ACK unit (unit_U02).
 *
 * @return active (1) or inactive (0)
 *
 * This macro invokes 'get_ExtPack_stored_unit_output_values' and modifies the result to extract the ack state.
 */
#define get_ExtPack_ack_state() (get_ExtPack_stored_unit_output_values(unit_U02) & (1<<ACK_STATE))

/**
 * Clears the acknowledgement received event of the ExtPack ACK unit (unit_U02).
 */
void clear_ExtPack_ack_event();

/**
 * Retrieves the acknowledge received event of the ExtPack ACK unit (unit_U02).
 * Additionally, it sets the event to not set.
 *
 * @return event set (1) or event not set (0)
 */
uint8_t get_ExtPack_ack_event();

/**
 * Blocks until an acknowledgment of the ACK unit (unit_U02) of ExtPack gets received or the timeout is over.
 * Also checks ACK data.
 * Resets ACK event to not set.
 *
 * @param data The data the acknowledgement is expected to be.
 * @param timeout_us Maximum time awaited in us until the function returns an error.
 * @return EXT_PACK_SUCCESS if the expected acknowledgement is received before timeout,
 *         EXT_PACK_FAILURE if the acknowledgement has wrong data or the timeout is reached.
 */
ext_pack_error_t wait_for_ExtPack_ACK_data(uint8_t data, uint16_t timeout_us);

/**
 * Blocks until an acknowledgment of the ACK unit (unit_U02) of ExtPack gets received or the timeout is over.
 * Does not compare ACK data.
 *
 * @param timeout_us Maximum time awaited in us until the function returns an error.
 * @return EXT_PACK_SUCCESS if an acknowledgement for the given unit is received before timeout,
 *         EXT_PACK_FAILURE if the timeout is reached.
 */
ext_pack_error_t wait_for_ExtPack_ACK(uint16_t timeout_us);

/**
 *  @def get_ExtPack_ack_data()
 *  @brief Alias to retrieve the acknowledgment received data of the ExtPack ACK unit.
 *
 *  This macro invokes `get_ExtPack_stored_unit_input_values` with unit_U02 as unit to get the received data.
 */
#define get_ExtPack_ack_data() get_ExtPack_stored_unit_input_values(unit_U02)

/**
 * @def set_ExtPack_ACK_enable(enable)
 * @brief Alias to send enable or disable command to the ExtPack ACK unit.
 *
 * This macro invokes `send_to_ExtPack` with the given enable value as enable and unit_U02 as unit to transmit the data.
 * Enable can ether be zero to disable the unit or greater as zero to enable it.
 */
#define set_ExtPack_ACK_enable(enable) send_to_ExtPack(set_ExtPack_access_mode(unit_U02, 00), enable)

// ------------------ UART_Unit interface ------------------

/**
 * @def send_ExtPack_UART_data
 * @brief Alias to send UART data to the specified UART unit of ExtPack.
 *
 * This macro invokes `send_to_ExtPack` to transmit the data.
 */
#define send_ExtPack_UART_data send_to_ExtPack

/**
 * @def send_ExtPack_UART_String
 *
 * @brief Alias to send a null-terminated string to the specified UART unit of ExtPack.
 *
 * Sends the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a normal send operation per send_to_ExtPack.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_delay_us The delay waited between two sent chars. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a char. 0 for unlimited retries.
 * @param retry_delay_us The delay between send char attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a uint8_t because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 *
 * This macro invokes 'send_String_to_ExtPack' to send the data.
 */
#define send_ExtPack_UART_String send_String_to_ExtPack

// ------------------ SPI_Unit interface -------------------

/**
 * @def set_ExtPack_SPI_slave(unit, slave_id)
 * @brief Sets the slave ID of the given SPI unit of ExtPack via UART control message.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send following data to.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with access mode 01 to send the data.
 */
#define set_ExtPack_SPI_slave(unit, slave_id) send_to_ExtPack(set_ExtPack_access_mode(unit, 01), slave_id)

/**
 * @def send_ExtPack_SPI_data
 * @brief Alias to send SPI data to the specified SPI unit of ExtPack.
 *
 * This macro invokes `send_to_ExtPack` to transmit the data.
 */
#define send_ExtPack_SPI_data send_to_ExtPack

/**
 * Sends given data to slave of SPI_Unit of ExtPack by setting the slave id of the unit followed by sending the data to set slave.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send the data to.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_ExtPack_SPI_data_to_slave(unit_t unit, uint8_t slave_id, uint8_t data);

/**
 * Sends a set_ExtPack_SPI_slave control message followed by the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a set_ExtPack_SPI_slave or a normal send operation per send_ExtPack_SPI_data.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param slave_id The slave id to send the data to.
 * @param data The data to be sent as String with terminating '\0'.
 * @param send_delay_us The delay waited between two sent messages. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a message. 0 for unlimited retries.
 * @param retry_delay_us The delay between send message attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a char because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t send_ExtPack_SPI_String_to_slave(unit_t unit, uint8_t slave_id, const uint8_t* data, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

/**
 * @def get_ExtPack_data_SPI_current_slave
 * @brief Retrieves the last set SPI slave id of the given ExtPack SPI unit.
 *
 * @param unit The SPI unit of ExtPack to get the current set slave id from.
 * @return The last sent SPI slave id.
 *
 * This macro invokes 'get_ExtPack_stored_unit_output_values' to get the data.
 */
#define get_ExtPack_data_SPI_current_slave get_ExtPack_stored_unit_output_values

/**
 * @def send_ExtPack_SPI_String
 * @brief Alias to send an SPI string to the specified SPI unit of ExtPack.
 *
 * This macro invokes `send_String_to_ExtPack` to transmit the data.
 */
#define send_ExtPack_SPI_String send_String_to_ExtPack

// ------------------ I2C_Unit interface -------------------

/**
 * @def get_ExtPack_data_I2C_current_partner_adr(unit)
 * @brief Retrieves the last set I2C partner address of the given ExtPack I2C unit.
 *
 * @param unit The I2C unit of ExtPack to get the current set partner address from.
 * @return The last sent I2C slave id.
 *
 * This macro invokes 'get_ExtPack_data_I2C_current_partner_adr' and modifies the result to only return the transmitted 7 bits.
 */
#define get_ExtPack_data_I2C_current_partner_adr(unit) (0b01111111 & (get_ExtPack_data_I2C_current_partner_adr(unit)))

/**
 * @def get_ExtPack_data_I2C_last_received_data
 * @brief Alias to retrieve the last received I2C data byte of the given ExtPack I2C unit.
 *
 * This macro invokes `get_ExtPack_stored_unit_input_values` to get the data.
 */
#define get_ExtPack_data_I2C_last_received_data get_ExtPack_stored_unit_input_values

/**
 * @def set_ExtPack_I2C_partner_adr(unit, slave_id)
 * @brief Alias to set the partner address of the given I2C unit of ExtPack.
 *
 * This macro invokes `send_to_ExtPack` with 01 as access mode to transmit the data.
 */
#define set_ExtPack_I2C_partner_adr(unit, slave_id) send_to_ExtPack(set_ExtPack_access_mode(unit, 01), slave_id)

/**
 * @def receive_ExtPack_I2C_data(unit)
 * @brief Requests a byte from the currently set partner of the I2C unit of ExtPack.
 *
 * @note The received data will not be returned. Use the custom ISR to work with the received data.
 *
 * @param unit The I2C unit of ExtPack to receive data from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with 01 as access mode to send the data.
 */
#define receive_ExtPack_I2C_data(unit) send_to_ExtPack(set_ExtPack_access_mode(unit, 10), 0x00)

/**
 * Request a byte from given partner and set the partner in the I2C_Unit of ExtPack.
 *
 * @note The received data will not be returned. Use the custom ISR to work with the received data.
 *
 * @param unit The I2C unit of ExtPack to receive data from.
 * @param partner_adr The partner address to get data from.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t receive_ExtPack_I2C_data_from_partner(unit_t unit, uint8_t partner_adr);

/**
 * @def send_ExtPack_I2C_data
 * @brief Alias to send I2C data to the specified I2C unit of ExtPack.
 *
 * This macro invokes `send_to_ExtPack` to transmit the data.
 */
#define send_ExtPack_I2C_data send_to_ExtPack

/**
 * @def send_ExtPack_I2C_String
 * @brief Alias to send an I2C string to the specified I2C unit of ExtPack.
 *
 * This macro invokes `send_String_to_ExtPack` to transmit the data.
 */
#define send_ExtPack_I2C_String send_String_to_ExtPack

/**
 * Sends given data to slave of I2C_Unit of ExtPack by setting the partner adr of the unit followed by sending the data to set slave.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param partner_adr The partner address to send the data to.
 * @param data The data to be sent.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 */
ext_pack_error_t send_ExtPack_I2C_data_to_partner(unit_t unit, uint8_t partner_adr, uint8_t data);

/**
 * Sends a set_ExtPack_I2C_partner_adr control message followed by the given String until '\0' to ExtPack with send chosen mode.
 * If a send char operation fails the function aborts (can only happen if max_attempts is not set to zero) and returns an error.
 *
 * @note max_attempts equal 1 is equivalent to a set_ExtPack_I2C_partner_adr or a normal send operation per send_ExtPack_I2C_data.
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The ExtPack unit to which the data should be sent.
 * @param partner_adr The partner address to send the data to.
 * @param data The data to be sent as String with terminating '\0'.
 * @param delay_us The delay waited between two sent messages. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a message. 0 for unlimited retries.
 * @param retry_delay_us The delay between send message attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE if the function was aborted when sending a char because of an error while sending. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t send_ExtPack_I2C_String_to_partner(unit_t unit, uint8_t partner_adr, const uint8_t* data, uint16_t delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

// ------------------ GPIO_Unit interface ------------------

/**
 * @def refresh_ExtPack_gpio_data(unit)
 * @brief Requests the current input pin states of a GPIO unit.
 *
 * @param unit The target GPIO unit.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with access mode 01 and 0x00 as data.
 */
#define refresh_ExtPack_gpio_data(unit) send_to_ExtPack(set_ExtPack_access_mode(unit, 01), 0x00)

/**
 * @def get_ExtPack_data_gpio_in
 * @brief Retrieves the last received values of the given ExtPack GPIO unit's input pins.
 *
 * @param unit The GPIO unit of ExtPack from which the values are retrieved.
 * @return The last received input values of the GPIO unit.
 *
 * This macro invokes 'get_ExtPack_stored_unit_input_values' to get the data.
 */
#define get_ExtPack_data_gpio_in get_ExtPack_stored_unit_input_values

/**
 * @def get_ExtPack_data_gpio_out
 * @brief Retrieves the last sent values of the given ExtPack GPIO unit's output pins.
 *
 * @param unit The GPIO unit of ExtPack from which the values are retrieved.
 * @return The last sent output values of the GPIO unit.
 *
 * This macro invokes 'get_ExtPack_stored_unit_output_values' to get the data.
 */
#define get_ExtPack_data_gpio_out get_ExtPack_stored_unit_output_values

/**
 * @def set_ExtPack_gpio_out
 * @brief Alias to send output pin values to the specified GPIO unit of ExtPack.
 *
 * This macro invokes `send_to_ExtPack` to transmit the data.
 */
#define set_ExtPack_gpio_out send_to_ExtPack

// -------------------- Timer interface --------------------

/**
 * @def set_ExtPack_timer_enable(unit, enable)
 * @brief Sends a command to enable or disable the specified Timer unit of ExtPack.
 *
 * @param unit The Timer unit of ExtPack whose enable state should be set.
 * @param enable Set to 0 to disable, any nonzero value to enable.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with access mode 00 to send the data.
 */
#define set_ExtPack_timer_enable(unit, enable) send_to_ExtPack(set_ExtPack_access_mode(unit, 00), enable)

/**
 * @def restart_ExtPack_timer(unit)
 * @brief Sends a command to restart the specified Timer unit of ExtPack.
 *
 * @param unit The Timer unit of ExtPack to be restarted.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with access mode 01 and 0x00 as data.
 */
#define restart_ExtPack_timer(unit) send_to_ExtPack(set_ExtPack_access_mode(unit, 01), 0x00)

/**
 * @def set_ExtPack_timer_prescaler(unit, prescaler_divisor)
 * @brief Sends the timer prescaler divisor to ExtPack for the specified Timer unit.
 *
 * @param unit The Timer unit of ExtPack for which the prescaler divisor is set.
 * @param prescaler_divisor The prescaler divisor value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with access mode 10 to send the data.
 */
#define set_ExtPack_timer_prescaler(unit, prescaler_divisor) send_to_ExtPack(set_ExtPack_access_mode(unit, 10), prescaler_divisor)

/**
 * @def set_ExtPack_timer_start_value(unit, start_value)
 * @brief Sends the new start value to ExtPack for the specified Timer unit.
 *
 * @param unit The Timer unit of ExtPack for which the start value is set.
 * @param start_value The start value to be applied.
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure.
 *
 * This macro invokes 'send_to_ExtPack' with access mode 11 to send the data.
 */
#define set_ExtPack_timer_start_value(unit, start_value) send_to_ExtPack(set_ExtPack_access_mode(unit, 11), start_value)

/**
 * Configures the timer with prescaler divisor and start value.
 * Additionally, the timer is restarted and enabled.
 *
 * @note Use max_attempts equal zero for unlimited retries. Please use this only if absolutely necessary.
 *
 * @warning Do not use SEND_MAX_ATTEMPTS on this function.
 *
 * @param unit The Timer unit of ExtPack which to configure.
 * @param prescaler_divisor The prescaler divisor value to be applied.
 * @param start_value The start value to be applied.
 * @param send_delay_us The delay waited between two sent commands. (Usually set between 100-1000us)
 * @param max_attempts The maximum attempts to send a command. 0 for unlimited retries.
 * @param retry_delay_us The delay between two send command attempts. (Usually set between 100-1000us)
 * @return EXT_PACK_SUCCESS on success, EXT_PACK_FAILURE on failure. If max_attempts is set to zero always EXT_PACK_SUCCESS is returned.
 */
ext_pack_error_t configure_ExtPack_timer(unit_t unit, uint8_t prescaler_divisor, uint8_t start_value, uint16_t send_delay_us, uint8_t max_attempts, uint16_t retry_delay_us);

#endif //EXTPACK