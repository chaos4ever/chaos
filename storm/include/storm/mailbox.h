// Abstract: Mailbox structures
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

#include <storm/types.h>

// The message_parameter_type is a structure which gets passed in to
// mailbox_send and mailbox_receive.
typedef struct
{
    unsigned int protocol;
    unsigned int message_class;

    // When receiving a message, the maximum size of a message that can
    // be received. This is usually (but not necessarily) the number of
    // bytes allocated for the 'data' pointer.
    //
    // When sending a message, this is the length of the message.
    unsigned int length;

    bool block;

    // A pointer to the data buffer. Note that when receiving messages,
    // this is presumed to be allocated by the caller already;
    // mailbox_receive will not allocate memory for you.
    void *data;
} message_parameter_type;
