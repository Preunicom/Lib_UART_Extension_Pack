#include "ExtPack_Events.h"
#include "ExtPack_Events.h"
#include "ExtPack_Internal.h"
#include "ExtPack_LL_M328P.h"

uint64_t unit_events = 0;

void set_ExtPack_event(unit_t unit) {
    enter_critical_zone();
    unit_events |= (1 << unit);
    exit_critical_zone();
}

uint8_t get_ExtPack_event(unit_t unit) {
    return (unit_events & (1 << unit)) > 0;
}

void clear_ExtPack_event(unit_t unit) {
    enter_critical_zone();
    unit_events &= ~(1 << unit);
    exit_critical_zone();
}

void reset_ExtPack_events() {
    unit_events = 0;
}