#include "unity.h"

#include "app.h"
#include "app_config.h"
#include "display_probe.h"
#include "event_queue.h"
#include "fake_bsp.h"

/*
 * The glue in app.c: timer interrupt -> event queue -> state machine,
 * button -> state machine, and the main loop's housekeeping.
 * Built with 1 s countdown units.
 */

static const uint8_t k_periods[] = {COUNTDOWN_PERIOD_OPTIONS};

#define DEFAULT_PERIOD ((int)k_periods[COUNTDOWN_DEFAULT_PERIOD_INDEX])
#define NEXT_PERIOD    ((int)k_periods[COUNTDOWN_DEFAULT_PERIOD_INDEX + 1u])

// Let time pass the way it does on the device: one main-loop pass per millisecond.
static void run_main_loop_ms(uint32_t ms)
{
    for (uint32_t i = 0u; i < ms; i++)
    {
        g_fake_bsp.millis++;
        app_run_iteration();
    }
}

static void press_button_ms(uint32_t ms)
{
    g_fake_bsp.button_pressed = true;
    run_main_loop_ms(ms);
    g_fake_bsp.button_pressed = false;
    run_main_loop_ms(10u);
}

void setUp(void)
{
    app_event_t leftover;
    while (event_queue_pop(&leftover))
    {
    }

    fake_bsp_reset();
    app_init();
    run_main_loop_ms(1u); // settle the button state left over from the previous test
}

void tearDown(void)
{
}

static void test_init_leaves_the_motor_off_and_shows_the_full_period(void)
{
    TEST_ASSERT_FALSE(g_fake_bsp.motor_on);
    TEST_ASSERT_EQUAL_INT(DEFAULT_PERIOD, display_read_ignoring_blink());
}

static void test_timer_ticks_wait_in_the_queue_until_the_main_loop_runs(void)
{
    app_countdown_tick_isr();
    app_countdown_tick_isr();
    TEST_ASSERT_EQUAL_INT(DEFAULT_PERIOD, display_read_ignoring_blink()); // only queued so far

    app_run_iteration();
    TEST_ASSERT_EQUAL_INT(DEFAULT_PERIOD - 2, display_read_ignoring_blink());
}

static void test_ticks_from_the_timer_interrupt_end_in_a_motor_pulse(void)
{
    for (int second = 0; second < DEFAULT_PERIOD; second++)
    {
        app_countdown_tick_isr();
        app_run_iteration();
    }

    TEST_ASSERT_TRUE(g_fake_bsp.motor_on);
}

static void test_holding_the_button_opens_settings(void)
{
    g_fake_bsp.button_pressed = true;
    run_main_loop_ms(BUTTON_LONG_PRESS_MS); // the first pass only sees the press start
    TEST_ASSERT_FALSE(display_is_blinking());

    run_main_loop_ms(1u);
    TEST_ASSERT_TRUE(display_is_blinking());
}

static void test_a_short_press_in_settings_selects_the_next_period(void)
{
    press_button_ms(BUTTON_LONG_PRESS_MS + 100u); // open settings; the release is not a press
    TEST_ASSERT_EQUAL_INT(DEFAULT_PERIOD, display_read_ignoring_blink());

    press_button_ms(50u);
    TEST_ASSERT_EQUAL_INT(NEXT_PERIOD, display_read_ignoring_blink());
}

static void test_the_display_interrupt_lights_the_display(void)
{
    TEST_ASSERT_TRUE(g_fake_bsp.display_off); // bsp_init() starts with everything off

    app_display_tick_isr();

    TEST_ASSERT_FALSE(g_fake_bsp.display_off);
}

static void test_every_main_loop_pass_kicks_the_watchdog(void)
{
    const uint32_t kicks_before = g_fake_bsp.watchdog_kicks;

    run_main_loop_ms(10u);

    TEST_ASSERT_EQUAL_UINT32(kicks_before + 10u, g_fake_bsp.watchdog_kicks);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_leaves_the_motor_off_and_shows_the_full_period);
    RUN_TEST(test_timer_ticks_wait_in_the_queue_until_the_main_loop_runs);
    RUN_TEST(test_ticks_from_the_timer_interrupt_end_in_a_motor_pulse);
    RUN_TEST(test_holding_the_button_opens_settings);
    RUN_TEST(test_a_short_press_in_settings_selects_the_next_period);
    RUN_TEST(test_the_display_interrupt_lights_the_display);
    RUN_TEST(test_every_main_loop_pass_kicks_the_watchdog);
    return UNITY_END();
}
