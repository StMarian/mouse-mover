#ifndef BSP_H
#define BSP_H

/*
 * Board support: the only code that knows about pins, the HAL and the MCU.
 */

#include <stdbool.h>
#include <stdint.h>

void bsp_init(void);

uint32_t bsp_millis(void);

// Sleep until the next interrupt (SysTick wakes the core at least every 1 ms).
void bsp_idle(void);

void bsp_watchdog_kick(void);

bool bsp_button_is_pressed(void);

void bsp_motor_set(bool on);

// Light one digit of the display. digit 0 = tens, 1 = units; bit n of segments = segment a..g.
void bsp_display_show(uint8_t digit, uint8_t segments);

void bsp_display_off(void);

// Motor and display off. Register writes only, so it is safe to call from fault handlers.
void bsp_safe_state(void);

#endif /* BSP_H */
