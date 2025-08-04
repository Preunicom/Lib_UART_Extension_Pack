#include "ExtPack_Ringbuffer_Internal.h"

void init_ringbuffer_metadata(ringbuffer_t buf, uint8_t buf_len, volatile ringbuffer_metadata_t* metadata) {
    metadata->data = buf;
    metadata->buf_len = buf_len;
    metadata->free_slots = buf_len;
    metadata->next_read_slot_index = 0;
    metadata->next_write_slot_index = 0;
}

ext_pack_error_t write_buf(volatile ringbuffer_metadata_t* metadata, ringbuffer_elem_t data) {
    enter_critical_zone();
    if(metadata->free_slots == 0) {
        exit_critical_zone();
        return EXT_PACK_FAILURE;
    }
    metadata->data[metadata->next_write_slot_index] = data;
    metadata->next_write_slot_index = (metadata->next_write_slot_index + 1) % metadata->buf_len;
    metadata->free_slots--;
    exit_critical_zone();
    return EXT_PACK_SUCCESS;
}

ext_pack_error_t read_buf(volatile ringbuffer_metadata_t* metadata, ringbuffer_elem_t* data) {
    enter_critical_zone();
    if (metadata->free_slots == metadata->buf_len) {
        exit_critical_zone();
        return EXT_PACK_FAILURE; // Error, nothing to read
    }
    uint8_t current_read_index = metadata->next_read_slot_index;
    metadata->next_read_slot_index = (metadata->next_read_slot_index + 1) % metadata->buf_len;
    metadata->free_slots++;
    *data = metadata->data[current_read_index];
    exit_critical_zone();
    return EXT_PACK_SUCCESS;
}
