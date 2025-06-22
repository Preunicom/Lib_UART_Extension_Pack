/**
* @file ExtPack_Events_Internal.h
 *
 * @brief Inline event handling for the UART Extension Pack and definition of the events in an uint64_t with each bit as an event.
 *
 * This file manages events for all 64 units of ExtPack as inline functions.
 *
 * ## Features:
 * - Inline event management interface for ExtPack unit events.
 *
 * @author Markus Remy
 * @date 22.06.2025
 */

#ifndef EXTPACK_EVENTS_INTERNAL_H
#define EXTPACK_EVENTS_INTERNAL_H

#include "ExtPack_Defs.h"

/**
 * @var unit_events
 * @brief 64-bit integer representing the events of all 64 units.
 *
 * @note Each bit of the uint64_t represents a event of a unit.
 */
extern uint64_t unit_events;

/**
 * @brief Sets the event of the unit in unit_events to 1.
 * @note Inline function to use internally in ExtPack modules.
 *
 * @param unit The unit to set the event for.
 */
extern inline void set_ExtPack_event(unit_t unit);

/**
 * @brief Returns the event of the unit stored in unit_events.
 * @note Inline function to use internally in ExtPack modules.
 *
 * @param unit The unit to get the event from.
 * @return Event set (1) or event not set (0).
 */
extern inline uint8_t get_ExtPack_event(unit_t unit);

/**
 * @brief Clears the event of the unit in unit_events.
 * @note Inline function to use internally in ExtPack modules.
 *
 * @param unit The unit to clear the event from.
 */
extern inline void clear_ExtPack_event(unit_t unit);

/**
 * @brief Clears events of all units in unit_events.
 * @note Inline function to use internally in ExtPack modules.
 */
extern inline void reset_ExtPack_events();

#endif //EXTPACK_EVENTS_INTERNAL_H
