#include "fsm.h"

#include <stddef.h>
#include <stdint.h>

#include "app_config.h"
#include "bsp.h"
#include "seg7.h"
#include "settings.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

typedef enum
{
    FSM_STATE_COUNTDOWN,
    FSM_STATE_ACTIVE,
    FSM_STATE_CONFIGURE,
} fsm_state_t;

typedef enum
{
    FSM_EVENT_TIMER_EXPIRED,
    FSM_EVENT_PRESS,
    FSM_EVENT_LONG_PRESS,
} fsm_event_t;

typedef struct
{
    fsm_state_t from;
    fsm_event_t event;
    fsm_state_t to;
    void (*action)(void);
} fsm_transition_t;

static const uint8_t k_period_options[] = {COUNTDOWN_PERIOD_OPTIONS};

_Static_assert(COUNTDOWN_DEFAULT_PERIOD_INDEX < ARRAY_SIZE(k_period_options), "default period index out of range");
_Static_assert(ARRAY_SIZE(k_period_options) <= UINT8_MAX, "period index is a uint8_t");

static fsm_state_t s_state = FSM_STATE_COUNTDOWN;
static uint8_t s_period_index = COUNTDOWN_DEFAULT_PERIOD_INDEX;
static uint32_t s_seconds_left = 0u; // until the current state's timer expires

static uint32_t get_period_seconds(void)
{
    return (uint32_t)k_period_options[s_period_index] * COUNTDOWN_UNIT_SECONDS;
}

static void show_time_left(void)
{
    seg7_show_number((uint16_t)((s_seconds_left + COUNTDOWN_UNIT_SECONDS - 1u) / COUNTDOWN_UNIT_SECONDS));
}

static void show_period_option(void)
{
    seg7_show_number(k_period_options[s_period_index]);
}

static void start_countdown(uint32_t seconds)
{
    s_seconds_left = seconds;
    show_time_left();
}

// --- Transition actions -------------------------------------------------------

static void start_pulse(void)
{
    bsp_motor_set(true);
    seg7_show_dashes();

    s_seconds_left = MOTOR_PULSE_SECONDS;
}

static void end_pulse(void)
{
    bsp_motor_set(false);

    start_countdown(get_period_seconds());
}

static void open_config(void)
{
    bsp_motor_set(false);
    seg7_set_blinking(true);

    show_period_option();
    s_seconds_left = CONFIG_TIMEOUT_SECONDS;
}

static void next_period(void)
{
    s_period_index = (uint8_t)((s_period_index + 1u) % ARRAY_SIZE(k_period_options));
    show_period_option();

    s_seconds_left = CONFIG_TIMEOUT_SECONDS; // restart the timeout
}

static void close_config(void)
{
    const settings_t settings = {.period_index = s_period_index};

    seg7_set_blinking(false);

    (void)settings_save(&settings);

    start_countdown(get_period_seconds());
}

// clang-format off
static const fsm_transition_t k_transitions[] = {
//    from                 event                    to                   action
    { FSM_STATE_COUNTDOWN, FSM_EVENT_TIMER_EXPIRED, FSM_STATE_ACTIVE,    start_pulse  },
    { FSM_STATE_ACTIVE,    FSM_EVENT_TIMER_EXPIRED, FSM_STATE_COUNTDOWN, end_pulse    },
    { FSM_STATE_COUNTDOWN, FSM_EVENT_LONG_PRESS,    FSM_STATE_CONFIGURE, open_config  },
    { FSM_STATE_ACTIVE,    FSM_EVENT_LONG_PRESS,    FSM_STATE_CONFIGURE, open_config  },
    { FSM_STATE_CONFIGURE, FSM_EVENT_PRESS,         FSM_STATE_CONFIGURE, next_period  },
    { FSM_STATE_CONFIGURE, FSM_EVENT_TIMER_EXPIRED, FSM_STATE_COUNTDOWN, close_config },
};
// clang-format on

static void dispatch(fsm_event_t event)
{
    for (size_t i = 0u; i < ARRAY_SIZE(k_transitions); i++)
    {
        const fsm_transition_t *transition = &k_transitions[i];

        if ((transition->from == s_state) && (transition->event == event))
        {
            transition->action();
            s_state = transition->to;
            return;
        }
    }
}

// --- Public interface ---------------------------------------------------------

void fsm_init(void)
{
    settings_t settings;

    s_period_index = COUNTDOWN_DEFAULT_PERIOD_INDEX;
    if (settings_load(&settings) && (settings.period_index < ARRAY_SIZE(k_period_options)))
    {
        s_period_index = settings.period_index;
    }

    s_state = FSM_STATE_COUNTDOWN;
    bsp_motor_set(false);
    seg7_set_blinking(false);

    start_countdown(get_period_seconds());
}

void fsm_on_tick(void)
{
    if (s_seconds_left > 0u)
    {
        s_seconds_left--;
    }

    if (s_state == FSM_STATE_COUNTDOWN)
    {
        show_time_left();
    }

    if (s_seconds_left == 0u)
    {
        dispatch(FSM_EVENT_TIMER_EXPIRED);
    }
}

void fsm_on_press(void)
{
    dispatch(FSM_EVENT_PRESS);
}

void fsm_on_long_press(void)
{
    dispatch(FSM_EVENT_LONG_PRESS);
}
