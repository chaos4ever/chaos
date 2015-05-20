// Abstract: Return values
// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>
//          Anders Ohrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once

// External error numbers.
enum
{
    // The system call succeeded.
    STORM_RETURN_SUCCESS = 0,

    // The code for the specified system call isn't finished yet...
    STORM_RETURN_FUNCTION_UNFINISHED,

    // We ran out of memory.
    STORM_RETURN_OUT_OF_MEMORY,

    // Access to the given action was denied for some reason.
    STORM_RETURN_ACCESS_DENIED,

    // Whatever you requested, it is busy. ;)
    STORM_RETURN_BUSY,

    // A limit has been overrun.
    STORM_RETURN_LIMIT_OVERRUN,

    // The DMA channel specified does not exist.
    STORM_RETURN_INVALID_DMA_CHANNEL,

    // Some, or all, of the sections was not page aligned.
    STORM_RETURN_UNALIGNED_SECTION,

    // The requested mailbox was not available.
    STORM_RETURN_MAILBOX_UNAVAILABLE,

    // The message could not be put in the mailbox since it was full.
    STORM_RETURN_MAILBOX_FULL,

    // The message was too large to fit into the given buffer.
    STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE,

    // The mailbox was empty.
    STORM_RETURN_MAILBOX_EMPTY,

    // The protocol was not available. (service_get)
    STORM_RETURN_PROTOCOL_UNAVAILABLE,

    // All the services didn't fit into the service_parameter_type structure. (service_get)
    STORM_RETURN_TOO_MANY_SERVICES,

    // We tried to memory_deallocate() a pointer that wasn't referencing to any valid block.
    STORM_RETURN_MEMORY_NOT_ALLOCATED,

    // One or more of the input arguments was invalid.
    STORM_RETURN_INVALID_ARGUMENT,

    // One or more of the sections were placed outside the limits of the process addressing space.
    STORM_RETURN_SECTION_MISPLACED,
};
