// Abstract: storm glue layer for liballoc
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2019 chaos development

#include <storm/return_values.h>
#include <storm/generic/debug.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>

#include "liballoc.h"

/** This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide.
 *
 * \return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
int liballoc_lock()
{
    // Dummy implementation - the calling code in memory_global.c
    // already ensures the caller has taken the lock.
    return 0;
}

/** This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * \return 0 if the lock was successfully released.
 */
int liballoc_unlock()
{
    return 0;
}

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
void *liballoc_alloc(int num_pages)
{
    // FIXME: See if we can use a simpler, sbrk-like approach instead
    // and just let the page fault handler grab physical pages on
    // demand. We still need something to keep track of unallocated/free
    // global memory pages though... the famous chicken and egg problem. :)

    uint32_t virtual_page = memory_global_allocate_page(num_pages);

    uint32_t physical_page;

    if (memory_physical_allocate(&physical_page, num_pages, "Global memory data structure") != STORM_RETURN_SUCCESS)
    {
        DEBUG_HALT("Failed to allocate memory");
    }

    memory_virtual_map(virtual_page, physical_page, num_pages, PAGE_KERNEL);

    return (void *) (virtual_page * SIZE_PAGE);
}

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
int liballoc_free(void *ptr, int num_pages)
{
    if (memory_global_deallocate_page(((uint32_t) ptr) / SIZE_PAGE) == STORM_RETURN_SUCCESS)
    {
        return 0;
    }

    // Something went wrong, but we have no way to propagate the error to the caller.
    return -1;
}
