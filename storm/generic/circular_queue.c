// Abstract: Circular queue implementation
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include <storm/generic/circular_queue.h>
#include <storm/generic/defines.h>
#include <storm/generic/memory.h>

void circular_queue_initialize(circular_queue_type *queue, int queue_size)
{
    queue->head = queue->data;
    queue->tail = NULL;
    queue->size = queue_size - sizeof(circular_queue_type);
    queue->data_end = queue->data + queue->size;
}

bool circular_queue_enqueue(circular_queue_type *queue, void *message, int length)
{
    int total_length = length + sizeof(int);

    if (queue->head == queue->tail)
    {
        // The head has reached the tail. There is absolutely no free space in this queue. Indicate
        // this to the caller.
        return FALSE;
    }

    if (queue->head > queue->tail)
    {
        //
        // The queue looks roughly like this:
        //
        // +--------------------+------+------+------+-------------------------+
        // | free tail space    | msg1 | msg2 | msg3 | free head space         |
        // +--------------------+------+------+------+-------------------------+
        //                      ^                    ^
        //                    tail                  head
        //
        // Note: this branch also handles the tail == NULL case. Remember that NULL is just a
        // convenient alias for 0 in C pointer arithmetics.
        //
        int free_space_after_head = queue->data_end -
                                    queue->head;

        if (free_space_after_head >= total_length)
        {
            if (queue->tail == NULL)
            {
                // This is the first message in the queue; the tail pointer must be initialized.
                queue->tail = queue->head;
            }

            *(int *)queue->head = length;
            memory_copy(queue->head + sizeof(int), message, length);

            queue->head += total_length;

            if (queue->head >= queue->data_end)
            {
                // All space at the end of the queue has been used up; move the pointer to the
                // free tail space.
                queue->head = queue->data;
            }

            return TRUE;
        }
        else
        {
            if (queue->tail == NULL)
            {
                // There is no tail space at all, since there are no messages in the queue. This
                // likely means that the message is too large to fit in the head space.
                return FALSE;
            }

            int free_space_before_tail = queue->tail -
                                         queue->data;

            if (free_space_before_tail >= total_length)
            {
                *(int *)queue->data = length;
                memory_copy(queue->data + sizeof(int), message, length);

                queue->head = queue->data + total_length;

                // No need to check if queue->tail is NULL at this stage since it's already been
                // asserted a few lines above. This message is never the first message being
                // enqueued in this queue.
                return TRUE;
            }
            else
            {
                // Not enough free tail space.
                return FALSE;
            }
        }
    }
    else // queue->tail > queue->head
    {
        //
        // The queue looks roughly like this:
        //
        // +------+ -----+-----------------+------+------+------+
        // | msg4 | msg5 | free tail space | msg1 | msg2 | msg3 |
        // +------+------------------------+------+------+------+
        //               ^                 ^
        //             head              tail
        //

        int free_space_between_head_and_tail = queue->tail - queue->head;

        if (free_space_between_head_and_tail >= total_length)
        {
            *(int *)queue->data = length;
            memory_copy(queue->data + sizeof(int), message, length);

            queue->head += total_length;

            // No need to check if queue->tail is NULL at this stage since it's already been
            // asserted a few lines above. This message is never the first message being
            // enqueued in this queue.
            return TRUE;
        }
        else
        {
            // Not enough free tail space.
            return FALSE;
        }

        return FALSE;
    }
}

void *circular_queue_dequeue(circular_queue_type *queue)
{
    if (queue->tail == NULL)
    {
        // There are no messages in this queue at the moment.
        return NULL;
    }

    int length = *(int *)queue->tail;
    void *result = queue->tail + sizeof(int);

    if (length > queue->size)
    {
        // The message in the queue is corrupted; the length of an individual message can never
        // exceed the whole queue size. Indicate this to the caller.
        return NULL;
    }

    queue->tail += length + sizeof(int);

    if (queue->tail >= queue->data_end)
    {
        // We have reached the end of the buffer; move the pointer back to the very start of it.
        queue->tail = queue->data;
    }

    if (queue->tail == queue->head)
    {
        // We have reached the head => there are no more messages waiting in the queue. Update the
        // tail pointer accordingly.
        queue->tail = NULL;

        // We can also reset the head to the beginning of the buffer, to simplify the
        // enqueueing logic.
        queue->head = queue->data;
    }

    return result;
}

int circular_queue_peek(circular_queue_type *queue)
{
    if (queue->tail == NULL)
    {
        // There are no messages in this queue at the moment.
        return -1;
    }

    int length = *(int *)queue->tail;

    return length;
}

int circular_queue_get_maximum_enqueue_size(circular_queue_type *queue)
{
    if (queue->head == queue->tail)
    {
        // The head has reached the tail. There is absolutely no free space in this queue.
        return 0;
    }

    if (queue->head > queue->tail)
    {
        //
        // The queue looks roughly like this:
        //
        // +--------------------+------+------+------+-------------------------+
        // | free tail space    | msg1 | msg2 | msg3 | free head space         |
        // +--------------------+------+------+------+-------------------------+
        //                      ^                    ^
        //                    tail                  head
        //
        // Note: this branch also handles the tail == NULL case. Remember that NULL is just a
        // convenient alias for 0 in C pointer arithmetics.
        //
        int free_space_after_head = queue->data_end - queue->head;
        int free_space_before_tail = 0;

        if (queue->tail != NULL)
        {
            // There is space before the tail, calculate its size.
            free_space_before_tail = queue->tail - queue->data;
        }

        return MAX_OF_TWO(free_space_after_head, free_space_before_tail);
    }
    else // queue->tail > queue->head
    {
        //
        // The queue looks roughly like this:
        //
        // +------+ -----+-----------------+------+------+------+
        // | msg4 | msg5 | free tail space | msg1 | msg2 | msg3 |
        // +------+------------------------+------+------+------+
        //               ^                 ^
        //             head              tail
        //

        int free_space_between_head_and_tail = queue->tail - queue->head;

        return free_space_between_head_and_tail;
    }
}
