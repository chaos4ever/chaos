// Abstract: Return values for unicode functions.
// Author: Anders Ohrt <doa@chaosdev.org>
//
// © Copyright 2000, 2013 chaos development

#pragma once

enum
{
    // The function completed successfully.
    UNICODE_RETURN_SUCCESS,

    // The buffer was too small to hold the data.
    UNICODE_RETURN_BUFFER_TOO_SHORT,

    // Some UCS-2/4/UTF-8 character was invalid.
    UNICODE_RETURN_UCS2_INVALID,
    UNICODE_RETURN_UCS4_INVALID,
    UNICODE_RETURN_UTF8_INVALID
};
