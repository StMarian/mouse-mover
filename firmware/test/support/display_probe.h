#ifndef DISPLAY_PROBE_H
#define DISPLAY_PROBE_H

/*
 * Reads the 7-segment display the way a person would: runs the refresh
 * interrupt for both digits, then decodes the lit segments on the fake board.
 */

#include <stdbool.h>

#define DISPLAY_DASHES     (-1) // "--"
#define DISPLAY_BLANK      (-2) // all segments off (blink phase)
#define DISPLAY_UNREADABLE (-3) // a pattern that is not a digit

// The number on the display, or one of the DISPLAY_* values above.
int display_read(void);

// Same, but read during the "on" half of the blink cycle.
int display_read_ignoring_blink(void);

// True if the display is blinking (it is off during the "off" half of the cycle).
bool display_is_blinking(void);

#endif /* DISPLAY_PROBE_H */
