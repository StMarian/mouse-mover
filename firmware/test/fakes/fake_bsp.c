#include "fake_bsp.h"

#include <string.h>

#include "bsp.h"
#include "bsp_flash.h"

fake_bsp_t g_fake_bsp;

void fake_bsp_reset(void)
{
    memset(&g_fake_bsp, 0, sizeof(g_fake_bsp));
    memset(g_fake_bsp.flash, 0xFF, sizeof(g_fake_bsp.flash)); // erased flash reads as 0xFF
}

// --- bsp.h --------------------------------------------------------------------

void bsp_init(void)
{
    bsp_safe_state();
}

uint32_t bsp_millis(void)
{
    return g_fake_bsp.millis;
}

void bsp_idle(void)
{
    // Nothing to wait for: the test decides when time passes.
}

void bsp_watchdog_kick(void)
{
    g_fake_bsp.watchdog_kicks++;
}

bool bsp_button_is_pressed(void)
{
    return g_fake_bsp.button_pressed;
}

void bsp_motor_set(bool on)
{
    g_fake_bsp.motor_on = on;
}

void bsp_display_show(uint8_t digit, uint8_t segments)
{
    if (digit < 2u)
    {
        g_fake_bsp.digit_segments[digit] = segments;
    }
    g_fake_bsp.last_digit = digit;
    g_fake_bsp.display_off = false;
}

void bsp_display_off(void)
{
    g_fake_bsp.display_off = true;
}

void bsp_safe_state(void)
{
    bsp_motor_set(false);
    bsp_display_off();
}

// --- bsp_flash.h --------------------------------------------------------------

const void *bsp_flash_settings_data(void)
{
    return g_fake_bsp.flash;
}

size_t bsp_flash_settings_size(void)
{
    return sizeof(g_fake_bsp.flash);
}

bool bsp_flash_settings_write(const void *data, size_t size)
{
    // Same contract as the real driver: an even number of bytes that fits the page.
    if ((data == NULL) || (size == 0u) || ((size % 2u) != 0u) || (size > sizeof(g_fake_bsp.flash)))
    {
        return false;
    }

    if (g_fake_bsp.flash_write_fails)
    {
        return false; // e.g. the unlock failed: the page is left as it was
    }

    memset(g_fake_bsp.flash, 0xFF, sizeof(g_fake_bsp.flash)); // erase
    memcpy(g_fake_bsp.flash, data, size);                     // program
    g_fake_bsp.flash_writes++;
    return true;
}
