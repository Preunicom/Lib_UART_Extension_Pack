#include "ExtPack_Events.h"
#include "ExtPack_Internal.h"

volatile uint64_t unit_events = 0;

void set_ExtPack_event(unit_t unit) {
    enter_critical_zone();
    unit_events |= ((uint64_t)1 << unit); // Cast necessary, otherwise treated as a unit_t (uint8_t) --> Max shift: 7
    exit_critical_zone();
}

uint8_t get_ExtPack_event(unit_t unit) {
    return (unit_events & ((uint64_t)1 << unit)) > 0; // Cast necessary, see reason above
}

void clear_ExtPack_event(unit_t unit) {
    enter_critical_zone();
    unit_events &= ~((uint64_t)1 << unit); // Cast necessary, see reason above
    exit_critical_zone();
}

void reset_ExtPack_events() {
    unit_events = 0;
}