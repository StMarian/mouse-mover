#include "unity.h"

#include "app_config.h"
#include "button.h"

static button_t s_button;
static uint32_t s_presses;
static uint32_t s_long_presses;

// Feed one level for every millisecond in [from_ms, to_ms) and count what comes out.
static void hold_level(bool pressed, uint32_t from_ms, uint32_t to_ms)
{
    for (uint32_t now = from_ms; now != to_ms; now++)
    {
        switch (button_update(&s_button, pressed, now))
        {
        case BUTTON_EVENT_PRESS:
            s_presses++;
            break;
        case BUTTON_EVENT_LONG_PRESS:
            s_long_presses++;
            break;
        case BUTTON_EVENT_NONE:
        default:
            break;
        }
    }
}

void setUp(void)
{
    s_button = (button_t){0};
    s_presses = 0u;
    s_long_presses = 0u;
}

void tearDown(void)
{
}

static void test_an_untouched_button_reports_nothing(void)
{
    hold_level(false, 0u, 2000u);

    TEST_ASSERT_EQUAL_UINT32(0u, s_presses);
    TEST_ASSERT_EQUAL_UINT32(0u, s_long_presses);
}

static void test_a_short_press_is_reported_on_release(void)
{
    hold_level(true, 0u, 100u);
    TEST_ASSERT_EQUAL_UINT32(0u, s_presses); // still held: could become a long press

    hold_level(false, 100u, 200u);
    TEST_ASSERT_EQUAL_UINT32(1u, s_presses);
    TEST_ASSERT_EQUAL_UINT32(0u, s_long_presses);
}

static void test_a_long_press_is_reported_once_when_the_threshold_is_reached(void)
{
    hold_level(true, 0u, BUTTON_LONG_PRESS_MS);
    TEST_ASSERT_EQUAL_UINT32(0u, s_long_presses); // one millisecond short

    hold_level(true, BUTTON_LONG_PRESS_MS, BUTTON_LONG_PRESS_MS + 1u);
    TEST_ASSERT_EQUAL_UINT32(1u, s_long_presses);

    hold_level(true, BUTTON_LONG_PRESS_MS + 1u, 10000u); // keep holding
    TEST_ASSERT_EQUAL_UINT32(1u, s_long_presses);
}

static void test_releasing_after_a_long_press_is_not_a_short_press(void)
{
    hold_level(true, 0u, 1000u);
    hold_level(false, 1000u, 1100u);

    TEST_ASSERT_EQUAL_UINT32(1u, s_long_presses);
    TEST_ASSERT_EQUAL_UINT32(0u, s_presses);
}

static void test_every_press_is_reported(void)
{
    hold_level(true, 0u, 50u);
    hold_level(false, 50u, 100u);
    hold_level(true, 100u, 150u);
    hold_level(false, 150u, 200u);

    TEST_ASSERT_EQUAL_UINT32(2u, s_presses);
}

static void test_a_long_press_is_still_detected_when_the_millisecond_counter_wraps(void)
{
    const uint32_t start = UINT32_MAX - 100u; // the counter wraps to 0 during the press

    hold_level(true, start, start + BUTTON_LONG_PRESS_MS + 1u);

    TEST_ASSERT_EQUAL_UINT32(1u, s_long_presses);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_an_untouched_button_reports_nothing);
    RUN_TEST(test_a_short_press_is_reported_on_release);
    RUN_TEST(test_a_long_press_is_reported_once_when_the_threshold_is_reached);
    RUN_TEST(test_releasing_after_a_long_press_is_not_a_short_press);
    RUN_TEST(test_every_press_is_reported);
    RUN_TEST(test_a_long_press_is_still_detected_when_the_millisecond_counter_wraps);
    return UNITY_END();
}
