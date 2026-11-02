#include "display_probe.h"

#include <stdint.h>

#include "fake_bsp.h"
#include "seg7.h"

/*
 * The test's own copy of the digit patterns (bit 0..6 = segment a..g), so a
 * mistake in seg7.c's table shows up as a failing test instead of being
 * compared with itself.
 */
static const uint8_t k_expected_digits[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

#define SEGMENTS_DASH 0x40u

static int decode(uint8_t segments)
{
    for (int digit = 0; digit < 10; digit++)
    {
        if (k_expected_digits[digit] == segments)
        {
            return digit;
        }
    }
    return -1;
}

int display_read(void)
{
    // Each refresh lights one digit, so two refreshes light both.
    for (int i = 0; i < 2; i++)
    {
        seg7_refresh_isr();
        if (g_fake_bsp.display_off)
        {
            return DISPLAY_BLANK;
        }
    }

    const uint8_t tens = g_fake_bsp.digit_segments[0];
    const uint8_t units = g_fake_bsp.digit_segments[1];

    if ((tens == SEGMENTS_DASH) && (units == SEGMENTS_DASH))
    {
        return DISPLAY_DASHES;
    }

    const int tens_value = decode(tens);
    const int units_value = decode(units);
    if ((tens_value < 0) || (units_value < 0))
    {
        return DISPLAY_UNREADABLE;
    }

    return (tens_value * 10) + units_value;
}

int display_read_ignoring_blink(void)
{
    const uint32_t now = g_fake_bsp.millis;

    g_fake_bsp.millis = 0u; // t = 0 is in the "on" half of every blink cycle
    const int value = display_read();
    g_fake_bsp.millis = now;

    return value;
}

bool display_is_blinking(void)
{
    const uint32_t now = g_fake_bsp.millis;

    g_fake_bsp.millis = 100u; // t = 100 ms is in the "off" half of the first blink cycle
    const bool blank = (display_read() == DISPLAY_BLANK);
    g_fake_bsp.millis = now;

    return blank;
}
