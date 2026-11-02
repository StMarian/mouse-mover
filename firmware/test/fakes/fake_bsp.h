#ifndef FAKE_BSP_H
#define FAKE_BSP_H

/*
 * Fake board for host tests.
 *
 * Implements bsp.h and bsp_flash.h on plain RAM. Tests set the inputs (time,
 * button, flash failures) and check the outputs the firmware drove (motor,
 * display, flash contents) - no hardware needed.
 */

#include <stdbool.h>
#include <stdint.h>

#define FAKE_FLASH_PAGE_SIZE 1024u

typedef struct
{
    // Inputs: set by the test.
    uint32_t millis;
    bool button_pressed;
    bool flash_write_fails;

    // Outputs: driven by the firmware.
    bool motor_on;
    bool display_off;          // the last display call was bsp_display_off()
    uint8_t last_digit;        // the digit lit by the last bsp_display_show()
    uint8_t digit_segments[2]; // the last pattern lit on each digit
    uint32_t watchdog_kicks;
    uint32_t flash_writes;
    uint8_t flash[FAKE_FLASH_PAGE_SIZE];
} fake_bsp_t;

extern fake_bsp_t g_fake_bsp;

// Time 0, button released, outputs off, flash erased (every byte 0xFF).
void fake_bsp_reset(void);

#endif /* FAKE_BSP_H */
