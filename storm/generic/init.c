// Abstract: Initialise the state of a new process.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
// © Copyright 2000, 2013 chaos development.

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/init.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/mutex.h>
#include <storm/generic/return_values.h>
#include <storm/generic/thread.h>

// If defined as TRUE, this code will print lots of debugging information.
#define DEBUG FALSE

// This system call should be the first system call the process runs. It will perform such initialisation that can only (easily) be
// performed from within the process' own addressing space. Currently, all it does is configure the process' virtual memory AVL
// tree correctly, so that the process can allocate memory.
return_type init(void)
{
    // Clear the bitmap, marking all slots as free.
    memory_set_u8((u8 *) process_avl_header->bitmap, 0, SIZE_PAGE - sizeof (avl_header_type));

    // Mark the first entry in the bitmap as used.
    process_avl_header->bitmap[0] = 1;

    // Per: I'm not really sure if this is needed, but I presume memory_virtual_reserve () could happen to allocate memory at
    // times?
    mutex_kernel_wait(&tss_tree_mutex);

    DEBUG_MESSAGE(DEBUG, "Reserving VM for process AVL tree");
    memory_virtual_reserve(GET_PAGE_NUMBER(BASE_PROCESS_AVL_TREE), process_avl_header->limit_pages_bitmap +
                           process_avl_header->limit_pages_array);

    DEBUG_MESSAGE(DEBUG, "Reserving VM for process page tables");
    memory_virtual_reserve(GET_PAGE_NUMBER(BASE_PROCESS_PAGE_TABLES), 1024);

    DEBUG_MESSAGE(DEBUG, "Reserving VM for global memory");
    memory_virtual_reserve(GET_PAGE_NUMBER(BASE_GLOBAL), SIZE_IN_PAGES(SIZE_GLOBAL));

    DEBUG_MESSAGE(DEBUG, "Reserving VM for process stack");
    memory_virtual_reserve(GET_PAGE_NUMBER(BASE_PROCESS_STACK), SIZE_IN_PAGES(SIZE_PROCESS_STACK));

    DEBUG_MESSAGE(DEBUG, "Reserving VM for code and data segments");
    memory_virtual_reserve(GET_PAGE_NUMBER(current_tss->virtual_code_base), current_tss->code_pages);
    memory_virtual_reserve(GET_PAGE_NUMBER(current_tss->virtual_data_base), current_tss->data_pages);

    mutex_kernel_signal(&tss_tree_mutex);
    current_tss->initialised = TRUE;

    DEBUG_MESSAGE(DEBUG, "Done");

    return STORM_RETURN_SUCCESS;
}
