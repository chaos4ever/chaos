// Abstract: Return values for the file library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

enum
{
    // The call returned successfully.
    FILE_RETURN_SUCCESS,

    // The file/directory did not exist.
    FILE_RETURN_FILE_ABSENT,

    // The requested service was not available.
    FILE_RETURN_SERVICE_UNAVAILABLE,

    // Sending or receiving to or from a mailbox failed.
    FILE_RETURN_IPC_FAILED
};
