// Abstract: IPC library return values.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    // The function call completed successfully.
    IPC_RETURN_SUCCESS,

    // The system call ran out of memory.
    IPC_RETURN_OUT_OF_MEMORY,

    // One or more of the input arguments was invalid.
    IPC_RETURN_INVALID_ARGUMENT,

    // A connection to this service was requested.
    IPC_RETURN_CONNECTION_REQUESTED,

    // The services didn't fit into the data structure. (ipc_service_resolv)
    IPC_RETURN_TOO_MANY_SERVICES,

    // We failed on a system_call_mailbox_create. (ipc_service_establish)
    IPC_RETURN_FAILED_MAILBOX_CREATE,

    // We failed on a system_call_mailbox_send. (ipc_service_establish)
    IPC_RETURN_FAILED_MAILBOX_SEND,

    // We failed on a system_call_mailbox_receive. (ipc_service_connection_request)
    IPC_RETURN_FAILED_MAILBOX_RECEIVE,

    // No service of the given type was found. (ipc_service_resolve)
    IPC_RETURN_SERVICE_UNAVAILABLE,

    // The requested operation was not allowed.
    IPC_RETURN_ACCESS_DENIED,

    // The given mailbox was not available.
    IPC_RETURN_MAILBOX_UNAVAILABLE,

    // We did a non-blocking ipc_receive (), and the only data that was available was too large.
    IPC_RETURN_MESSAGE_TOO_LARGE,

    // We did a non-blocking ipc_receive (), and there was no data in the mailbox.
    IPC_RETURN_MAILBOX_EMPTY,

    // We did a non-blocking ipc_send (), and the mailbox we addressed was full.
    IPC_RETURN_MAILBOX_FULL,

    // We got an unknown IPC command and could thus not proceed.
    IPC_RETURN_UNKNOWN_COMMAND,

    // Something went wrong, and we could not figure out what. (probably because of an unimplemented kernel return value).
    IPC_RETURN_UNKNOWN_ERROR,
};
