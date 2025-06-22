/**
 * @file ExtPack_Ringbuffer_Internal.h
 *
 * @brief Header file for the ringbuffer as interface between low level UART ExtPack communication and high level logic.
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

typedef uint8_t* ringbuffer_t;
typedef uint8_t ringbuffer_elem_t;
typedef struct {
    ringbuffer_t data;
    uint8_t buf_len;
    uint8_t free_slots;
    uint8_t next_read_slot_index;
    uint8_t next_write_slot_index;
} ringbuffer_metadata_t;

/**
 * Initializes the metadata of the ringbuffer given as pointer.
 *
 * @note Create a ringbuffer with wished length first (ringbuffer_elem_t xyz[<wished_length>];)
 * as well as the metadata struct (ringbuffer_metadata_t xyz;) and pass both as well as the chosen buffer length here.
 *
 * @param buf A pointer to the ringbuffer to connect to the metadata.
 * @param buf_len The length of the ringbuffer to connect.
 * @param metadata A pointer to the metadata to initialize.
 */
static inline void init_ringbuffer_metadata(ringbuffer_t buf, uint8_t buf_len, ringbuffer_metadata_t* metadata);

/**
 * Writes an element in the buffer when enough slots are free.
 *
 * @param metadata The metadata of the ringbuffer to work with.
 * @param data The data to write.
 * @return 0 if successful, 1 otherwise.
 */
static inline uint8_t write_buf(ringbuffer_metadata_t* metadata, ringbuffer_elem_t data);

/**
 * Read a element of the buffer when something to read is available.
 *
 * @param metadata The metadata of the ringbuffer to read from.
 * @param data A pointer to the data slot to store the result if read is successful.
 * @return 0 if successful, 1 otherwise.
 */
static inline ringbuffer_elem_t read_buf(ringbuffer_metadata_t* metadata, ringbuffer_elem_t* data);

#endif //EXTPACK_RINGBUFFER_INTERNAL_H
