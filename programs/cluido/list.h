// Abstract: Linked list function prototypes and structure definitions.
// Author: Per Lundberg <per@chaosdev.io>
//         Anders Öhrt <hal@chaosdev.org>
//
// © Copyright 1999 chaos development

#pragma once

typedef struct
{
    struct list_type *previous;
    struct list_type *next;
    void *data;
} list_type;
