// Abstract: ELF execution library return values.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

// Return values.
enum
{
    // The function completed successfully.
    EXECUTE_ELF_RETURN_SUCCESS,

    EXECUTE_ELF_RETURN_ELF_UNSUPPORTED,
    EXECUTE_ELF_RETURN_IMAGE_INVALID,

    // FIXME: More detailed errors. This is more a fallback for "unknown errors".
    EXECUTE_ELF_RETURN_FAILED
};
