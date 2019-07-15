// Abstract: Circular queue implementation
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include <storm/generic/types.h>

#pragma once

//
// Data types
//

//
// The implementation talks about "message" as a term for an individual element of the queue, as
// a convention.
//
// The approximate format of the circular queue is as below:
//
// +--------------------+------+------+------+-------------------------+
// | free tail space    | msg1 | msg2 | msg3 | free head space         |
// +--------------------+------+------+------+-------------------------+
//                      ^                    ^
//                    tail                  head
//
// For simplicity, a message will never be split between the tail & head
// space; the head space is first attempted, but if not enough space
// is available the, the enqueueing code tries to
typedef struct
{
    // This will always point to the next element that should be dequeued. When the queue is empty,
    // this will be NULL.
    uint8_t *tail;

    // The will always point to the next location where an element should be enqueued. Note that
    // because the queue is circular, this could be both before or after `tail`
    uint8_t *head;

    // The size of the `data` buffer (in bytes)
    int size;

    // A pointer to the byte right after the end of data[] array, for easy comparison and
    // debugging purposes. Note that this byte can NOT be written to, since it's one byte past
    // the allocated buffer.
    uint8_t *data_end;

    // The buffer holds the messages which are currently placed in the queue.
    uint8_t data[0];
} circular_queue_type;

//
// Function prototypes
//
// Initializes a circular queue. Note that `queue_size` given here is the total size of the queue,
// include the header in `circular_queue_type`; it does not refer to the size of the data buffer
// which is slightly smaller.
extern void circular_queue_initialize(circular_queue_type *queue, int queue_size);

// Enqueues a message
extern bool circular_queue_enqueue(circular_queue_type *queue, void *message, int length);

// Dequeues a message. Note that the message is not copied; a pointer is returned which points
// into the buffer owned by the circular queue. It will not necessary be valid after the next
// operation on the queue. It is the callers responsibility to copy the message to a privately
// owned buffer if necessary.
extern void *circular_queue_dequeue(circular_queue_type *queue);

// Checks if there are any messages in the queue without modifying the state of the queue. Returns
// the size of the next message to be dequeued (which can be 0 for empty messages), or -1 if there
// are no messages available in the queue.
extern int circular_queue_peek(circular_queue_type *queue);

// Gets the size of the largest message that would fit in the queue currently.
extern int circular_queue_get_maximum_enqueue_size(circular_queue_type *queue);
