#ifndef BUTTON_H
#define BUTTON_H

/*
 * Turns the button level into press / long-press events.
 *
 * The button is debounced in hardware
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_PRESS,
    BUTTON_EVENT_LONG_PRESS,
} button_event_t;

typedef struct
{
    bool was_pressed;
    bool long_press_sent;
    uint32_t pressed_at_ms;
} button_t;

button_event_t button_update(button_t *button, bool pressed, uint32_t now_ms);

#endif /* BUTTON_H */
