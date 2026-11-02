#ifndef SEG7_H
#define SEG7_H

/*
 * Two-digit multiplexed 7-segment display.
 *
 * The main loop publishes what to show; a periodic timer interrupt calls
 * seg7_refresh_isr(), which lights one digit per call.
 */

#include <stdbool.h>
#include <stdint.h>

// Called from main-loop
void seg7_show_number(uint16_t value);
void seg7_show_dashes(void);
void seg7_set_blinking(bool blinking);

// Called from ISR
void seg7_refresh_isr(void);

#endif /* SEG7_H */
