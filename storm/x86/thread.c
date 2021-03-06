// Abstract: Thread routines. Part of the process system. Responsible for adding and removing threads under a cluster.
// Authors: Henrik Hallin <hal@chaosdev.org>,
//          Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

// FIXME: Move most of this to the generic subdirectory.

// Define this as TRUE when debugging this part of the kernel.
#define DEBUG FALSE

#include <storm/state.h>

#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <storm/generic/mutex.h>

#include <storm/x86/gdt.h>
#include <storm/x86/timer.h>
#include <storm/x86/tss.h>

static tss_list_type **tss_hash;
mutex_kernel_type tss_tree_mutex = MUTEX_UNLOCKED;

volatile uint32_t number_of_tasks = 0;
storm_tss_type *kernel_tss;

// A linked list of all the threads in the system. Used by the dispatcher.
tss_list_type *tss_list = NULL;
tss_list_type *idle_tss_node = NULL;

// The lowest free thread ID.
volatile thread_id_type free_thread_id = 1;

// Gets the hash value for the given mailbox ID.
static unsigned int hash(thread_id_type thread_id)
{
    return (thread_id % limit_thread_hash_entries);
}

// Initialise the thread code.
void thread_init(void)
{
    tss_hash = memory_global_allocate(sizeof (tss_list_type *) * limit_thread_hash_entries);
    memory_set_uint8_t((uint8_t *) tss_hash, 0, sizeof (tss_list_type *) * limit_thread_hash_entries);
}

// Links the given TSS into the TSS list.
tss_list_type *thread_link_list(tss_list_type **list, storm_tss_type *tss)
{
    tss_list_type *tss_list_node;

    DEBUG_MESSAGE(DEBUG, "list = %p", list);

    // Add this entry into the list.
    tss_list_node = memory_global_allocate(sizeof(tss_list_type));
    assert(tss_list_node != NULL, "memory_global_allocate returned a NULL pointer.");

    tss_list_node->thread_id = tss->thread_id;
    tss_list_node->next = (struct tss_list_type *) (*list);
    tss_list_node->previous = NULL;
    tss_list_node->tss = tss;

    // If this is the first entry, presume it is the idle thread. A little ugly... but it works. ;)
    if (idle_tss_node == NULL)
    {
        idle_tss_node = tss_list_node;
    }

    if (*list != NULL)
    {
        (*list)->previous = (struct tss_list_type *) tss_list_node;
    }

    *list = tss_list_node;

    return tss_list_node;
}

// Links the given TSS into the TSS hash and list.
tss_list_type *thread_link(storm_tss_type *tss)
{
    unsigned int entry = hash(tss->thread_id);

    thread_link_list(&tss_hash[entry], tss);

    return thread_link_list(&tss_list, tss);
}

// Unlinks the given entry in the given TSS list.
void thread_unlink_list(tss_list_type **list, thread_id_type thread_id)
{
    tss_list_type *tss_list_node = *list;
    tss_list_type *previous, *next;

    DEBUG_MESSAGE(DEBUG, "Unlinking %u", thread_id);

    // Find our boy in the list.
    while (tss_list_node->thread_id != thread_id &&
           tss_list_node != NULL)
    {
        tss_list_node = (tss_list_type *) tss_list_node->next;
    }

    if (tss_list_node == NULL)
    {
        DEBUG_HALT("Tried to unlink a thread which did not exist.");
    }

    previous = (tss_list_type *) tss_list_node->previous;
    next = (tss_list_type *) tss_list_node->next;

    if (previous == NULL)
    {
        *list = next;
    }
    else
    {
        previous->next = (struct tss_list_type *) next;
    }

    if (next != NULL)
    {
        next->previous = (struct tss_list_type *) previous;
    }

    // Let's deallocate some memory.
    memory_global_deallocate(tss_list_node);
}

// Unlinks the TSS with the given thread ID from the TSS tree and list.
void thread_unlink(thread_id_type thread_id)
{
    tss_list_type *tss_list_node = tss_list;
    unsigned int entry = hash(thread_id);
    tss_list_type *previous, *next;

    thread_unlink_list(&tss_list, thread_id);

    tss_list_node = tss_hash[entry];
    while (tss_list_node->thread_id != thread_id)
    {
        tss_list_node = (tss_list_type *) tss_list_node->next;
    }

    previous = (tss_list_type *) tss_list_node->previous;
    next = (tss_list_type *) tss_list_node->next;

    // Was this the first entry in the list? If so, update the entry in the hash table.
    if (previous == NULL)
    {
        tss_hash[entry] = next;
    }
    else
    {
        previous->next = (struct tss_list_type *) next;
    }

    if (next != NULL)
    {
        next->previous = (struct tss_list_type *) previous;
    }

    // Let's deallocate some memory.
    memory_global_deallocate(tss_list_node);
}

// Get the TSS address for the thread with the given ID. This function presumes that the tree is already locked.
storm_tss_type *thread_get_tss(thread_id_type thread_id)
{
    unsigned int entry = hash(thread_id);
    tss_list_type *node = tss_hash[entry];

    while (node != NULL)
    {
        if (node->thread_id == thread_id)
        {
            return node->tss;
        }

        node = (tss_list_type *) node->next;
    }

    DEBUG_HALT("Thread didn't exist in the data storage.");
}

// Get the first available thread ID.
// FIXME: Implement free-ID-pool.
thread_id_type thread_get_free_id(void)
{
    thread_id_type thread_id;

    thread_id = free_thread_id++;
    DEBUG_MESSAGE(DEBUG, "thread_id = %u", thread_id);

    return thread_id;
}

// Create a thread under the current cluster. Returns STORM_RETURN_THREAD_ORIGINAL for the calling thread and
// STORM_RETURN_THREAD_NEW for new thread.

// FIXME: Some regions used by the kernel for temporary mappings need to be unique (or mutexed) to each thread under
// a cluster. Discuss how this is best done! Right now, we lock everything, which is sub-optimal.
return_type thread_create(uint32_t current_thread_esp, void *(*start_routine) (void *), void *argument)
{

    storm_tss_type *new_tss;
    uint32_t stack_physical_page, page_directory_physical_page, system_page_table_physical_page;
    int index;
    process_info_type *process_info;

    cpu_interrupts_disable();
    mutex_kernel_wait(&memory_mutex);
    mutex_kernel_wait(&tss_tree_mutex);

    // FIXME: Check return value.
    memory_physical_allocate(&page_directory_physical_page, 1, "Thread page directory.");
    memory_physical_allocate(&system_page_table_physical_page, 1, "Thread page table (system pages)");

    // Map the page directory and the lowest page table.
    memory_virtual_map(GET_PAGE_NUMBER(BASE_PROCESS_TEMPORARY), page_directory_physical_page, 1, PAGE_KERNEL);
    memory_virtual_map(GET_PAGE_NUMBER(BASE_PROCESS_TEMPORARY) + 1, system_page_table_physical_page, 1, PAGE_KERNEL);
    page_directory_entry_page_table *new_page_directory = (page_directory_entry_page_table *) BASE_PROCESS_TEMPORARY;
    page_table_entry *system_page_table = (page_table_entry *) (BASE_PROCESS_TEMPORARY + SIZE_PAGE);

    // Allocate memory for a TSS.
    new_tss = ((storm_tss_type *) memory_global_allocate(sizeof (storm_tss_type) + current_tss->iomap_size));

    // Clone the TSS.
    memory_copy((uint8_t *) new_tss, (uint8_t *) current_tss, sizeof (storm_tss_type) + current_tss->iomap_size);

    // FIXME: tss_tree_mutex should be changed to a 'dispatcher_mutex', or something... This looks a little weird if
    // you don't know why it's written this way.

    new_tss->thread_id = thread_get_free_id();

    // What has changed in the TSS is the ESP/ESP0 and the EIP. We must update those fields.
    new_tss->eip = (uint32_t) start_routine;
    new_tss->cr3 = page_directory_physical_page * SIZE_PAGE;

    //  debug_print ("thread: %u\n", new_tss->thread_id);

    // Clone the page directory and the lowest page table.
    memory_copy((uint8_t *) new_page_directory, (uint8_t *) BASE_PROCESS_PAGE_DIRECTORY, SIZE_PAGE);
    memory_copy((uint8_t *) system_page_table, (uint8_t *) BASE_PROCESS_PAGE_TABLES, SIZE_PAGE);

    // Set the stack as non-present.
    // FIXME: defines.
    for (index = 1024 - 16; index < 1024; index++)
    {
        new_page_directory[index].present = 0;
    }

    // Map the thread's page directory and update the mapping for the first pagetable.
    new_page_directory[0].page_table_base = system_page_table_physical_page;
    memory_virtual_map_other(new_tss,
                             GET_PAGE_NUMBER(BASE_PROCESS_PAGE_DIRECTORY),
                             page_directory_physical_page, 1, PAGE_KERNEL);

    // The "magical" mapping previously created by memory_virtual_create_page_tables_mapping also
    // needs to be adjusted. Otherwise we will update the wrong thread's addressing space when
    // mapping memory for the new thread.
    memory_virtual_create_page_tables_mapping(new_page_directory, page_directory_physical_page);

    // FIXME: Map into all sister threads address spaces when creating a new page table for a thread.

    // Start by creating a PL0 stack, and map it into our current context so we can clear it.
    // FIXME: Check return values; all of these can fail.
    memory_physical_allocate(&stack_physical_page, 1, "Thread PL0 stack.");
    memory_virtual_map(GET_PAGE_NUMBER(BASE_PROCESS_CREATE), stack_physical_page, 1, PAGE_KERNEL);
    memory_set_uint8_t((uint8_t *) BASE_PROCESS_CREATE, 0, SIZE_PAGE);

    // Make the PL0 stack accessible in the new thread's context. Its virtual memory address range
    // is 0xFC000000-0xFC000FF.
    memory_virtual_map_other(new_tss, GET_PAGE_NUMBER(BASE_PROCESS_STACK), stack_physical_page, 1,
                             PAGE_KERNEL);

    // PL0 stack is now set up. We now need to set up the PL3 stack also, which is slightly more complicated.
    // All the content of the current thread's stack gets copied to the stack for the new thread, so that stack
    // variables set in the mother thread will have expected values in the child thread also.

    // FIXME: Check return values.
    memory_physical_allocate(&stack_physical_page, current_tss->stack_pages, "Thread PL3 stack.");
    memory_virtual_map(GET_PAGE_NUMBER(BASE_PROCESS_CREATE), stack_physical_page, current_tss->stack_pages,
                       PAGE_KERNEL);
    memory_copy((uint8_t *) BASE_PROCESS_CREATE,
                (uint8_t *) ((MAX_PAGES - current_tss->stack_pages) * SIZE_PAGE),
                current_tss->stack_pages * SIZE_PAGE);
    memory_virtual_map_other(new_tss, MAX_PAGES - current_tss->stack_pages, stack_physical_page,
                             current_tss->stack_pages, PAGE_WRITABLE | PAGE_NON_PRIVILEGED);

    new_tss->esp = current_thread_esp;

    // 0xFFF = 4095, so we get the offset of the stack pointer within its current page.
    int offset = (new_tss->esp & 0xFFF);

    if (offset < 8)
    {
        // FIXME: This algorithm has a serious flaw. If the current_thread_esp has LESS than 8 bytes left until
        // it reaches the next page boundary, the stack operations below will fail. It is indeed an edge case,
        // but clearly something that's very likely to happen at some point. We need to special case it here,
        // or re-think the algorithm in some other way.
        DEBUG_HALT("Kernel bug encountered. %d is less than 8.", offset);
    }

    // Place the thread's optional parameter into its stack. We utilize the fact here that the last stack page
    // will always be mapped at BASE_PROCESS_CREATE, since x86 stacks grow downwards => "the last shall be
    // first."
    uint32_t new_stack_in_current_address_space = BASE_PROCESS_CREATE + offset;
    new_tss->esp -= 4;
    new_stack_in_current_address_space -= 4;
    *(void **)new_stack_in_current_address_space = argument;

    // FIXME: Make the return address here be to a thread_exit() method or similar. As it is now,
    // returning from a thread will trigger a page fault. It's not great, but it's a lot better
    // than leaving it as a wild pointer into an undefined location in memory.
    new_tss->esp -= 4;
    new_stack_in_current_address_space -= 4;
    *(void **)new_stack_in_current_address_space = NULL;

    new_tss->ss = new_tss->ss0;
    new_tss->cs = SELECTOR_KERNEL_CODE;
    new_tss->eflags = THREAD_NEW_EFLAGS;
    new_tss->timeslices = 0;
    string_copy(new_tss->thread_name, "unnamed");

    process_info = (process_info_type *) new_tss->process_info;
    thread_link_list(&process_info->thread_list, new_tss);
    thread_link(new_tss);
    number_of_tasks++;
    process_info->number_of_threads++;

    mutex_kernel_signal(&tss_tree_mutex);
    mutex_kernel_signal(&memory_mutex);
    cpu_interrupts_enable();

    return STORM_RETURN_SUCCESS;
}

// Delete a thread. If all threads under a cluster are deleted, the cluster is removed. If all clusters under a process
// is deleted, the whole process is deleted.
//
// This function takes for granted that tss_array_mutex is already locked.
static return_type thread_delete(storm_tss_type *tss)
{
    process_info_type *process_info = (process_info_type *) tss->process_info;

    // Free IRQs we might have allocated.
    irq_free_all(tss->thread_id);

    // Free port ranges possibly allocated.
    // FIXME: This doesn't seem to really work...
    // port_range_free_all (thread_id);

    thread_unlink_list(&process_info->thread_list, tss->thread_id);
    process_info->number_of_threads--;

    tss->state = STATE_ZOMBIE;

    if (process_info->number_of_threads == 0)
    {
        process_parent_unblock();
    }

    // FIXME: Add this thread to a list of threads that the idle thread
    // should delete, and implement this in the idle thread. For now,
    // all resources used by the thread is leaked and nothing handles the
    // case where all threads are terminated and the whole process should
    // be removed from the system.
    return RETURN_SUCCESS;
}

// Control the state of a thread.
return_type thread_control(thread_id_type thread_id, unsigned int class, unsigned int parameter __attribute__ ((unused)))
{
    storm_tss_type *tss;

    // Find out the task number for this thread. We need to lock the TSS tree mutex during this, to make sure the task
    // ID:s won't get changed.
    mutex_kernel_wait(&tss_tree_mutex);
    tss = thread_get_tss(thread_id);
    if (tss == NULL)
    {
        mutex_kernel_signal(&tss_tree_mutex);

        return STORM_RETURN_INVALID_ARGUMENT;
    }

    switch (class)
    {
        // Terminate the target thread.
        case THREAD_TERMINATE:
        {
            // If we try to kill the kernel, reboot.
            if (thread_id == THREAD_ID_KERNEL)
            {
                cpu_reset();
            }

            thread_delete(tss);

            // If this is the current thread, call the dispatcher so that we get out of the way as soon as possible.
            if (tss->thread_id == current_tss->thread_id)
            {
                mutex_kernel_signal(&tss_tree_mutex);
                dispatch_next();
            }

            break;
        }

        // Put a thread to sleep.
        case THREAD_SLEEP:
        {
            // #undef DEBUG
            // #define DEBUG TRUE
            DEBUG_MESSAGE(DEBUG, "Putting TID %u to sleep for %u milliseconds", thread_id, parameter);
            // #undef DEBUG
            // #define DEBUG FALSE

            tss->state = STATE_SLEEP;

            // Add an event timer to start the thread again.
            timer_add_event(parameter, TIMER_EVENT_THREAD_WAKEUP, tss);

            // Give up the rest of the time-slice.
            if (thread_id == current_thread_id)
            {
                mutex_kernel_signal(&tss_tree_mutex);
                dispatch_next();

                return STORM_RETURN_SUCCESS;
            }

            break;
        }
    }

    mutex_kernel_signal(&tss_tree_mutex);

    return STORM_RETURN_SUCCESS;
}

// FIXME: Optimise this whole block/unblock stuff, perhaps with a linked list or whatever...
// Block a thread on a kernel mutex.
void thread_block_kernel_mutex(storm_tss_type *tss, mutex_kernel_type *mutex_kernel)
{
#if FALSE
    uint32_t esp = cpu_get_esp();

    DEBUG_MESSAGE(DEBUG, "Called from %p", ((uint32_t *) esp)[8]);
#endif

    // FIXME: Security.
    DEBUG_MESSAGE(DEBUG, "Called");
    DEBUG_MESSAGE(DEBUG, "Blocking %x", mutex_kernel);

    tss->state = STATE_MUTEX_KERNEL;
    tss->mutex_kernel = mutex_kernel;
    tss->mutex_time = timeslice;

    if (tss->thread_id == current_tss->thread_id)
    {
        dispatch_next();
    }
}

// Unblock a thread which is blocked on a user mutex.
return_type thread_unblock_kernel_mutex(mutex_kernel_type *mutex_kernel)
{
    time_type lowest_time = MAX_TIME;
    storm_tss_type *lowest_tss = NULL;
    tss_list_type *tss_node;

    DEBUG_MESSAGE(DEBUG, "Called");

    // FIXME: We can't do this with a mutex. So how do we do?
    //  while (tss_tree_mutex == MUTEX_LOCKED);

    tss_node = tss_list;

    while (tss_node != NULL)
    {
        if (tss_node->tss->state == STATE_MUTEX_KERNEL &&
            tss_node->tss->mutex_time < lowest_time &&
            tss_node->tss->mutex_kernel == mutex_kernel)
        {
            lowest_tss = tss_node->tss;
            lowest_time = tss_node->tss->mutex_time;
        }

        tss_node = (tss_list_type *) tss_node->next;
    }

    if (lowest_tss != NULL)
    {
        lowest_tss->state = STATE_DISPATCH;

        return RETURN_THREAD_UNBLOCKED;
    }
    else
    {
        return RETURN_NO_THREAD_UNBLOCKED;
    }
}

// FIXME: Those two could be put into one.

// Unblock a thread which is blocked on a mailbox send.
// FIXME: Unblock the oldest thread. This could lead to starvation.
void thread_unblock_mailbox_send(mailbox_id_type mailbox_id)
{
    tss_list_type *tss_node;

    tss_node = tss_list;

    while (tss_node != NULL)
    {
        if (tss_node->tss->state == STATE_MAILBOX_SEND &&
            tss_node->tss->mailbox_id == mailbox_id)
        {
            tss_node->tss->state = STATE_DISPATCH;
            break;
        }

        tss_node = (tss_list_type *) tss_node->next;
    }
}

// Unblock a thread which is blocked on a mailbox receive.
void thread_unblock_mailbox_receive(mailbox_id_type mailbox_id)
{
    tss_list_type *tss_node;

    tss_node = tss_list;

    DEBUG_MESSAGE(DEBUG, "Unlocking thread locked on mailbox ID %u", mailbox_id);

    while (tss_node != NULL)
    {
        if (tss_node->tss->state == STATE_MAILBOX_RECEIVE &&
            tss_node->tss->mailbox_id == mailbox_id)
        {
            DEBUG_MESSAGE(DEBUG, "Unlocking thread %u", tss_node->thread_id);
            tss_node->tss->state = STATE_DISPATCH;
            break;
        }

        tss_node = (tss_list_type *) tss_node->next;
    }
}
