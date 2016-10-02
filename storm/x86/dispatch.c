// Abstract: Functions for the task dispatcher.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>

// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

// Define this as TRUE if you want *lots* of debug information.
#define DEBUG FALSE

// If you are debugging the idle task, define this. It will print some fancy stuff in the corners of the screen.
#undef DEBUG_IDLE

#include <storm/state.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/irq.h>
#include <storm/generic/limits.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/thread.h>
#include <storm/generic/time.h>
#include <storm/generic/types.h>

#include <storm/x86/timer.h>
#include <storm/x86/tss.h>
#include <storm/x86/gdt.h>

volatile uint8_t dispatch_task_flag = 0;

volatile process_id_type current_process_id = PROCESS_ID_KERNEL;
volatile cluster_id_type current_cluster_id = CLUSTER_ID_KERNEL;
volatile thread_id_type current_thread_id = THREAD_ID_KERNEL;
volatile time_type timeslice = 0;
storm_tss_type *current_tss = (storm_tss_type *) BASE_VIRTUAL_KERNEL_TSS;
uint32_t jump_data[2] = { 0, 0 };
tss_list_type *current_tss_node;

void dispatch_init(void)
{
    assert(idle_tss_node != NULL, "idle_tss_node == NULL");
    current_tss_node = idle_tss_node;
}

static void timers_update(void)
{
    timeslice++;
    irq[0].occurred++;

    if ((timeslice % hz) == 0)
    {
        uptime++;
        time++;
    }
}

// Updates the data tables used by the assembly dispatcher code.
// FIXME: Get rid of these 0 and 1 return values.
static int update_data(void)
{
    uint32_t counter = 0;
    tss_list_type *tss_node = current_tss_node;
#ifdef DEBUG_IDLE
    screen_type *screen = (screen_type *) BASE_SCREEN;

    screen[0].attribute++;
#endif

    // Increase the task pointers and create new GDT descriptors so that we can call the next task.
    // FIXME: Do something like this:
#if FALSE
    if (uptime == event->first_event->time)
    {
        handle_event(event->first_event);
        event->first_event = event->first_event->next;
    }
#endif

    assert(tss_node != NULL, "tss_node == NULL");
    assert(tss_list != NULL, "tss_list == NULL");

    // Search for the next task waiting to be dispatched.
    do
    {
        if (counter == number_of_tasks)
        {
            // There are no active threads. Let the idle thread run.
            DEBUG_MESSAGE(DEBUG, "Dispatching idle thread");
            tss_node = idle_tss_node;
#ifdef DEBUG_IDLE
            screen[40].attribute++;
#endif
            break;
        }

#ifdef DEBUG_IDLE
        screen[20].attribute++;
#endif

        counter++;
        tss_node = (tss_list_type *) tss_node->next;
        if (tss_node == NULL)
        {
            tss_node = tss_list;
        }
    } while (tss_node->tss->state != STATE_DISPATCH);

    // We have found a thread to run; increase the timeslice counter for thread.
    tss_node->tss->timeslices++;

    // Make sure we don't try to task switch recursively.
    if (current_tss_node == tss_node)
    {
        DEBUG_MESSAGE(DEBUG, "No new threads. Keeping up with thread %u (state %u)",
                      current_thread_id, tss_node->tss->state);

        return 1;
    }

#if FALSE
    if (tss_node == idle_tss_node)
    {
        tss_list_type *node = tss_list;

        DEBUG_MESSAGE(TRUE, "Dumping list of threads.");

        while (node != NULL)
        {
            debug_print("TSS %p, Process %s, TID %u, state %u, dispatched %u\n",
                        node->tss, node->tss->process_name, node->thread_id,
                        node->tss->state, node->tss->timeslices);

            node = (tss_list_type *) node->next;
        }
    }
#endif

    current_tss_node = tss_node;
    current_tss = current_tss_node->tss;

    assert(current_tss->eip >= BASE_KERNEL &&
           current_tss->eip <= BASE_PROCESS_SPACE_END, "EIP outside allowed range for processes. Possible TSS corruption?");

    // Update information about the current process.
    current_process_id = current_tss->process_id;
    current_cluster_id = current_tss->cluster_id;
    current_thread_id = current_tss->thread_id;

    DEBUG_MESSAGE(DEBUG, "Switching to PID %u, CID %u, TID %u, EIP %x",
                  current_process_id, current_cluster_id, current_thread_id,
                  current_tss->eip);

    // We use two entries in the GDT, and switch between them. For this to work we need a flag telling us which entry to use.
    dispatch_task_flag ^= 1;

    switch (dispatch_task_flag)
    {
        case 0:
        {
            gdt_setup_tss_descriptor(SELECTOR_TSS1 >> 3, current_tss, 3,
                                     sizeof (storm_tss_type) +
                                     current_tss->iomap_size);
            jump_data[1] = SELECTOR_TSS1;
            break;
        }

        case 1:
        {
            gdt_setup_tss_descriptor(SELECTOR_TSS2 >> 3, current_tss, 3,
                                     sizeof (storm_tss_type) +
                                     current_tss->iomap_size);
            jump_data[1] = SELECTOR_TSS2;
            break;
        }
    }

    return 0;
}

// Release the rest of this time slice so that we can go on task switching.
void dispatch_next(void)
{
#if !OPTION_RELEASE
    if (tss_tree_mutex == MUTEX_LOCKED)
    {
        uint32_t esp __attribute__ ((unused)) = cpu_get_esp();
        DEBUG_MESSAGE(TRUE, "Called from %p", ((uint32_t *) esp)[7]);
        DEBUG_HALT("We was called when the TSS tree mutex was locked. This should not happen.");
    }
#endif

    // Make sure we don't get aborted. mutex_kernel_wait can not be used.
    // FIXME: This won't always work...
    tss_tree_mutex = MUTEX_LOCKED;

    cpu_interrupts_disable();

    if (update_data() == 0)
    {
        // There is at least one more task that should be executed, so let's do it.
        mutex_kernel_signal(&tss_tree_mutex);
        dispatch();
        cpu_interrupts_enable();
    }
    else
    {
        // No other thread to run. We need to make _sure_ to yield the CPU in this case; otherwise we will always consume 100%
        // CPU, even when we are completely idle.
        mutex_kernel_signal(&tss_tree_mutex);
        cpu_interrupts_enable();
        asm("hlt");
    }
}

// Update dispatcher information. Called from the IRQ 0 handler.
int dispatch_update(void)
{
    timers_update();

    // If the dispatcher is locked, we can not do reliable task switching.
    if (tss_tree_mutex == MUTEX_LOCKED ||
        mutex_spinlock == MUTEX_SPIN_LOCKED)
    {
        current_tss->timeslices++;

        return 1;
    }

    // Check the timers.
    timer_check_events();

    return update_data();
}

// This is the task switcher function itself. It is the IRQ 0 handler.
void dispatch_task_switcher(void)
{
    // I had to split this to make it work. Otherwise, gcc would trash eax, which is pretty bad, to say the least. :-)
    asm volatile ("pusha");
    asm volatile
    ("movl        32(%%esp), %%eax"
     : "=a" (current_tss->instruction_pointer)
     : "m" (current_tss->instruction_pointer));

    asm volatile
    (
        // Call the dispatch helper function.
        "call        dispatch_update\n"

        "movb        %al, %bl\n"

        "movb        $0x20, %al\n"
        "outb        %al, $0x20\n"

        "cmpb        $1, %bl\n"
        "je          1f\n"

        // Dispatch the new task.
        "ljmp        *jump_data\n"

        "1:  popa\n"
        "iret"
    );

    // Just to make gcc happy, so that it will know that this function will never return...
    while (TRUE);
}

// This function is called from system_call_dispatch_next ().
void dispatch(void)
{
    // This dispatches the new task.
    // FIXME: Use constraints.
    asm ("ljmp	*jump_data");
}
