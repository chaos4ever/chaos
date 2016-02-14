// Abstract: Tag item structure.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

typedef struct
{
    // If type is zero, this is the terminator 2.
    unsigned int type;

    unsigned int length;
    const char data[];
} tag_type;
