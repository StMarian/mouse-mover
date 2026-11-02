#ifndef SETTINGS_H
#define SETTINGS_H

/*
 * User settings that survive power-off, stored in the flash settings page.
 */

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t period_index; // index into COUNTDOWN_PERIOD_OPTIONS
} settings_t;

bool settings_load(settings_t *settings);
bool settings_save(const settings_t *settings);

#endif /* SETTINGS_H */
