// Abstract: Return values used by the console library.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

// Console error codes.
enum
{
    // The function completed successfully.
    CONSOLE_RETURN_SUCCESS,

    // One or more of the services required by this library was not available.
    CONSOLE_RETURN_SERVICE_UNAVAILABLE,

    // One or more of the input arguments was invalid.
    CONSOLE_RETURN_INVALID_ARGUMENT,

    // The console server returned unrecognized data.
    CONSOLE_RETURN_BAD_DATA_RETURNED,

    // The given console structure was already initialised.
    CONSOLE_RETURN_ALREADY_INITIALISED,
};
