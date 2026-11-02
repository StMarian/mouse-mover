#include "unity.h"

#include "display_probe.h"
#include "fake_bsp.h"
#include "seg7.h"

#define SEGMENTS_0 0x3Fu

void setUp(void)
{
    fake_bsp_reset();
    seg7_set_blinking(false);
    seg7_show_number(0u);
}

void tearDown(void)
{
}

static void test_shows_a_two_digit_number(void)
{
    seg7_show_number(42u);

    TEST_ASSERT_EQUAL_INT(42, display_read());
}

static void test_a_single_digit_number_keeps_its_leading_zero(void)
{
    seg7_show_number(7u);

    TEST_ASSERT_EQUAL_INT(7, display_read());
    TEST_ASSERT_EQUAL_HEX8(SEGMENTS_0, g_fake_bsp.digit_segments[0]);
}

static void test_numbers_above_99_show_dashes(void)
{
    seg7_show_number(100u);

    TEST_ASSERT_EQUAL_INT(DISPLAY_DASHES, display_read());
}

static void test_show_dashes(void)
{
    seg7_show_dashes();

    TEST_ASSERT_EQUAL_INT(DISPLAY_DASHES, display_read());
}

static void test_each_refresh_lights_the_other_digit(void)
{
    seg7_refresh_isr();
    const uint8_t first = g_fake_bsp.last_digit;

    seg7_refresh_isr();
    TEST_ASSERT_NOT_EQUAL(first, g_fake_bsp.last_digit);

    seg7_refresh_isr();
    TEST_ASSERT_EQUAL_UINT8(first, g_fake_bsp.last_digit);
}

static void test_a_new_number_appears_on_the_next_refresh(void)
{
    seg7_show_number(12u);
    TEST_ASSERT_EQUAL_INT(12, display_read());

    seg7_show_number(34u);
    TEST_ASSERT_EQUAL_INT(34, display_read());
}

static void test_blinking_turns_the_display_off_every_other_100_ms(void)
{
    seg7_show_number(5u);
    seg7_set_blinking(true);

    g_fake_bsp.millis = 0u;
    TEST_ASSERT_EQUAL_INT(5, display_read());
    g_fake_bsp.millis = 99u;
    TEST_ASSERT_EQUAL_INT(5, display_read());
    g_fake_bsp.millis = 100u;
    TEST_ASSERT_EQUAL_INT(DISPLAY_BLANK, display_read());
    g_fake_bsp.millis = 199u;
    TEST_ASSERT_EQUAL_INT(DISPLAY_BLANK, display_read());
    g_fake_bsp.millis = 200u;
    TEST_ASSERT_EQUAL_INT(5, display_read());
}

static void test_without_blinking_the_display_stays_on(void)
{
    seg7_show_number(5u);

    g_fake_bsp.millis = 100u; // a "blank" phase if blinking were on
    TEST_ASSERT_EQUAL_INT(5, display_read());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_shows_a_two_digit_number);
    RUN_TEST(test_a_single_digit_number_keeps_its_leading_zero);
    RUN_TEST(test_numbers_above_99_show_dashes);
    RUN_TEST(test_show_dashes);
    RUN_TEST(test_each_refresh_lights_the_other_digit);
    RUN_TEST(test_a_new_number_appears_on_the_next_refresh);
    RUN_TEST(test_blinking_turns_the_display_off_every_other_100_ms);
    RUN_TEST(test_without_blinking_the_display_stays_on);
    return UNITY_END();
}
