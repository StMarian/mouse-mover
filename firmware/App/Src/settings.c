#include "settings.h"

#include <string.h>

#include "bsp_flash.h"

#define SETTINGS_MAGIC   0x4D4Du // "MM"
#define SETTINGS_VERSION 1u      // bump when the record layout changes

// Stored as-is in flash. All fields are 16 bits because the F1 programs flash a halfword at a time.
typedef struct
{
    uint16_t magic;
    uint16_t version;
    uint16_t period_index;
    uint16_t check; // 0xFFFF XOR of the fields above
} settings_record_t;

_Static_assert(sizeof(settings_record_t) == 8u, "record must have no padding");

static uint16_t record_check(const settings_record_t *record)
{
    return (uint16_t)(0xFFFFu ^ record->magic ^ record->version ^ record->period_index);
}

static bool read_record(settings_record_t *record)
{
    if (bsp_flash_settings_size() < sizeof(*record))
    {
        return false;
    }

    memcpy(record, bsp_flash_settings_data(), sizeof(*record));

    return (record->magic == SETTINGS_MAGIC) && (record->version == SETTINGS_VERSION) &&
           (record->check == record_check(record));
}

bool settings_load(settings_t *settings)
{
    settings_record_t record;

    if (!read_record(&record) || (record.period_index > UINT8_MAX))
    {
        return false;
    }

    settings->period_index = (uint8_t)record.period_index;
    return true;
}

bool settings_save(const settings_t *settings)
{
    settings_record_t record = {
        .magic = SETTINGS_MAGIC,
        .version = SETTINGS_VERSION,
        .period_index = settings->period_index,
        .check = 0u,
    };
    record.check = record_check(&record);

    settings_record_t stored;
    if (read_record(&stored) && (memcmp(&stored, &record, sizeof(record)) == 0))
    {
        // Nothing changed
        return true;
    }

    return bsp_flash_settings_write(&record, sizeof(record));
}
