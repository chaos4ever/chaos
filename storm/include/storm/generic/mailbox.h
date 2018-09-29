// Abstract: Function prototypes for the mailbox IPC system.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/mailbox.h>
#include <storm/generic/defines.h>
#include <storm/generic/mutex.h>
#include <storm/generic/types.h>

// This defines the structure for messages inside mailboxes.
typedef struct
{
    process_id_type sender_process_id;
    cluster_id_type sender_cluster_id;
    thread_id_type sender_thread_id;

    unsigned int protocol;
    unsigned int class;

    struct message_type *next;

    unsigned int length;

    uint8_t data[0];
} message_type;

// The format of a mailbox.
typedef struct
{
    // Mailbox id.
    mailbox_id_type id;

    // Who owns the mailbox.
    process_id_type owner_process_id;
    cluster_id_type owner_cluster_id;
    thread_id_type owner_thread_id;

    // Who is allowed to send to the mailbox.
    process_id_type user_process_id;
    cluster_id_type user_cluster_id;
    thread_id_type user_thread_id;

    // Size of the mailbox.
    unsigned int total_size;

    // Size of the free part of the mailbox.
    unsigned int free_size;

    // Size of the message the blocked sender is awaiting to deliver.
    unsigned int blocked_size;

    // Number of messages in the mailbox.
    unsigned int number_of_messages;

    // Is a thread blocked on reading from this mailbox?
    bool reader_blocked;

    // The thread ID of the blocked reader, or THREAD_NONE if no thread
    // blocked on this mailbox.
    thread_id_type reader_thread_id;

    // Start of the first message in the mailbox.
    message_type *first_message;
    message_type *last_message;

    // Tree nodes.
    struct mailbox_type *next;
} PACKED mailbox_type;

extern return_type mailbox_create(mailbox_id_type *mailbox_id, unsigned int size, process_id_type user_process_id,
                                  cluster_id_type user_cluster_id, thread_id_type user_thread_id);
extern return_type mailbox_destroy(mailbox_id_type mailbox_id);
extern return_type mailbox_flush(mailbox_id_type mailbox_id);
extern return_type mailbox_send(mailbox_id_type mailbox_id, message_parameter_type *message_parameter);
extern return_type mailbox_receive(mailbox_id_type mailbox_id, message_parameter_type *message_parameter);
extern void mailbox_init(void) INIT_CODE;
extern return_type mailbox_create_kernel(mailbox_id_type *mailbox_id, unsigned int size, process_id_type user_process_id,
                                         cluster_id_type user_cluster_id, thread_id_type user_thread_id);
