#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

/*
 * Events raised in interrupt context and handled in the main loop.
 *
 * Interrupts never touch application state directly: they post an event and
 * return. Single producer / single consumer, lock-free
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    APP_EVENT_COUNTDOWN_TICK = 0, // TIM2, 1 Hz
} app_event_t;

// From interrupt handlers
bool event_queue_push(app_event_t event);

// From main loop
bool event_queue_pop(app_event_t *event);

uint32_t event_queue_dropped(void);

#endif /* EVENT_QUEUE_H */
