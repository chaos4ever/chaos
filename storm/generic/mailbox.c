// Abstract: Mailbox functions for the LPC (local process communication).
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Anders Öhrt <doa@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development

// Define this as TRUE if you are debugging this module.
#define DEBUG           TRUE

// Extra verbose debugging. This will make the system pretty unusable, so be warned.
#define VERBOSE_DEBUG   FALSE

#include <storm/state.h>
#include <storm/generic/mailbox.h>
#include <storm/generic/circular_queue.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/return_values.h>
#include <storm/generic/thread.h>
#include <storm/current-arch/tss.h>

static mailbox_id_type mailbox_initialize(mailbox_type *new_mailbox, int size,
                                          process_id_type user_process_id, cluster_id_type user_cluster_id,
                                          thread_id_type user_thread_id);

// The maximum size of an individual message. The size here is optimized for 64 KiB mailboxes,
// since there is a slight overhead that maxes the maximum message size be slightly lower than
// the actual mailbox size.
static int maximum_message_size = 63 * KB;

// static mutex_kernel_type mailbox_mutex = MUTEX_UNLOCKED;
// static mutex_kernel_type hash_mutex = MUTEX_UNLOCKED;
// static mutex_kernel_type free_id_mutex = MUTEX_UNLOCKED;

// The mailbox hash is an array of pointers.
static mailbox_type **mailbox_hash_table;
static mailbox_id_type free_mailbox_id = 1;

// A static, pre-allocated structure used when sending a message. The reason this exists is to avoid
// heavy dynamic allocation during each mailbox_send() call. This is protected via the
// `tss_tree_mutex` mutex for now.
static message_type *message;

// Gets the hash value for the given mailbox ID.
static int hash(mailbox_id_type mailbox_id)
{
    return (mailbox_id % limit_mailbox_hash_entries);
}

void mailbox_init(void)
{
    mailbox_hash_table = (mailbox_type **) memory_global_allocate(sizeof(mailbox_type *) * limit_mailbox_hash_entries);

    DEBUG_MESSAGE(VERBOSE_DEBUG, "mailbox_hash_table = %x, size = %u",
                  mailbox_hash_table, sizeof (mailbox_type *) *
                  limit_mailbox_hash_entries);

    // Initially set all pointers to NULL. This isn't entirely nice, but doing this as a for loop wouldn't even be
    // close to cool.
    memory_set_uint8_t((uint8_t *) mailbox_hash_table, 0, limit_mailbox_hash_entries * sizeof (mailbox_type *));

    message = (message_type *) memory_global_allocate(sizeof(message_type) + maximum_message_size);
}

// Looks the given mailbox ID up in the hash table.
static mailbox_type *mailbox_find(mailbox_id_type mailbox_id)
{
    mailbox_type *node;

    node = mailbox_hash_table[hash(mailbox_id)];

    DEBUG_MESSAGE(VERBOSE_DEBUG, "node = %x", node);

    while (node != NULL && node->id != mailbox_id)
    {
        node = (mailbox_type *) node->next;
        DEBUG_MESSAGE(VERBOSE_DEBUG, "node = %x", node);
    }

    return node;
}

// Get the first available mailbox id.
static mailbox_id_type mailbox_get_free_id(void)
{
    mailbox_id_type id;

    id = free_mailbox_id;
    free_mailbox_id++;

    return id;
}

// Link in a new mailbox in the mailbox hash table. Returns TRUE on successful linking of the mailbox, or FALSE otherwise.
static bool mailbox_link(mailbox_type *new_mailbox)
{
    int index;

    index = hash(new_mailbox->id);

    new_mailbox->next = (struct mailbox_type *) mailbox_hash_table[index];
    mailbox_hash_table[index] = new_mailbox;

    return TRUE;
}

// Unlink a mailbox with a certain id. Returns TRUE on success, and FALSE otherwise.
static bool mailbox_unlink(mailbox_id_type mailbox_id)
{
    int index;
    mailbox_type *mailbox, *previous_mailbox;

    index = hash(mailbox_id);

    mailbox = mailbox_hash_table[index];

    if (mailbox->id == mailbox_id)
    {
        mailbox_hash_table[index] = (mailbox_type *) mailbox->next;
        memory_global_deallocate(mailbox);

        return TRUE;
    }
    else
    {
        while (mailbox != NULL && mailbox->id != mailbox_id)
        {
            previous_mailbox = mailbox;
            mailbox = (mailbox_type *) mailbox->next;
        }
        if (mailbox == NULL)
        {
            return FALSE;
        }
        else
        {
            previous_mailbox->next = mailbox->next;
            memory_global_deallocate(mailbox);

            return TRUE;
        }
    }
}

// Create a new mailbox without locking the dispatcher (presuming it has already been locked). This function can only
// be called from within the kernel.
return_type mailbox_create_kernel(mailbox_id_type *mailbox_id, unsigned int size, process_id_type user_process_id,
    cluster_id_type user_cluster_id, thread_id_type user_thread_id)
{
    mailbox_type *new_mailbox;

    DEBUG_MESSAGE(VERBOSE_DEBUG, "Called");

    new_mailbox = (mailbox_type *) memory_global_allocate(sizeof (mailbox_type));
    *mailbox_id = mailbox_initialize(new_mailbox, size, user_process_id, user_cluster_id, user_thread_id);

    mailbox_link(new_mailbox);

    return STORM_RETURN_SUCCESS;
}

// Create a new mailbox.
return_type mailbox_create(mailbox_id_type *mailbox_id, unsigned int size, process_id_type user_process_id,
    cluster_id_type user_cluster_id, thread_id_type user_thread_id)
{
    return_type return_value;

    mutex_kernel_wait(&tss_tree_mutex);
    return_value = mailbox_create_kernel(mailbox_id, size, user_process_id, user_cluster_id, user_thread_id);
    mutex_kernel_signal(&tss_tree_mutex);

    return return_value;
}

// Destroy a mailbox, using some common explosive.
return_type mailbox_destroy(mailbox_id_type mailbox_id)
{
    mailbox_type *mailbox;

    mutex_kernel_wait(&tss_tree_mutex);

    mailbox = mailbox_find(mailbox_id);

    //  debug_print ("%s: destroying %u\n", __FUNCTION__, mailbox_id);

    if (mailbox == NULL)
    {
        mutex_kernel_signal(&tss_tree_mutex);

        return STORM_RETURN_MAILBOX_UNAVAILABLE;
    }

    if (current_process_id == mailbox->owner_process_id &&
        current_cluster_id == mailbox->owner_cluster_id &&
        current_thread_id == mailbox->owner_thread_id)
    {
        // We are allowed to destroy this mailbox.
        mailbox_unlink(mailbox_id);

        //    tree_dump (root);

        //    debug_print ("root: %x\n", root);

        // No need to close the mailbox, since it is destroyed.
        mutex_kernel_signal(&tss_tree_mutex);

        return STORM_RETURN_SUCCESS;
    }
    else
    {
        debug_print("Segmentation fault.\n");
        mutex_kernel_signal(&tss_tree_mutex);

        return STORM_RETURN_ACCESS_DENIED;
    }
}

// Delete all queued messages in the mailbox.
// FIXME: Write this code.
return_type mailbox_flush(mailbox_id_type mailbox_id)
{
    mailbox_id = mailbox_id;

    return STORM_RETURN_SUCCESS;
}

// Post a message in a mailbox.
// FIXME: Is the dude allowed to send to OUR mailbox?!
return_type mailbox_send(mailbox_id_type mailbox_id, message_parameter_type *message_parameter)
{
    // Perform some sanity checking on the input parameters.
    if (message_parameter == NULL || (message_parameter->data == NULL &&
                                      message_parameter->length > 0))
    {
        DEBUG_SDB(DEBUG, "Invalid argument.");

        return STORM_RETURN_INVALID_ARGUMENT;
    }

    mutex_kernel_wait(&tss_tree_mutex);

    DEBUG_MESSAGE(VERBOSE_DEBUG,
                  "Called (message_parameter = %p, data = %p, protocol = %u, class = %u, id = %u)",
                  message_parameter, message_parameter->data,
                  message_parameter->protocol, message_parameter->message_class,
                  mailbox_id);

    mailbox_type *mailbox = mailbox_find(mailbox_id);

    if (mailbox == NULL)
    {
        mutex_kernel_signal(&tss_tree_mutex);
        DEBUG_SDB(DEBUG, "mailbox %u could not be found", mailbox_id);

        return STORM_RETURN_MAILBOX_UNAVAILABLE;
    }

    int full_length = sizeof(message_type) + message_parameter->length;

    // If the message won't ever fit into this mailbox, fail.
    if (full_length > mailbox->queue->size)
    {
        mutex_kernel_signal(&tss_tree_mutex);
        DEBUG_MESSAGE(DEBUG,
                      "Message was larger than the mailbox! (%u > %u)",
                      full_length, mailbox->queue->size);

        return STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE;
    }

    // FIXME: This check is silly and incomplete. Publishing the message might still fail, since
    // maximum_message_size is a constant instead of determined from the mailbox size. This will
    // only affect mailboxes created with a size different than the default (64 KiB), e.g.
    // service mailboxes.
    if (full_length > maximum_message_size)
    {
        mutex_kernel_signal(&tss_tree_mutex);
        DEBUG_MESSAGE(DEBUG,
                      "Message was larger than the maximum message size (%u > %u)",
                      full_length, maximum_message_size);

        return STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE;
    }

    message->sender_process_id = current_process_id;
    message->sender_cluster_id = current_cluster_id;
    message->sender_thread_id = current_thread_id;

    message->protocol = message_parameter->protocol;
    message->class = message_parameter->message_class;

    message->length = message_parameter->length;
    memory_copy(message->data, message_parameter->data, message_parameter->length);

    while (TRUE)
    {
        if (circular_queue_enqueue(mailbox->queue, message, sizeof(message_type) + message_parameter->length))
        {
            // The message was successfully enqueued. If another thread is blocked on this mailbox,
            // it should be unblocked now.
            if (mailbox->reader_blocked)
            {
                DEBUG_MESSAGE(VERBOSE_DEBUG, "Unblocking...");
                thread_unblock_mailbox_receive(mailbox_id);
                DEBUG_MESSAGE(VERBOSE_DEBUG, "Done.");
            }

            mutex_kernel_signal(&tss_tree_mutex);

            return STORM_RETURN_SUCCESS;
        }
        else
        {
            // The mailbox was probably full. Block or return, depending on what the caller
            // asked for.
            if (message_parameter->block)
            {
                // Block until receiver reads messages so that there is room for us.
                current_tss->mailbox_id = mailbox_id;
                current_tss->mutex_time = timeslice;
                mailbox->blocked_size = message_parameter->length + sizeof(message_parameter_type);
                current_tss->state = STATE_MAILBOX_SEND;
                mutex_kernel_signal(&tss_tree_mutex);
                dispatch_next();
            }
            else
            {
                mutex_kernel_signal(&tss_tree_mutex);
                DEBUG_SDB(DEBUG, "Mailbox was full");

                return STORM_RETURN_MAILBOX_FULL;
            }
        }
    }
}

// Receive a message from the mailbox.
return_type mailbox_receive(mailbox_id_type mailbox_id, message_parameter_type *message_parameter)
{
    if (message_parameter == NULL)
    {
        DEBUG_SDB(DEBUG, "message_parameter == NULL");

        return STORM_RETURN_INVALID_ARGUMENT;
    }

    mutex_kernel_wait(&tss_tree_mutex);
    DEBUG_MESSAGE(VERBOSE_DEBUG, "Called (id %u).", mailbox_id);
    mailbox_type *mailbox = mailbox_find(mailbox_id);

    // When we get here, we are allowed to access the mailbox.
    if (mailbox == NULL)
    {
        mutex_kernel_signal(&tss_tree_mutex);
        DEBUG_HALT("Mailbox was NULL");

        return STORM_RETURN_MAILBOX_UNAVAILABLE;
    }

    // We would *like* to add current_thread_id == mailbox->owner_thread_id)
    // to the check below, but the IPC library currently passes the reply
    // mailbox ID from the service client to the process that is
    // listening for connections on the service, making that check
    // impossible for now.
    if (!(current_process_id == mailbox->owner_process_id &&
          current_cluster_id == mailbox->owner_cluster_id))
    {
        // We don't have read-access to this mailbox, since we are not the owner.
        DEBUG_MESSAGE(DEBUG, "Access denied for process/thread %u/%u (should have been %u/%u) mailbox ID %u",
                      current_process_id, current_thread_id,
                      mailbox->owner_process_id, mailbox->owner_thread_id,
                      mailbox_id);
        mutex_kernel_signal(&tss_tree_mutex);

        while (TRUE) ;

        return STORM_RETURN_ACCESS_DENIED;
    }

    int next_message_size = circular_queue_peek(mailbox->queue);

    // If the mailbox is empty, block or return.
    if (next_message_size == -1)
    {
        if (message_parameter->block)
        {
            // This _should_ never happen, but we've seen it in cases where multiple threads are (incorrectly) reading from
            // the same mailbox => chaos ensues.
            assert(!mailbox->reader_blocked, "Attempting to block on mailbox_receive for mailbox ID %d, but mailbox is already blocked by thread %d", mailbox_id, mailbox->reader_thread_id);

            // Block ourselves until the mailbox gets populated.
            mailbox->reader_blocked = TRUE;
            mailbox->reader_thread_id = current_tss->thread_id;

            DEBUG_MESSAGE(VERBOSE_DEBUG, "Blocking ourselves.");

            // Modify this task's entry in the TSS structure.
            current_tss->state = STATE_MAILBOX_RECEIVE;
            current_tss->mailbox_id = mailbox_id;
            current_tss->mutex_time = timeslice;
            mutex_kernel_signal(&tss_tree_mutex);

            // Pass on control to the task switcher, since we are now
            // blocked on MAILBOX_RECEIVE.
            dispatch_next();

            mutex_kernel_wait(&tss_tree_mutex);
            mailbox->reader_blocked = FALSE;
            mailbox->reader_thread_id = PROCESS_ID_NONE;

            assert(circular_queue_peek(mailbox->queue) >= 0,
                   "Was unblocked but no messages are available in mailbox %d", mailbox_id)
        }
        else
        {
            // No messages are available and we were instructed to not block the caller => return.
            mutex_kernel_signal(&tss_tree_mutex);

            return STORM_RETURN_MAILBOX_EMPTY;
        }
    }

    // Receive the message.
    next_message_size = circular_queue_peek(mailbox->queue);

    assert(next_message_size != -1,
        "circular_queue_peek returned -1 even though a message should be available");

    unsigned int next_message_data_size = next_message_size - sizeof(message_type);

    if (message_parameter->length >= next_message_data_size)
    {
        // The message is small enough to fit into the buffer provided by the caller. Dequeue it
        // from the queue.
        message_type *message_received = circular_queue_dequeue(mailbox->queue);

        assert(message_received != NULL,
            "circular_queue_peek returned %d but circular_queue_dequeue returned NULL. "
            "Corrupted data structure or software bug or reentrancy problem...?", mailbox_id)

        message_parameter->protocol = message_received->protocol;
        message_parameter->message_class = message_received->class;
        message_parameter->length = message_received->length;

        memory_copy(message_parameter->data, message_received->data, message_received->length);

        // Is there now room for unblocking the sender waiting on this mailbox?
        if (mailbox->blocked_size != 0 &&
            mailbox->blocked_size < circular_queue_get_maximum_enqueue_size(mailbox->queue))
        {
            thread_unblock_mailbox_send(mailbox_id);
            mailbox->blocked_size = 0;
        }

        mutex_kernel_signal(&tss_tree_mutex);

        return STORM_RETURN_SUCCESS;
    }
    else // next_message_data_size > message_parameter->length
    {
        DEBUG_MESSAGE(DEBUG, "Message in mailbox %u was too large (max %u bytes, message size was %u bytes)",
                      mailbox_id, message_parameter->length, next_message_data_size);

        message_parameter->length = next_message_data_size;
        mutex_kernel_signal(&tss_tree_mutex);

        return STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE;
    }
}

// Internal methods, should not be considered part of the public API.
mailbox_id_type mailbox_initialize(mailbox_type *new_mailbox, int size,
                                   process_id_type user_process_id, cluster_id_type user_cluster_id,
                                   thread_id_type user_thread_id)
{
    new_mailbox->owner_process_id = current_process_id;
    new_mailbox->owner_cluster_id = current_cluster_id;
    new_mailbox->owner_thread_id = current_thread_id;

    new_mailbox->user_process_id = user_process_id;
    new_mailbox->user_cluster_id = user_cluster_id;
    new_mailbox->user_thread_id = user_thread_id;

    // FIXME: Check for allowed limits.
    new_mailbox->blocked_size = 0;

    new_mailbox->reader_blocked = FALSE;

    new_mailbox->queue = memory_global_allocate(size);
    circular_queue_initialize(new_mailbox->queue, size);

    new_mailbox->next = NULL;

    new_mailbox->id = mailbox_get_free_id();

    return new_mailbox->id;
}
