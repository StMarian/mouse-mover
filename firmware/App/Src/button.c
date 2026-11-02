#include "button.h"

#include "app_config.h"

button_event_t button_update(button_t *button, bool pressed, uint32_t now_ms)
{
    button_event_t event = BUTTON_EVENT_NONE;

    if (pressed && !button->was_pressed)
    {
        button->pressed_at_ms = now_ms;
        button->long_press_sent = false;
    }
    else if (pressed && !button->long_press_sent && ((now_ms - button->pressed_at_ms) >= BUTTON_LONG_PRESS_MS))
    {
        button->long_press_sent = true;
        event = BUTTON_EVENT_LONG_PRESS;
    }
    else if (!pressed && button->was_pressed && !button->long_press_sent)
    {
        // A short press counts on release: only then is it known not to be a long press.
        event = BUTTON_EVENT_PRESS;
    }
    else
    {
        // Nothing to do
    }

    button->was_pressed = pressed;
    return event;
}
