/**
* @file ExtPack_Events.h
 *
 * @brief Event handling for the UART Extension Pack.
 *
 * This file manages events for all 64 units of ExtPack.
 *
 * ## Features:
 * - Event management interface for ExtPack unit events.
 *
 * @author Markus Remy
 * @date 22.06.2025
 */

#ifndef EXTPACK_EVENTS_H
#define EXTPACK_EVENTS_H

#include "ExtPack_Defs.h"

/**
 * @brief Sets the event for the given ExtPack unit to 1.
 *
 * @layer Core
 *
 * @param unit The unit to set the event for.
 */
void set_ExtPack_event(unit_t unit);

/**
 * @brief Returns the event state for the given ExtPack unit.
 *
 * @layer Core
 *
 * @param unit The unit to get the event state for.
 * @return 0 if event is not set, 1 otherwise.
 */
uint8_t get_ExtPack_event(unit_t unit);

/**
 * @brief Sets the event for the given ExtPack unit to 0.
 *
 * @layer Core
 *
 * @param unit The unit to set the event for.
 */
void clear_ExtPack_event(unit_t unit);

/**
 * @brief Sets the events for all ExtPack units to 0.
 *
 * @layer Core
 */
void reset_ExtPack_events();

#endif //EXTPACK_EVENTS_H