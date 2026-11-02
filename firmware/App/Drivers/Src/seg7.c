#include "seg7.h"

#include "bsp.h"

/*
 * Segment bit layout
 *
 *      bit  6  5  4  3  2  1  0
 *           g  f  e  d  c  b  a
 *
 *        aaa
 *       f   b
 *        ggg
 *       e   c
 *        ddd
 */
static const uint8_t k_digit_segments[10] = {
    0x3Fu, // 0 : a b c d e f
    0x06u, // 1 :   b c
    0x5Bu, // 2 : a b   d e   g
    0x4Fu, // 3 : a b c d     g
    0x66u, // 4 :   b c     f g
    0x6Du, // 5 : a   c d   f g
    0x7Du, // 6 : a   c d e f g
    0x07u, // 7 : a b c
    0x7Fu, // 8 : a b c d e f g
    0x6Fu, // 9 : a b c d   f g
};

#define SEGMENTS_DASH        0x40u // g
#define BLINK_HALF_PERIOD_MS 100u

/*
 * Written by the main loop, read by the refresh interrupt. Both digits are
 * packed into one halfword so a new value is published with a single 16-bit
 * store
 *
 *   bits 15..8 = tens segments, bits 7..0 = units segments
 */
static volatile uint16_t s_segments = 0u;
static volatile bool s_blinking = false;

// Used only inside the interrupt.
static uint8_t s_active_digit = 0u;

static void publish(uint8_t tens, uint8_t units)
{
    s_segments = (uint16_t)(((uint16_t)tens << 8) | units);
}

void seg7_show_number(uint16_t value)
{
    if (value > 99u)
    {
        seg7_show_dashes();
        return;
    }

    publish(k_digit_segments[value / 10u], k_digit_segments[value % 10u]);
}

void seg7_show_dashes(void)
{
    publish(SEGMENTS_DASH, SEGMENTS_DASH);
}

void seg7_set_blinking(bool blinking)
{
    s_blinking = blinking;
}

void seg7_refresh_isr(void)
{
    s_active_digit = (uint8_t)(s_active_digit ^ 1u);

    const bool blank_phase = s_blinking && (((bsp_millis() / BLINK_HALF_PERIOD_MS) & 1u) != 0u);
    if (blank_phase)
    {
        bsp_display_off();
        return;
    }

    const uint16_t segments = s_segments; // read the shared value once
    const uint8_t digit_segments = (s_active_digit == 0u) ? (uint8_t)(segments >> 8) : (uint8_t)segments;

    bsp_display_show(s_active_digit, digit_segments);
}
