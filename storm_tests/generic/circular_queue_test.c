// Abstract: Tests for circular_queue.c
// Authors: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include "../test_helper.h"

#include <stdlib.h>
#include <string.h>

#include <storm/generic/circular_queue.h>
#include <storm/generic/defines.h>

#define QUEUE_SIZE      64 * KB
#define MESSAGE_SIZE    100

typedef struct
{
    void *message;
    circular_queue_type *queue;
} circular_queue_test_type;

int circular_queue_test_setup(void **state)
{
    circular_queue_test_type *circular_queue_test = malloc(sizeof(circular_queue_test_type));

    circular_queue_test->message = malloc(MESSAGE_SIZE);
    memset(circular_queue_test->message, 42, MESSAGE_SIZE);

    circular_queue_test->queue = malloc(QUEUE_SIZE);
    circular_queue_initialize(circular_queue_test->queue, QUEUE_SIZE);

    *state = circular_queue_test;

    return 0;
}

void test_circular_queue_enqueue_one(void **state)
{
    // Arrange
    circular_queue_test_type *circular_queue_test = (circular_queue_test_type *) *state;
    circular_queue_type *queue = circular_queue_test->queue;
    void *message = circular_queue_test->message;

    // Act
    //
    // Deliberately publish only half the message, to avoid all tests being too similar.
    bool result = circular_queue_enqueue(queue, message, 50);

    // Assert
    //
    // The queue holds a size pointer right before each message, which is why the head is now
    // expected to be 50 + sizeof(int) bytes inside the array.
    assert_true(result);
    assert_ptr_equal(queue->head, queue->data + 50 + sizeof(int));
}

void test_circular_queue_enqueue_until_full(void **state)
{
    // Arrange
    circular_queue_test_type *circular_queue_test = (circular_queue_test_type *) *state;
    circular_queue_type *queue = circular_queue_test->queue;
    void *message = circular_queue_test->message;

    // Act & Assert
    //
    // The circular queue is 64 KiB, so in total there should is room for (65536 - 16) / 104 = 630
    // messages of this size.
    for (int i = 0; i < 630; i++)
    {
        assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
    }

    // Since there are only ~20 bytes left in the buffer at this stage (36 - the size of the fields
    // in the circular_queue_type), enqueueing this message is expected to fail.
    assert_false(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
}

void test_circular_queue_enqueue_before_tail(void **state)
{
    // Arrange
    circular_queue_test_type *circular_queue_test = (circular_queue_test_type *) *state;
    circular_queue_type *queue = circular_queue_test->queue;
    void *message = circular_queue_test->message;

    // Act & Assert
    //
    // Like in the test above, this should succeed.
    for (int i = 0; i < 630; i++)
    {
        assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
    }

    void *dequeued_message = circular_queue_dequeue(queue);
    assert_ptr_not_equal(dequeued_message, NULL);

    // There should now be room before the tail, like this:
    //
    //   +-----------------+------+------+------+------+-----+--------+
    //   | free tail space | msg2 | msg3 | msg4 | msg5 | ... | msg630 |
    //   +-----------------+------+------+------+------+-----+--------+
    //   ^                 ^
    // head              tail

    assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
}

void test_circular_queue_enqueue_between_head_and_tail(void **state)
{
    // Arrange
    circular_queue_test_type *circular_queue_test = (circular_queue_test_type *) *state;
    circular_queue_type *queue = circular_queue_test->queue;
    void *message = circular_queue_test->message;

    // Act & Assert
    //
    // Like in the test above, this should succeed.
    for (int i = 0; i < 630; i++)
    {
        assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
    }

    void *dequeued_message = circular_queue_dequeue(queue);
    assert_ptr_not_equal(dequeued_message, NULL);

    dequeued_message = circular_queue_dequeue(queue);
    assert_ptr_not_equal(dequeued_message, NULL);

    dequeued_message = circular_queue_dequeue(queue);
    assert_ptr_not_equal(dequeued_message, NULL);

    // There should now be room before the tail, like this:
    //
    //   +-----------------+------+------+------+------+-----+--------+
    //   | free tail space | msg4 | msg5 | msg6 | msg7 | ... | msg630 |
    //   +-----------------+------+------+------+------+-----+--------+
    //   ^                 ^
    // head              tail
    //
    // The point of this test is to ensure that the free tail space can be used up in full; the
    // test_circular_queue_enqueue_before_tail() only attempts to enqueue a single message into
    // the free tail space while this test enqueues multiple ones. So this can be considered a
    // superset of test_circular_queue_enqueue_before_tail()

    assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
    assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
    assert_true(circular_queue_enqueue(queue, message, MESSAGE_SIZE));

    // The queue is now expected to be full.
    assert_false(circular_queue_enqueue(queue, message, MESSAGE_SIZE));
}
