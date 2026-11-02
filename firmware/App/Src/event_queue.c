#include "event_queue.h"

/*
 * Single-producer / single-consumer ring buffer.
 *
 * One slot always stays empty, so head == tail means "empty" and
 * head + 1 == tail means "full".
 */

#define QUEUE_SIZE 8u // must be a power of two
#define QUEUE_MASK (QUEUE_SIZE - 1u)

_Static_assert((QUEUE_SIZE & QUEUE_MASK) == 0u, "QUEUE_SIZE must be a power of two");
_Static_assert(QUEUE_SIZE <= 256u, "indices are uint8_t");

static volatile uint8_t s_buffer[QUEUE_SIZE];
static volatile uint8_t s_head = 0u; // next slot to write; producer only
static volatile uint8_t s_tail = 0u; // next slot to read; consumer only
static volatile uint32_t s_dropped = 0u;

bool event_queue_push(app_event_t event)
{
    const uint8_t head = s_head;
    const uint8_t next = (uint8_t)((head + 1u) & QUEUE_MASK);

    if (next == s_tail)
    {
        s_dropped++;
        return false;
    }

    s_buffer[head] = (uint8_t)event;
    s_head = next;
    return true;
}

bool event_queue_pop(app_event_t *event)
{
    const uint8_t tail = s_tail;

    if (tail == s_head)
    {
        return false;
    }

    *event = (app_event_t)s_buffer[tail];
    s_tail = (uint8_t)((tail + 1u) & QUEUE_MASK);
    return true;
}

uint32_t event_queue_dropped(void)
{
    return s_dropped;
}
