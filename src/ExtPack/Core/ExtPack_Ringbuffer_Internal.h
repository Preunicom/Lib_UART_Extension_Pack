/**
 * @file ExtPack_Ringbuffer_Internal.h
 *
 * @brief Header file for the ringbuffer as interface between low level UART ExtPack communication and high level logic.
 *
 * @layer Core
 *
 * @warning This file is only for access for ExtPack library functions. The user should not directly use this header file.
 *
 *
 * ## Features:
 * - Initialize ringbuffer
 * - Read ringbuffer
 * - Write ringbuffer
 *
 * @author Markus Remy
 * @date 22.06.2025
 */

#ifndef EXTPACK_RINGBUFFER_INTERNAL_H
#define EXTPACK_RINGBUFFER_INTERNAL_H

#include "ExtPack.h"

/**
 * @brief Typedef for the ringbuffer memory pointer.
 *
 * @layer Core
 *
 * Represents a pointer to the memory area (buffer) used for storing ringbuffer elements.
 * This buffer is externally provided and managed by the ringbuffer metadata.
 */
typedef volatile uint16_t* ringbuffer_t;

/**
 * @brief Type of individual elements stored in the ringbuffer.
 *
 * @layer Core
 *
 * Each slot in the ringbuffer holds a value of type `uint16_t`.
 */
typedef uint16_t ringbuffer_elem_t;

/**
 * @brief Metadata structure for managing the ringbuffer state.
 *
 * @layer Core
 *
 * @details Contains buffer length, available slot count, and indices for reading and writing.
 * Required to control the behavior of the ringbuffer.
 */
typedef struct {
    /**
     * @brief Pointer to the ringbuffer memory array.
     *
     * @layer Core
     *
     * This points to the memory block that stores the actual ringbuffer data elements.
     */
    ringbuffer_t data;
    /**
     * @brief Total number of slots in the ringbuffer.
     *
     * @layer Core
     *
     * This defines the maximum capacity of the ringbuffer.
     */
    uint8_t buf_len;
    /**
     * @brief Number of currently free slots in the buffer.
     *
     * @layer Core
     *
     * @details Decreases with every write and increases with every read operation.
     */
    uint8_t free_slots;
    /**
     * @brief Index of the next slot to read from.
     *
     * @layer Core
     */
    uint8_t next_read_slot_index;
    /**
     * @brief Index of the next slot to write to.
     *
     * @layer Core
     */
    uint8_t next_write_slot_index;
} ringbuffer_metadata_t;

/**
 * @brief Initializes the metadata of the ringbuffer given as pointer.
 *
 * @layer Core
 *
 * @note Create a ringbuffer with wished length first (ringbuffer_elem_t buf[buf_len];)
 * as well as the metadata struct (ringbuffer_metadata_t xyz;) and pass both as well as the chosen buffer length here.
 *
 * @param buf A pointer to the ringbuffer to connect to the metadata.
 * @param buf_len The length of the ringbuffer to connect.
 * @param metadata A pointer to the metadata to initialize.
 */
void init_ringbuffer_metadata(ringbuffer_t buf, uint8_t buf_len, volatile ringbuffer_metadata_t* metadata);

/**
 * @brief Writes an element in the buffer when enough slots are free.
 *
 * @layer Core
 *
 * @param metadata The metadata of the ringbuffer to work with.
 * @param data The data to write.
 * @return EXT_PACK_SUCCESS if successful, EXT_PACK_FAILURE if no spot to write available.
 */
ext_pack_error_t write_buf(volatile ringbuffer_metadata_t* metadata, ringbuffer_elem_t data);

/**
 * @brief Read an element of the buffer when something to read is available.
 *
 * @layer Core
 *
 * @param metadata The metadata of the ringbuffer to read from.
 * @param data A pointer to the data slot to store the result if read is successful.
 * @return EXT_PACK_SUCCESS if successful, EXT_PACK_FAILURE if nothing to read available.
 */
ext_pack_error_t read_buf(volatile ringbuffer_metadata_t* metadata, ringbuffer_elem_t* data);

/**
 * @brief Checks if the given buffer is full.
 *
 * @layer Core
 *
 * @param metadata The buffer metadata to check.
 * @return 1 if full, 0 otherwise.
 */
static inline uint8_t is_buf_full(volatile ringbuffer_metadata_t* metadata) {
    return metadata->free_slots == 0;
}

/**
 * @brief Checks if the given buffer is empty.
 *
 * @layer Core
 *
 * @param metadata The buffer metadata to check.
 * @return 1 if empty, 0 otherwise.
 */
static inline uint8_t is_buf_empty(volatile ringbuffer_metadata_t* metadata) {
    return metadata->free_slots == metadata->buf_len;
}

#endif //EXTPACK_RINGBUFFER_INTERNAL_H
