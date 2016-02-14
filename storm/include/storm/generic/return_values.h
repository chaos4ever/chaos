// Abstract: Return values.

// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once
#include <storm/return_values.h>

// Internal return values
// FIXME: Look over this and see that all is good.
enum
{
    // Function returned successfully.
    RETURN_SUCCESS = 0,

    // The AVL tree was broken.
    RETURN_AVL_TREE_BROKEN,

    // The node you tried to free, was not previously allocated.
    RETURN_AVL_NODE_ALREADY_FREE,

    // FIXME: Remove this when the kernel API has stabilised.
    RETURN_FUNCTION_UNFINISHED,

    // We tried to put something at the NULL descriptor.
    RETURN_GDT_ENTRY_BAD,

    // The ELF image is corrupted.
    RETURN_ELF_INVALID,

    // The ELF image is in an unsupported format. (bad word length, for example)
    RETURN_ELF_UNSUPPORTED,

    // One, or more, of the sections in this ELF image is not placed in the regions reserved for data and code.
    RETURN_ELF_SECTION_MISPLACED,

    // There are multiple instances of the same section type (i.e. multiple
    // .data sections)
    RETURN_ELF_SECTION_MULTIPLE_INSTANCES,

    // "Couldn't get a free page" :-)
    RETURN_OUT_OF_MEMORY,

    // We tried to reserve a page already in use.
    RETURN_PAGE_USED,

    // We tried to reserve a page that couldn't be found.
    RETURN_PAGE_NOT_FOUND,

    // The ports requested are not available.
    RETURN_PORTS_UNAVAILABLE,

    // No threads were unblocked.
    RETURN_NO_THREAD_UNBLOCKED,

    // The thread was unblocked successfully.
    RETURN_THREAD_UNBLOCKED,

    // We're out of threads.
    RETURN_THREAD_UNAVAILABLE,

    // The thread given to a function does not exist.
    RETURN_THREAD_INVALID,

    // One or more of the input arguments was invalid.
    RETURN_INVALID_ARGUMENT,

    // The requested memory region could not be deallocated, since it wasn't previously allocated.
    RETURN_MEMORY_NOT_ALLOCATED,
};
