#include <string.h>

#include "unity.h"

#include "fake_bsp.h"
#include "settings.h"

/*
 * Mirror of the record layout in settings.c, so tests can plant records in the
 * fake flash byte for byte - including broken ones.
 */
typedef struct
{
    uint16_t magic;
    uint16_t version;
    uint16_t period_index;
    uint16_t check;
} record_t;

#define MAGIC   0x4D4Du
#define VERSION 1u

static uint16_t valid_check(uint16_t magic, uint16_t version, uint16_t period_index)
{
    return (uint16_t)(0xFFFFu ^ magic ^ version ^ period_index);
}

static void plant_record(uint16_t magic, uint16_t version, uint16_t period_index, uint16_t check)
{
    const record_t record = {magic, version, period_index, check};
    memcpy(g_fake_bsp.flash, &record, sizeof(record));
}

static void plant_valid_record(uint16_t period_index)
{
    plant_record(MAGIC, VERSION, period_index, valid_check(MAGIC, VERSION, period_index));
}

void setUp(void)
{
    fake_bsp_reset();
}

void tearDown(void)
{
}

// --- Loading -------------------------------------------------------------------

static void test_an_erased_page_holds_no_settings(void)
{
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

static void test_an_all_zero_page_holds_no_settings(void)
{
    // A plain XOR check would accept all zeros; starting the check from 0xFFFF rejects it.
    memset(g_fake_bsp.flash, 0x00, sizeof(g_fake_bsp.flash));
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

static void test_a_valid_record_loads(void)
{
    plant_valid_record(4u);
    settings_t settings;

    TEST_ASSERT_TRUE(settings_load(&settings));
    TEST_ASSERT_EQUAL_UINT8(4u, settings.period_index);
}

static void test_a_failed_load_leaves_the_output_untouched(void)
{
    settings_t settings = {.period_index = 42u};

    TEST_ASSERT_FALSE(settings_load(&settings));
    TEST_ASSERT_EQUAL_UINT8(42u, settings.period_index);
}

static void test_a_record_with_the_wrong_magic_is_rejected(void)
{
    plant_record(0x1234u, VERSION, 4u, valid_check(0x1234u, VERSION, 4u));
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

static void test_a_record_from_another_layout_version_is_rejected(void)
{
    plant_record(MAGIC, VERSION + 1u, 4u, valid_check(MAGIC, VERSION + 1u, 4u));
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

static void test_a_half_written_record_is_rejected(void)
{
    // Power lost before the check value was programmed: it still reads as erased.
    plant_record(MAGIC, VERSION, 4u, 0xFFFFu);
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

static void test_a_record_with_a_corrupted_value_is_rejected(void)
{
    // 4 -> 5 is still a valid period index, so only the check value can catch it.
    plant_record(MAGIC, VERSION, 5u, valid_check(MAGIC, VERSION, 4u));
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

static void test_an_index_too_large_for_the_settings_type_is_rejected(void)
{
    plant_valid_record(300u);
    settings_t settings;

    TEST_ASSERT_FALSE(settings_load(&settings));
}

// --- Saving --------------------------------------------------------------------

static void test_saved_settings_load_back(void)
{
    const settings_t saved = {.period_index = 6u};
    settings_t loaded;

    TEST_ASSERT_TRUE(settings_save(&saved));
    TEST_ASSERT_TRUE(settings_load(&loaded));
    TEST_ASSERT_EQUAL_UINT8(6u, loaded.period_index);
}

static void test_saving_a_new_value_writes_the_flash_once(void)
{
    const settings_t settings = {.period_index = 1u};

    TEST_ASSERT_TRUE(settings_save(&settings));
    TEST_ASSERT_EQUAL_UINT32(1u, g_fake_bsp.flash_writes);
}

static void test_saving_an_unchanged_value_does_not_write_the_flash(void)
{
    const settings_t settings = {.period_index = 1u};

    TEST_ASSERT_TRUE(settings_save(&settings));
    TEST_ASSERT_TRUE(settings_save(&settings));
    TEST_ASSERT_EQUAL_UINT32(1u, g_fake_bsp.flash_writes);
}

static void test_a_failed_flash_write_is_reported(void)
{
    g_fake_bsp.flash_write_fails = true;
    const settings_t settings = {.period_index = 1u};

    TEST_ASSERT_FALSE(settings_save(&settings));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_an_erased_page_holds_no_settings);
    RUN_TEST(test_an_all_zero_page_holds_no_settings);
    RUN_TEST(test_a_valid_record_loads);
    RUN_TEST(test_a_failed_load_leaves_the_output_untouched);
    RUN_TEST(test_a_record_with_the_wrong_magic_is_rejected);
    RUN_TEST(test_a_record_from_another_layout_version_is_rejected);
    RUN_TEST(test_a_half_written_record_is_rejected);
    RUN_TEST(test_a_record_with_a_corrupted_value_is_rejected);
    RUN_TEST(test_an_index_too_large_for_the_settings_type_is_rejected);
    RUN_TEST(test_saved_settings_load_back);
    RUN_TEST(test_saving_a_new_value_writes_the_flash_once);
    RUN_TEST(test_saving_an_unchanged_value_does_not_write_the_flash);
    RUN_TEST(test_a_failed_flash_write_is_reported);
    return UNITY_END();
}
