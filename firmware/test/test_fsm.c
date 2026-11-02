#include "unity.h"

#include "app_config.h"
#include "display_probe.h"
#include "fake_bsp.h"
#include "fsm.h"
#include "settings.h"

/*
 * Behaviour of the state machine, as seen on the board: what the display
 * shows, when the motor runs, what ends up in flash.
 *
 * Uses the real display driver and settings module on the fake board. Built
 * twice: with 1 s countdown units (Debug firmware) and 60 s units (Release).
 * Expected values come from app_config.h, so retuning the device does not
 * break the tests.
 */

static const uint8_t k_periods[] = {COUNTDOWN_PERIOD_OPTIONS};

#define PERIOD_COUNT  ((uint8_t)(sizeof(k_periods) / sizeof(k_periods[0])))
#define DEFAULT_INDEX ((uint8_t)COUNTDOWN_DEFAULT_PERIOD_INDEX)

static uint8_t period_at(uint32_t index)
{
    return k_periods[index % PERIOD_COUNT];
}

static uint32_t seconds_of(uint8_t units)
{
    return (uint32_t)units * COUNTDOWN_UNIT_SECONDS;
}

static void ticks(uint32_t count)
{
    for (uint32_t i = 0u; i < count; i++)
    {
        fsm_on_tick();
    }
}

static uint32_t ticks_until_motor_starts(void)
{
    const uint32_t limit = seconds_of(UINT8_MAX);
    uint32_t count = 0u;

    while (!g_fake_bsp.motor_on && (count < limit))
    {
        fsm_on_tick();
        count++;
    }
    return count;
}

static int shown(void)
{
    return display_read_ignoring_blink();
}

static void save_period_index(uint8_t index)
{
    const settings_t settings = {.period_index = index};
    TEST_ASSERT_TRUE(settings_save(&settings));
}

void setUp(void)
{
    fake_bsp_reset();
    fsm_init();
}

void tearDown(void)
{
}

// --- Power-up ------------------------------------------------------------------

static void test_boots_with_the_default_period_when_nothing_is_saved(void)
{
    TEST_ASSERT_FALSE(g_fake_bsp.motor_on);
    TEST_ASSERT_FALSE(display_is_blinking());
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());
}

static void test_boots_with_the_saved_period(void)
{
    save_period_index(0u);
    fsm_init();

    TEST_ASSERT_EQUAL_INT(period_at(0u), shown());
}

static void test_ignores_a_saved_period_index_that_is_out_of_range(void)
{
    save_period_index(PERIOD_COUNT); // a valid record holding a bad index
    fsm_init();

    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());
}

// --- Countdown and pulse -------------------------------------------------------

static void test_the_display_counts_down_one_unit_at_a_time(void)
{
    ticks(COUNTDOWN_UNIT_SECONDS - 1u);
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());

    ticks(1u);
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX) - 1, shown());
}

static void test_the_display_shows_1_during_the_last_second(void)
{
    ticks(seconds_of(period_at(DEFAULT_INDEX)) - 1u);

    TEST_ASSERT_EQUAL_INT(1, shown());
    TEST_ASSERT_FALSE(g_fake_bsp.motor_on);
}

static void test_the_first_pulse_comes_one_full_period_after_power_up(void)
{
    TEST_ASSERT_EQUAL_UINT32(seconds_of(period_at(DEFAULT_INDEX)), ticks_until_motor_starts());
    TEST_ASSERT_EQUAL_INT(DISPLAY_DASHES, shown());
}

static void test_the_pulse_lasts_MOTOR_PULSE_SECONDS(void)
{
    ticks_until_motor_starts();

    ticks(MOTOR_PULSE_SECONDS - 1u);
    TEST_ASSERT_TRUE(g_fake_bsp.motor_on);

    ticks(1u);
    TEST_ASSERT_FALSE(g_fake_bsp.motor_on);
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());
}

static void test_after_a_pulse_the_next_one_comes_one_full_period_later(void)
{
    ticks_until_motor_starts();
    ticks(MOTOR_PULSE_SECONDS);

    TEST_ASSERT_EQUAL_UINT32(seconds_of(period_at(DEFAULT_INDEX)), ticks_until_motor_starts());
}

// --- Settings mode -------------------------------------------------------------

static void test_a_long_press_opens_settings_showing_the_current_period(void)
{
    fsm_on_long_press();

    TEST_ASSERT_TRUE(display_is_blinking());
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());
}

static void test_a_long_press_during_a_pulse_stops_the_motor(void)
{
    ticks_until_motor_starts();

    fsm_on_long_press();

    TEST_ASSERT_FALSE(g_fake_bsp.motor_on);
    TEST_ASSERT_TRUE(display_is_blinking());
}

static void test_a_short_press_outside_settings_does_nothing(void)
{
    fsm_on_press();

    TEST_ASSERT_FALSE(display_is_blinking());
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());
}

static void test_short_presses_step_through_every_period_and_wrap_around(void)
{
    fsm_on_long_press();

    for (uint32_t step = 1u; step <= PERIOD_COUNT; step++)
    {
        fsm_on_press();
        TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX + step), shown());
    }
}

static void test_a_long_press_inside_settings_is_ignored(void)
{
    fsm_on_long_press();
    fsm_on_press();

    fsm_on_long_press();

    TEST_ASSERT_TRUE(display_is_blinking());
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX + 1u), shown());
}

static void test_settings_save_and_close_after_the_timeout(void)
{
    fsm_on_long_press();
    fsm_on_press();

    ticks(CONFIG_TIMEOUT_SECONDS - 1u);
    TEST_ASSERT_TRUE(display_is_blinking());
    TEST_ASSERT_EQUAL_UINT32(0u, g_fake_bsp.flash_writes);

    ticks(1u);
    TEST_ASSERT_FALSE(display_is_blinking());
    TEST_ASSERT_EQUAL_UINT32(1u, g_fake_bsp.flash_writes);
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX + 1u), shown());
}

static void test_each_press_restarts_the_settings_timeout(void)
{
    fsm_on_long_press();
    ticks(CONFIG_TIMEOUT_SECONDS - 1u);

    fsm_on_press();
    ticks(CONFIG_TIMEOUT_SECONDS - 1u);
    TEST_ASSERT_TRUE(display_is_blinking());

    ticks(1u);
    TEST_ASSERT_FALSE(display_is_blinking());
}

static void test_after_settings_close_the_countdown_runs_the_new_full_period(void)
{
    fsm_on_long_press();
    fsm_on_press();
    ticks(CONFIG_TIMEOUT_SECONDS);

    TEST_ASSERT_EQUAL_UINT32(seconds_of(period_at(DEFAULT_INDEX + 1u)), ticks_until_motor_starts());
}

static void test_the_new_period_survives_a_restart(void)
{
    fsm_on_long_press();
    fsm_on_press();
    ticks(CONFIG_TIMEOUT_SECONDS);

    fsm_init(); // power cycle

    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX + 1u), shown());
}

static void test_closing_settings_without_a_change_does_not_rewrite_the_flash(void)
{
    fsm_on_long_press();
    ticks(CONFIG_TIMEOUT_SECONDS); // first save: the page was empty
    TEST_ASSERT_EQUAL_UINT32(1u, g_fake_bsp.flash_writes);

    fsm_on_long_press();
    ticks(CONFIG_TIMEOUT_SECONDS); // same value again

    TEST_ASSERT_EQUAL_UINT32(1u, g_fake_bsp.flash_writes);
}

static void test_a_failed_save_keeps_the_new_period_until_the_next_restart(void)
{
    g_fake_bsp.flash_write_fails = true;

    fsm_on_long_press();
    fsm_on_press();
    ticks(CONFIG_TIMEOUT_SECONDS);
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX + 1u), shown());

    fsm_init(); // power cycle: nothing was saved
    TEST_ASSERT_EQUAL_INT(period_at(DEFAULT_INDEX), shown());
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_boots_with_the_default_period_when_nothing_is_saved);
    RUN_TEST(test_boots_with_the_saved_period);
    RUN_TEST(test_ignores_a_saved_period_index_that_is_out_of_range);

    RUN_TEST(test_the_display_counts_down_one_unit_at_a_time);
    RUN_TEST(test_the_display_shows_1_during_the_last_second);
    RUN_TEST(test_the_first_pulse_comes_one_full_period_after_power_up);
    RUN_TEST(test_the_pulse_lasts_MOTOR_PULSE_SECONDS);
    RUN_TEST(test_after_a_pulse_the_next_one_comes_one_full_period_later);

    RUN_TEST(test_a_long_press_opens_settings_showing_the_current_period);
    RUN_TEST(test_a_long_press_during_a_pulse_stops_the_motor);
    RUN_TEST(test_a_short_press_outside_settings_does_nothing);
    RUN_TEST(test_short_presses_step_through_every_period_and_wrap_around);
    RUN_TEST(test_a_long_press_inside_settings_is_ignored);
    RUN_TEST(test_settings_save_and_close_after_the_timeout);
    RUN_TEST(test_each_press_restarts_the_settings_timeout);
    RUN_TEST(test_after_settings_close_the_countdown_runs_the_new_full_period);
    RUN_TEST(test_the_new_period_survives_a_restart);
    RUN_TEST(test_closing_settings_without_a_change_does_not_rewrite_the_flash);
    RUN_TEST(test_a_failed_save_keeps_the_new_period_until_the_next_restart);

    return UNITY_END();
}
