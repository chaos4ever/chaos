// Abstract: Return values for the log library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development.

#pragma once

enum
{
    // The function executed successfully.
    LOG_RETURN_SUCCESS,

    // One of the arguments to a function was invalid in some way.
    LOG_RETURN_INVALID_ARGUMENT,

    // The log service could not be resolved.
    LOG_RETURN_SERVICE_UNAVAILABLE,
};
