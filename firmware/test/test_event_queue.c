#include "unity.h"

#include "event_queue.h"

#define QUEUE_CAPACITY 7u // 8 slots, one always kept empty

static void drain(void)
{
    app_event_t event;
    while (event_queue_pop(&event))
    {
    }
}

void setUp(void)
{
    drain(); // the queue is a module-level singleton, so start each test empty
}

void tearDown(void)
{
}

static void test_pop_from_an_empty_queue_returns_false(void)
{
    app_event_t event;

    TEST_ASSERT_FALSE(event_queue_pop(&event));
}

static void test_events_come_out_in_the_order_they_went_in(void)
{
    // There is only one event type today, so raw values stand in for distinct events.
    for (int i = 0; i < 3; i++)
    {
        TEST_ASSERT_TRUE(event_queue_push((app_event_t)i));
    }

    for (int i = 0; i < 3; i++)
    {
        app_event_t event;
        TEST_ASSERT_TRUE(event_queue_pop(&event));
        TEST_ASSERT_EQUAL_INT(i, (int)event);
    }

    app_event_t event;
    TEST_ASSERT_FALSE(event_queue_pop(&event));
}

static void test_a_full_queue_rejects_new_events_and_counts_them(void)
{
    const uint32_t dropped_before = event_queue_dropped();

    for (uint32_t i = 0u; i < QUEUE_CAPACITY; i++)
    {
        TEST_ASSERT_TRUE(event_queue_push(APP_EVENT_COUNTDOWN_TICK));
    }

    TEST_ASSERT_FALSE(event_queue_push(APP_EVENT_COUNTDOWN_TICK));
    TEST_ASSERT_EQUAL_UINT32(dropped_before + 1u, event_queue_dropped());
}

static void test_the_queue_keeps_working_after_many_wrap_arounds(void)
{
    for (int i = 0; i < 1000; i++)
    {
        app_event_t event;

        TEST_ASSERT_TRUE(event_queue_push((app_event_t)(i % 200)));
        TEST_ASSERT_TRUE(event_queue_pop(&event));
        TEST_ASSERT_EQUAL_INT(i % 200, (int)event);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_pop_from_an_empty_queue_returns_false);
    RUN_TEST(test_events_come_out_in_the_order_they_went_in);
    RUN_TEST(test_a_full_queue_rejects_new_events_and_counts_them);
    RUN_TEST(test_the_queue_keeps_working_after_many_wrap_arounds);
    return UNITY_END();
}
