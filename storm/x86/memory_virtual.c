// Abstract: Provides functions for managing the virtual memory (MMU) mechanisms of the IA32 architecture.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999 chaos development.

// Define as true to get debug information.
#define DEBUG FALSE

// If you define this, the AVL tree will be checked all the time. Good for ensuring that the data structure is not
// corrupted because of bugs in the code.
#define CHECK

#include <storm/generic/avl.h>
#include <storm/generic/avl_debug.h>
#include <storm/generic/avl_update.h>
#include <storm/generic/bit.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/mutex.h>
#include <storm/generic/port.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>

#include <storm/x86/gdt.h>
#include <storm/x86/trap.h>
#include <storm/x86/tss.h>

// FIXME: never invalidate cache if it is not needed. For example when mapping a new page table. Since it didn't exist before it
// can not be in the cache, and there is no need to invalidate. The performance loss of invalidating the cache all the time is
// rather big.

// FIXME: All mapping functions should take global as a flag. All kernel memory, GDT, IDT and all other memory shared by all
// processes should be marked global to increase performance. This applies to all global memory, and thus all mailboxes.

// The following flags are used when creating new entries in the page directory.
#define PAGE_DIRECTORY_FLAGS     (PAGE_WRITABLE | PAGE_NON_PRIVILEGED)

page_directory_entry_page_table *kernel_page_directory;
page_directory_entry_page_table *process_page_directory = (page_directory_entry_page_table *) BASE_PROCESS_PAGE_DIRECTORY;
page_table_entry *shared_page_tables;

// FIXME: We should have one mutex per cluster/thread/process, or whatever seems best.

//mutex_kernel_type memory_map_mutex = MUTEX_UNLOCKED;

// The following variables are for the allocation of physical pages for the global heap.
uint32_t largest_address;
uint32_t mapped_pages;

avl_header_type *process_avl_header = (avl_header_type *) BASE_PROCESS_AVL_TREE;

// Initialise the virtual memory system.
void memory_virtual_init(void)
{
    uint32_t counter;
    uint32_t physical_page;

    // Reserve memory for shared page tables which is mapped into all processes' linear memory at address BASE_GLOBAL (defined in
    // storm/x86/defines.h), and wipe it out.

    // FIXME: Check return value.
    memory_physical_allocate(
        &physical_page,
        (GET_PAGE_NUMBER(SIZE_GLOBAL) / 1024),
        "Shared page table"
    );

    shared_page_tables = (page_table_entry *)(physical_page * SIZE_PAGE);
    memory_set_uint8_t ((uint8_t *) shared_page_tables, 0, SIZE_GLOBAL / 1024);

    // FIXME: Check return value.
    memory_physical_allocate(&physical_page, 1, "Kernel page directory.");
    kernel_page_directory = (page_directory_entry_page_table *) (physical_page * SIZE_PAGE);
    memory_set_uint8_t ((uint8_t *) kernel_page_directory, 0, SIZE_PAGE);

    // Map some important stuff. The same page_directory is used for exception handlers also, so map what is needed for them.

    // GDT and IDT.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_GDT),
        GET_PAGE_NUMBER(BASE_GDT),
        1, PAGE_KERNEL
    );

    // Page directory.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_PROCESS_PAGE_DIRECTORY),
        GET_PAGE_NUMBER((uint32_t) kernel_page_directory),
        1,
        PAGE_KERNEL
    );

    // Kernel stack.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_KERNEL_STACK),
        GET_PAGE_NUMBER(BASE_KERNEL_STACK),
        SIZE_IN_PAGES(SIZE_KERNEL_STACK),
        PAGE_KERNEL
    );

    // Text-mode screen.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_SCREEN),
        GET_PAGE_NUMBER(BASE_SCREEN),
        16,
        PAGE_KERNEL
    );

    // Multiboot module names.
    // FIXME: this is not very snyggt.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_MODULE_NAME),
        GET_PAGE_NUMBER(BASE_MODULE_NAME),
        1,
        PAGE_KERNEL
    );

    // Kernel code and data.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_KERNEL),
        GET_PAGE_NUMBER(BASE_KERNEL),
        SIZE_IN_PAGES((uint32_t) &_end - BASE_KERNEL),
        PAGE_KERNEL
    );

    // Insert shared page tables into kernel page_directory.
    for (counter = 0; counter < SIZE_IN_PAGES(SIZE_GLOBAL) / 1024; counter++)
    {
        uint32_t index = (GET_PAGE_NUMBER(BASE_GLOBAL) / 1024) + counter;

        kernel_page_directory[index].present = 1;
        kernel_page_directory[index].flags = PAGE_DIRECTORY_FLAGS;
        kernel_page_directory[index].accessed = 0;
        kernel_page_directory[index].zero = 0;
        kernel_page_directory[index].page_size = 0;

        // This will boost performance a little, since this memory won't have to be cache invalidated on task switches.
        if (cpu_info.flags.flags.pge)
        {
            kernel_page_directory[index].global = 1;
        }
        else
        {
            kernel_page_directory[index].global = 0;
        }

        kernel_page_directory[index].available = 0;
        kernel_page_directory[index].page_table_base = (GET_PAGE_NUMBER(shared_page_tables) + counter);

        // Map the shared page tables.
        memory_virtual_map_kernel(
            kernel_page_directory,
            GET_PAGE_NUMBER(BASE_PROCESS_PAGE_TABLES) + index,
            GET_PAGE_NUMBER(shared_page_tables) + counter,
            1,
            PAGE_KERNEL
        );
    }
}

// This function is called at the end of the kernel init. It enables paging and updates some data.
void memory_virtual_enable(void)
{
    //  debug_print
    //    ("Would save %u K of memory.\n",
    //     ((page_avl_header->limit_nodes -
    //       avl_get_number_of_entries (page_avl_header->root)) *
    //      sizeof (avl_node_type)) / KB + 12 +
    //     ((uint32_t) &_init_end - (uint32_t) &_init_start) / KB);

    DEBUG_MESSAGE(DEBUG, "Called");

    // Map the kernel TSS.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_VIRTUAL_KERNEL_TSS),
        GET_PAGE_NUMBER(BASE_KERNEL_TSS),
        1,
        PAGE_KERNEL
    );

    // Map global data in the shared page tables. Since the page tables are the same for all processes, we can just map for
    // the kernel.

    // FIXME: need to find out how many pages are in use.
    memory_virtual_map_kernel(
        kernel_page_directory,
        GET_PAGE_NUMBER(BASE_PHYSICAL_MEMORY_TREE),
        GET_PAGE_NUMBER((uint32_t) page_avl_header),
        page_avl_pages,
        PAGE_KERNEL
    );

    // Update the pointers in lists and trees to point to where they are mapped.
    avl_tree_move(page_avl_header, BASE_PHYSICAL_MEMORY_TREE - (uint32_t) page_avl_header);

    page_avl_header = (avl_header_type *) BASE_PHYSICAL_MEMORY_TREE;

    // FIXME: Do this funky music, d0od.
    // Unreserve all the memory used by the page allocation system.
    //     page_range_unreserve (whatever);
    // ...and reserve the parts needed.

    // Specify page directory to use for the kernel.
    cpu_set_cr3((uint32_t) kernel_page_directory);

    // Enable paging (virtual memory), protection and set the extension type flag. No external datastructures can be accessed
    // after this.
    cpu_set_cr0(CPU_CR0_PE | CPU_CR0_PG | CPU_CR0_ET);

    if (cpu_info.flags.flags.pge == 1)
    {
        cpu_set_cr4(CPU_CR4_PGE);
    }

    // Prepare for task switching by loading TR and setting up a TSS for the kernel. Must be done after paging is enabled.
    gdt_setup_tss_descriptor(SELECTOR_TSS1 >> 3,
                             (storm_tss_type *) BASE_VIRTUAL_KERNEL_TSS, 0,
                             sizeof(storm_tss_type));
    cpu_set_tr(SELECTOR_TSS1);

    DEBUG_MESSAGE(DEBUG, "Left");
}

// Map pages when paging is still disabled. The difference is that we access the page_directories and page tables at their physical
// address.
return_type memory_virtual_map_kernel(page_directory_entry_page_table *page_directory, uint32_t virtual_page,
                                      uint32_t physical_page, uint32_t pages, uint32_t flags)
{
    page_table_entry *page_table;
    uint32_t counter, index;

    for (counter = 0; counter < pages; counter++)
    {
        // Which page table?
        index = (virtual_page + counter) / 1024;

        if (page_directory[index].present == 0)
        {
            uint32_t page_table_page;

            // FIXME: Check return value.
            memory_physical_allocate(&page_table_page, 1, "Kernel page table.");

            // Page Table is not set up yet, let's set up a new one.
            page_directory[index].present = 1;
            page_directory[index].flags = PAGE_DIRECTORY_FLAGS;
            page_directory[index].accessed = 0;
            page_directory[index].zero = 0;
            page_directory[index].page_size = 0;
            page_directory[index].global = 0;
            page_directory[index].available = 0;
            page_directory[index].page_table_base = page_table_page;

            // Make sure we could allocate memory.
            if (page_directory[index].page_table_base == 0)
            {
                return RETURN_OUT_OF_MEMORY;
            }

            // Make sure no pages in the new page table are marked as present.
            page_table = (page_table_entry *) (page_directory[index].page_table_base * SIZE_PAGE);
            memory_set_uint8_t((uint8_t *) page_table, 0, SIZE_PAGE);

            // Map the newly created page table in the page_directory.
            memory_virtual_map_kernel(
                page_directory,
                GET_PAGE_NUMBER(BASE_PROCESS_PAGE_TABLES) + index,
                GET_PAGE_NUMBER((uint32_t) page_table),
                1,
                PAGE_KERNEL
            );
        }
        else
        {
            page_table = (page_table_entry *) (page_directory[index].page_table_base * SIZE_PAGE);
        }

        // Which entry in the page table to modify.
        index = (virtual_page + counter) % 1024;

        // Set up a new page table entry.
        page_table[index].present = 1;
        page_table[index].flags = flags;
        page_table[index].accessed = 0;
        page_table[index].dirty = 0;
        page_table[index].zero = 0;
        page_table[index].available = 0;
        page_table[index].page_base = physical_page + counter;
    }

    return RETURN_SUCCESS;
}

// This function is for mapping memory in syscalls (when paging is enabled).
static return_type memory_virtual_map_real(uint32_t virtual_page, uint32_t physical_page, uint32_t pages, uint32_t flags)
{
    page_table_entry *page_table;
    uint32_t counter, index;

    DEBUG_MESSAGE(
        DEBUG,
        "Called (%x, %x, %x, %x)",
        virtual_page, physical_page, pages, flags
    );

    for (counter = 0; counter < pages; counter++)
    {
        index = (virtual_page + counter) / 1024;

        DEBUG_MESSAGE(DEBUG, "index = %u, counter = %u", index, counter);

        if (process_page_directory[index].present == 0)
        {
            uint32_t page_table_page;

            // Page Table is not set up yet. Let's set up a new one.
            process_page_directory[index].present = 1;
            process_page_directory[index].flags = PAGE_DIRECTORY_FLAGS;
            process_page_directory[index].accessed = 0;
            process_page_directory[index].zero = 0;
            process_page_directory[index].page_size = 0;
            process_page_directory[index].global = 0;
            process_page_directory[index].available = 0;

            // FIXME: Check return value.
            DEBUG_MESSAGE(DEBUG, "Allocating memory for a new page table.");
            memory_physical_allocate(&page_table_page, 1, "Process page table.");

            process_page_directory[index].page_table_base = page_table_page;

            //      memory_virtual_cache_invalidate
            //        ((void *) (process_page_directory[index].page_table_base * SIZE_PAGE));

            // Make sure we could allocate memory.
            if (process_page_directory[index].page_table_base == 0)
            {
                return RETURN_OUT_OF_MEMORY;
            }

            DEBUG_MESSAGE(DEBUG, "Recursing.");

            memory_virtual_map_real(
                GET_PAGE_NUMBER(BASE_PROCESS_PAGE_TABLES) + index,
                (uint32_t) process_page_directory[index].page_table_base,
                1,
                PAGE_KERNEL
            );

            memory_set_uint8_t((uint8_t *)(BASE_PROCESS_PAGE_TABLES + (index * SIZE_PAGE)), 0, SIZE_PAGE);
        }

        // The page table is in the page_directory.
        page_table = (page_table_entry *) (BASE_PROCESS_PAGE_TABLES + (index * SIZE_PAGE));

        // Which entry in the page table to modify.
        index = (virtual_page + counter) % 1024;

        // Set up a new page table entry.
        // FIXME: fix the flags to use defines. Both here and in the other functions.

        DEBUG_MESSAGE(DEBUG, "slem! %x", page_table);

        page_table[index].present = 1;
        page_table[index].flags = flags;
        page_table[index].accessed = 0;
        page_table[index].dirty = 0;
        page_table[index].zero = 0;
        page_table[index].available = 0;
        page_table[index].page_base = physical_page + counter;

        // FIXME: Only invalidate if present was 0. But there is maybe no performance loss in invalidating unconditionally?
        DEBUG_MESSAGE(DEBUG, "Invalidating cache");

        memory_virtual_cache_invalidate((void *)((virtual_page + counter) * SIZE_PAGE));

        DEBUG_MESSAGE(DEBUG, "Done");
    }

    DEBUG_MESSAGE(DEBUG, "Ritirning");
    return RETURN_SUCCESS;
}

// Map memory for another memory space in a system call.
static return_type memory_virtual_map_other_real(storm_tss_type *tss, uint32_t virtual_page, uint32_t physical_page,
                                                 uint32_t pages, uint32_t flags)
{
    page_directory_entry_page_table *page_directory = (page_directory_entry_page_table *) BASE_PROCESS_TEMPORARY;
    page_table_entry *page_table = (page_table_entry *) (BASE_PROCESS_TEMPORARY + SIZE_PAGE);
    uint32_t counter, index;
    bool global = (flags & PAGE_GLOBAL) != 0;

    // Remove PAGE_GLOBAL from the flags, if set.
    flags &= ~PAGE_GLOBAL;

    DEBUG_MESSAGE(DEBUG, "%x %x %u %u", virtual_page, physical_page, pages, flags);

    // Start by mapping the other process' page_directory.
    memory_virtual_map_real(
        GET_PAGE_NUMBER(BASE_PROCESS_TEMPORARY),
        GET_PAGE_NUMBER(tss->cr3),
        1,
        PAGE_KERNEL
    );

    // Start the mapping.
    for (counter = 0; counter < pages; counter++)
    {
        index = (virtual_page + counter) / 1024;

        DEBUG_MESSAGE(DEBUG, "Changing page directory entry %x", index);

        // Page Table is not yet set up.
        if (page_directory[index].present == 0)
        {
            uint32_t page_table_page;

            DEBUG_MESSAGE(DEBUG, "page_directory[index].present == 0");

            // FIXME: Check return value.
            memory_physical_allocate(&page_table_page, 1, "Page table.");

            // Let's set up a new page table.
            page_directory[index].present = 1;
            page_directory[index].flags = PAGE_DIRECTORY_FLAGS;
            page_directory[index].accessed = 0;
            page_directory[index].zero = 0;
            page_directory[index].page_size = 0;
            page_directory[index].global = 0;
            page_directory[index].available = 0;
            page_directory[index].page_table_base = page_table_page;

            memory_virtual_map_other_real(
                tss,
                GET_PAGE_NUMBER(BASE_PROCESS_PAGE_TABLES) + index,
                (uint32_t) page_directory[index].page_table_base,
                1,
                PAGE_KERNEL
            );

            // Map the page table.
            memory_virtual_map_real(
                GET_PAGE_NUMBER(page_table),
                (uint32_t) page_directory[index].page_table_base,
                1,
                PAGE_KERNEL
            );
            memory_set_uint8_t ((uint8_t *) page_table, 0, SIZE_PAGE);
        }

        memory_virtual_map_real(
            GET_PAGE_NUMBER(page_table),
            (uint32_t) page_directory[index].page_table_base,
            1,
            PAGE_KERNEL
        );

        // Which entry in the page table to modify.
        index = (virtual_page + counter) % 1024;

        DEBUG_MESSAGE(DEBUG, "Changing entry %u in the page table", index);

        // Set up a new page table entry.
        page_table[index].present = 1;
        page_table[index].flags = flags;
        page_table[index].accessed = 0;
        page_table[index].dirty = 0;
        page_table[index].zero = 0;
        page_table[index].available = 0;
        page_table[index].page_base = physical_page + counter;

        if (global)
        {
            page_table[index].global = 1;
        }
    }

    // No actual need to unmap the temporary pages.
    return RETURN_SUCCESS;
}

// This is the wrapper for memory_virtual_map. All it does is to wrap the code in plastic mutexes.
return_type memory_virtual_map(uint32_t virtual_page, uint32_t physical_page, uint32_t pages, uint32_t flags)
{
    return_type return_value;

    DEBUG_MESSAGE(DEBUG, "Mapping %x at %x (%u pages)", physical_page,
                  virtual_page, pages);

    //  mutex_kernel_wait (&memory_map_mutex);
    DEBUG_MESSAGE(DEBUG, "Calling memory_virtual_map_real");
    return_value = memory_virtual_map_real(virtual_page, physical_page,
                                           pages, flags);
    DEBUG_MESSAGE(DEBUG, "Done");
    DEBUG_MESSAGE(DEBUG, "Unlocking mutex");
    //  mutex_kernel_signal (&memory_map_mutex);
    DEBUG_MESSAGE(DEBUG, "Done");

    return return_value;
}

// This is the wrapper for memory_virtual_map_other. All it does is protect the function with mutexes.
return_type memory_virtual_map_other(storm_tss_type *tss, uint32_t virtual_page, uint32_t physical_page,
                                     uint32_t pages, uint32_t flags)
{
    return_type return_value;

    DEBUG_MESSAGE(DEBUG, "Virtual page: %x, physical page: %x, pages: %x",
                  virtual_page, physical_page, pages);

    //  mutex_kernel_wait (&memory_map_mutex);
    return_value = memory_virtual_map_other_real(tss, virtual_page,
                   physical_page, pages, flags);
    //  mutex_kernel_signal (&memory_map_mutex);

    return return_value;
}

// Unmap a region of pages for the current process.
void memory_virtual_unmap(uint32_t virtual_page, uint32_t pages)
{
    page_table_entry *page_table;
    uint32_t counter, index;

    //  mutex_kernel_wait (&memory_map_mutex);

    for (counter = 0; counter < pages; counter++)
    {
        index = (virtual_page + counter) / 1024;

        if (process_page_directory[index].present != 0)
        {
            // The page table exists, continue.
            page_table = (page_table_entry *)(BASE_PROCESS_PAGE_TABLES + index * SIZE_PAGE);

            // Which entry in the page table to modify.
            index = (virtual_page + counter) % 1024;

            // Deallocate the page range and unmap the page.
            memory_physical_deallocate(page_table[index].page_base);
            page_table[index].present = 0;
        }
    }

    //  mutex_kernel_signal (&memory_map_mutex);
}

// Allocate virtual memory.
return_type memory_virtual_allocate(uint32_t *page_number, uint32_t pages)
{
    // Sanitize the parameters
    if (pages == 0)
    {
        DEBUG_HALT("Pages must be > 0!");
    }

    avl_node_type *node = process_avl_header->root;
    avl_node_type *insert_node;

    if (tss_tree_mutex != MUTEX_LOCKED &&
            memory_mutex != MUTEX_LOCKED && initialised)
    {
        DEBUG_HALT("Code is not properly mutexed.");
    }

    DEBUG_MESSAGE(DEBUG, "Called with page_number = %p, pages = %u", page_number, pages);

    DEBUG_MESSAGE(
        DEBUG,
        "p: %p, r00t: %p, l: %u",
        process_avl_header,
        process_avl_header->root,
        process_avl_header->limit_nodes
    );

#ifdef CHECK
    avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

    // FIXME: Check if there is enough free memory.
    while (node != NULL)
    {
        if (node->largest_free_less >= pages)
        {
            DEBUG_MESSAGE(DEBUG, "less");
            node = (avl_node_type *) node->less;
        }
        else if (node->free_length >= pages)
        {
            DEBUG_MESSAGE(DEBUG, "found");

            if (node->busy_length == 0)
            {
                node->busy_length = pages;
                node->free_length -= pages;

                DEBUG_MESSAGE(DEBUG, "Special case!\n");

                avl_update_tree_largest_free(node->parent);

                DEBUG_MESSAGE(DEBUG, "Exiting");

#ifdef CHECK
                avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

                // FIXME: Allocate physical page(s) here and do the mapping too.
                *page_number = node->start;
                return STORM_RETURN_SUCCESS;
            }
            else
            {
                insert_node = avl_node_allocate(process_avl_header);

                // TODO: Make the description here configurable by the caller.
                avl_node_reset(insert_node, node->start + node->busy_length, pages, node->free_length - pages, NULL,
                               "Virtual memory (allocated)");

                node->free_length = 0;

                avl_update_tree_largest_free(node->parent);

                avl_node_insert(process_avl_header, insert_node);

                DEBUG_MESSAGE(DEBUG, "Exiting");

#ifdef CHECK
                avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

                *page_number = node->start + node->busy_length;
                return STORM_RETURN_SUCCESS;
            }
        }
        else if (node->largest_free_more >= pages)
        {
            DEBUG_MESSAGE(DEBUG, "more");
            node = (avl_node_type *) node->more;
        }
        else
        {
            DEBUG_HALT("Failed to allocate a page (tried to allocate %x pages).",
                       pages);
        }
    }

    DEBUG_MESSAGE(DEBUG, "p: %p, r00t: %p, l: %u", process_avl_header,
                  process_avl_header->root, process_avl_header->limit_nodes);
    avl_debug_tree_dump(process_avl_header->root);
    DEBUG_HALT("No block found, but nodes said there was one. Tree broken.");
}

// Deallocate virtual memory.
return_type memory_virtual_deallocate(uint32_t page_number)
{
    avl_node_type *node = process_avl_header->root;
    avl_node_type *adjacent_node;
    bool finished = FALSE;

#ifdef CHECK
    avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

    while (!finished && node != NULL)
    {
        if (page_number > node->start)
        {
            node = node->more;
        }
        else if (page_number < node->start)
        {
            node = node->less;
        }
        else
        {
            finished = TRUE;
        }
    }

    if (node == NULL)
    {
        DEBUG_MESSAGE(DEBUG, "Area not allocated!");

        return STORM_RETURN_MEMORY_NOT_ALLOCATED;
    }

    // Alright. We have the node to delete. Find the "less-adjacent" node.
    if (node->less == NULL)
    {
        adjacent_node = node;

        while (adjacent_node->parent != NULL &&
                adjacent_node->parent->less == adjacent_node)
        {
            adjacent_node = adjacent_node->parent;
        }

        if (adjacent_node != NULL)
        {
            adjacent_node = adjacent_node->parent;
        }
    }
    else
    {
        adjacent_node = node->less;

        while (adjacent_node->more != NULL)
        {
            adjacent_node = adjacent_node->more;
        }
    }

    // We have the adjacent node or NULL if there was none.
    if (adjacent_node == NULL)
    {
        node->free_length += node->busy_length;
        node->busy_length = 0;

        avl_update_tree_largest_free(node->parent);
    }
    else
    {
        adjacent_node->free_length += node->free_length + node->busy_length;

        avl_update_tree_largest_free(adjacent_node->parent);

        avl_node_delete(process_avl_header, node);
    }

#ifdef CHECK
    avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

    return STORM_RETURN_SUCCESS;
}

// Reserve a region, so it won't get allocated later.
return_type memory_virtual_reserve(unsigned int start_page, unsigned int pages)
{
    avl_node_type *node = process_avl_header->root;
    avl_node_type *insert_node;

    DEBUG_MESSAGE(DEBUG, "p: %p, r00t: %p, l: %u", process_avl_header,
                  process_avl_header->root, process_avl_header->limit_nodes);
    DEBUG_MESSAGE(DEBUG, "start_page = %u, pages = %u", start_page, pages);

#ifdef CHECK
    avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

    // Main loop. Continue until the end of the tree, or until we find a match.
    while (node != NULL)
    {
        if (start_page < node->start)
        {
            // The page we're looking for is on the left side.
            node = (avl_node_type *) node->less;
        }
        else if (start_page >= (node->start + node->busy_length + node->free_length))
        {
            // It is on the right side
            node = (avl_node_type *) node->more;
        }
        else if ((start_page >= node->start + node->busy_length) &&
                 ((start_page + pages) <=
                  (node->start + node->busy_length + node->free_length)))
        {
            // It is in this entry.
            insert_node = avl_node_allocate(process_avl_header);

            // TODO: Make it possible to override the description.
            avl_node_reset(insert_node, start_page, pages,
                           node->start + node->busy_length + node->free_length - start_page - pages,
                           NULL, "Virtual memory (reserved)");

            node->free_length = start_page - node->start - node->busy_length;

            avl_update_tree_largest_free(node->parent);

            avl_node_insert(process_avl_header, insert_node);

#ifdef CHECK
            avl_debug_tree_check(process_avl_header, process_avl_header->root);
#endif

            DEBUG_MESSAGE(DEBUG, "p: %p, r00t: %p, l: %u", process_avl_header,
                          process_avl_header->root,
                          process_avl_header->limit_nodes);

            return RETURN_SUCCESS;
        }
        else
        {
            DEBUG_HALT("You tried to reserve something that was already taken (%u-%u).",
                       start_page, pages);
            return RETURN_PAGE_NOT_FOUND;
        }
    }

    // We didn't find a match. This will normally never happen.
    DEBUG_HALT("Couldn't find a match");
    return RETURN_PAGE_NOT_FOUND;
}
