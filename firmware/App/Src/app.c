#include "app.h"

#include "bsp.h"
#include "button.h"
#include "event_queue.h"
#include "fsm.h"
#include "seg7.h"

static button_t s_button;

static void handle_button(void)
{
    switch (button_update(&s_button, bsp_button_is_pressed(), bsp_millis()))
    {
    case BUTTON_EVENT_PRESS:
        fsm_on_press();
        break;

    case BUTTON_EVENT_LONG_PRESS:
        fsm_on_long_press();
        break;

    case BUTTON_EVENT_NONE:
    default:
        break;
    }
}

static void handle_events(void)
{
    app_event_t event;

    while (event_queue_pop(&event))
    {
        switch (event)
        {
        case APP_EVENT_COUNTDOWN_TICK:
            fsm_on_tick();
            break;

        default:
            break;
        }
    }
}

void app_init(void)
{
    bsp_init();
    fsm_init();
}

void app_run_iteration(void)
{
    bsp_watchdog_kick();
    handle_button();
    handle_events();

    // Sleep until the next interrupt
    bsp_idle();
}

void app_countdown_tick_isr(void)
{
    (void)event_queue_push(APP_EVENT_COUNTDOWN_TICK);
}

void app_display_tick_isr(void)
{
    seg7_refresh_isr();
}
