// Abstract: Log urgencies.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development.

#pragma once

enum
{
    // Emergency messages are to be printed when the program has encountered a serious error which
    // forces termination.
    LOG_URGENCY_EMERGENCY,

    // Something is wrong, but execution may continue.
    LOG_URGENCY_ERROR,

    // Something might be wrong, but we are not entirely sure.
    LOG_URGENCY_WARNING,

    // Typical informative messages are showing which hardware we detected, etc.
    LOG_URGENCY_INFORMATIVE,

    // Debug messages are for debugging purposes only -- normally, they can be safely ignored.
    // Eventually, they will be removed.
    LOG_URGENCY_DEBUG
};
