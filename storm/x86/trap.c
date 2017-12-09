// Abstract: Handle CPU exceptions (called 'traps' on motorola.. ;)
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Johan Thim <nospam@inter.net>
//          Per Lundberg <per@chaosdev.io>
//
// © Copyright 1998 chaos development

// Define this when debugging this module.
#define DEBUG FALSE

// Define this if you want to halt the system whenever an unhandled trap occurs. (Sometimes, the system will triple fault otherwise)
#define HALT_ON_ERROR

#include <storm/state.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/process.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>
#include <storm/x86/descriptor.h>
#include <storm/x86/flags.h>
#include <storm/x86/gdt.h>
#include <storm/x86/idt.h>
#include <storm/x86/trap.h>
#include <storm/x86/tss.h>

static descriptor_type trap_descriptor;
tss_type *trap_tss;
static void *trap_stack;

// Now, it's time for some exception handlers.
static void trap_divide_error_fault(void) __attribute__ ((noreturn));
static void trap_divide_error_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Divide error fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_debug_trap(void) __attribute__ ((noreturn));
static void trap_debug_trap(void)
{
    while (TRUE)
    {
        debug_crash_screen("Debug trap/fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_nmi(void) __attribute__ ((noreturn));
static void trap_nmi(void)
{
    debug_crash_screen("NMI received", current_tss);
    DEBUG_HALT("System halted.");
}

static void trap_breakpoint_trap(void) __attribute__ ((noreturn));
static void trap_breakpoint_trap(void)
{
    while (TRUE)
    {
        debug_crash_screen("Breakpoint trap", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_overflow_trap(void) __attribute__ ((noreturn));
static void trap_overflow_trap(void)
{
    while (TRUE)
    {
        debug_crash_screen("Overflow trap", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_bound_range_exceeded_fault(void) __attribute__ ((noreturn));
static void trap_bound_range_exceeded_fault(void)
{
    // FIXME: What do we do here?
    while (TRUE)
    {
        debug_crash_screen("Bound range exceeded fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_invalid_opcode_fault(void) __attribute__ ((noreturn));
static void trap_invalid_opcode_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Invalid opcode fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_device_not_available_fault(void) __attribute__ ((noreturn));
static void trap_device_not_available_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Device not available fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_double_fault(void) __attribute__ ((noreturn));
static void trap_double_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Double fault (probably due to a kernel bug)",
                           current_tss);
        cpu_halt();
    }
}

static void trap_coprocessor_segment_overrun_abort(void) __attribute__ ((noreturn));
static void trap_coprocessor_segment_overrun_abort(void)
{
    while (TRUE)
    {
        debug_crash_screen("Coprocessor segment overrun abort", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_invalid_tss_fault(void) __attribute__ ((noreturn));
static void trap_invalid_tss_fault(void)
{
    debug_crash_screen("Invalid TSS fault", current_tss);
    DEBUG_HALT("System halted.");
}

static void trap_segment_not_present_fault(void) __attribute__ ((noreturn));
static void trap_segment_not_present_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Segment not present fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_stack_fault(void) __attribute__ ((noreturn));
static void trap_stack_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Stack fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_general_protection_fault(void) __attribute__ ((noreturn));
static void trap_general_protection_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("General protection fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_page_fault(void) __attribute__ ((noreturn));
static void trap_page_fault(void)
{
    uint32_t address;

    while (TRUE)
    {
        // We need to check if this is a fault caused by the stack.
        address = cpu_get_cr2();

        // If this pagefault is caused by a growing stack, just map more memory.
        if (address >= BASE_PROCESS_STACK)
        {
            DEBUG_MESSAGE(DEBUG, "Stack is growing (was %u pages)", current_tss->stack_pages);
            if (memory_mutex == MUTEX_LOCKED)
            {
                DEBUG_MESSAGE(DEBUG, "Memory mutex was locked.");
            }
            else
            {
                uint32_t physical_page;

                mutex_kernel_wait(&memory_mutex);

                // FIXME: Check return value.
                memory_physical_allocate(&physical_page, 1, "Growing stack.");

                memory_virtual_map_other(current_tss, GET_PAGE_NUMBER(address),
                                         physical_page, 1,
                                         PAGE_WRITABLE | PAGE_NON_PRIVILEGED);
                mutex_kernel_signal(&memory_mutex);

                current_tss->stack_pages++;
                DEBUG_MESSAGE(DEBUG, "Done growing stack.");
            }
        }
        // Ugly hack to detect (some) kernel stack overruns.
        else if (address >= BASE_PROCESS_STACK - SIZE_PAGE)
        {
            debug_crash_screen("Kernel stack overrun", current_tss);
            current_tss->state = STATE_ZOMBIE;

#ifdef HALT_ON_ERROR
            debug_run();
#endif

            dispatch_next();
        }
        // ...or else, kill the thread.
        else
        {
            debug_crash_screen("Illegal page fault", current_tss);
            current_tss->state = STATE_ZOMBIE;

#ifdef HALT_ON_ERROR
            debug_run();
#endif

            dispatch_next();
        }

        asm volatile ("iret");
    }
}

static void trap_klotis_fault(void) __attribute__ ((noreturn));
static void trap_klotis_fault(void)
{
    debug_crash_screen("Buggy CPU non-fault", current_tss);
    DEBUG_HALT("System halted.");
}

static void trap_floating_point_error_fault(void) __attribute__ ((noreturn));
static void trap_floating_point_error_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Floating point error fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_alignment_check_fault(void) __attribute__ ((noreturn));
static void trap_alignment_check_fault(void)
{
    while (TRUE)
    {
        debug_crash_screen("Alignment check fault", current_tss);
        current_tss->state = STATE_ZOMBIE;
        dispatch_next();
    }
}

static void trap_machine_check_abort(void) __attribute__ ((noreturn));
static void trap_machine_check_abort(void)
{
    debug_crash_screen("Machine check abort", current_tss);
    DEBUG_HALT("System halted.");
}

// Initialise traps. (Well, exceptions really, but they're called traps on Motorola. ;)
static void trap_setup_handler(uint32_t number, tss_type *setup_tss, void *trap_pointer)
{
    // Virtual address to the TSS.
    uint32_t trap_tss_address = BASE_PROCESS_TRAP_TSS + number * 104;

    setup_tss->cr3 = (uint32_t) kernel_page_directory;

    // Set the flags to AF only.
    setup_tss->eflags = FLAG_ADJUST;

    setup_tss->eip = (uint32_t) trap_pointer;
    setup_tss->esp = BASE_TRAP_STACK + SIZE_PAGE;

    // Set up segment selectors.
    setup_tss->cs = SELECTOR_KERNEL_CODE;
    setup_tss->ss = SELECTOR_KERNEL_DATA;
    setup_tss->ds = SELECTOR_KERNEL_DATA;
    setup_tss->es = SELECTOR_KERNEL_DATA;
    setup_tss->fs = SELECTOR_KERNEL_DATA;
    setup_tss->gs = SELECTOR_KERNEL_DATA;

    // Create a descriptor for this TSS.
    trap_descriptor.limit_lo = SIZE_PAGE - 1;
    trap_descriptor.limit_hi = 0;
    trap_descriptor.granularity = 0;
    trap_descriptor.base_lo = trap_tss_address & 0xFFFF;
    trap_descriptor.base_hi = (trap_tss_address >> 16) & 0xFF;
    trap_descriptor.base_hi2 = (trap_tss_address >> 24) & 0xFF;

    trap_descriptor.type = DESCRIPTOR_TYPE_TSS;
    trap_descriptor.descriptor_type = 0;
    trap_descriptor.dpl = 0;
    trap_descriptor.segment_present = 1;
    trap_descriptor.zero = 0;
    trap_descriptor.operation_size = 0;

    // Add it to the GDT and IDT.
    gdt_add_entry(GDT_BASE_EXCEPTIONS + number, &trap_descriptor);
    idt_setup_task_gate(number, GDT(GDT_BASE_EXCEPTIONS + number, 0), 0);
}

// Initialise traps.
void trap_init(void)
{
    uint32_t physical_page;
    //  uint32_t counter;

    // Allocate a page for the trap TSS:es.
    // FIXME: Check return value.
    memory_physical_allocate(&physical_page, 1, "Trap TSS.");

    trap_tss = (void *) (physical_page * SIZE_PAGE);

    // Allocate a page for the trap stack.
    // FIXME: Check return value.
    memory_physical_allocate(&physical_page, 1, "Trap stack.");

    trap_stack = (void *) (physical_page * SIZE_PAGE);

    // Map this.
    memory_virtual_map_kernel
        (kernel_page_directory, GET_PAGE_NUMBER(BASE_PROCESS_TRAP_TSS),
        GET_PAGE_NUMBER(trap_tss), 1, PAGE_KERNEL);
    memory_virtual_map_kernel
        (kernel_page_directory, GET_PAGE_NUMBER(BASE_TRAP_STACK),
        GET_PAGE_NUMBER(trap_stack), 1, PAGE_KERNEL);

    // Wipe the TSS.
    memory_set_uint8_t((uint8_t *) trap_tss, 0, SIZE_PAGE);

    // Setup exception handlers for all exceptions.
    trap_setup_handler(0, &trap_tss[0], trap_divide_error_fault);
    trap_setup_handler(1, &trap_tss[1], trap_debug_trap);
    trap_setup_handler(2, &trap_tss[2], trap_nmi);
    trap_setup_handler(3, &trap_tss[3], trap_breakpoint_trap);
    trap_setup_handler(4, &trap_tss[4], trap_overflow_trap);
    trap_setup_handler(5, &trap_tss[5], trap_bound_range_exceeded_fault);
    trap_setup_handler(6, &trap_tss[6], trap_invalid_opcode_fault);
    trap_setup_handler(7, &trap_tss[7], trap_device_not_available_fault);
    trap_setup_handler(8, &trap_tss[8], trap_double_fault);
    trap_setup_handler(9, &trap_tss[9], trap_coprocessor_segment_overrun_abort);
    trap_setup_handler(10, &trap_tss[10], trap_invalid_tss_fault);
    trap_setup_handler(11, &trap_tss[11], trap_segment_not_present_fault);
    trap_setup_handler(12, &trap_tss[12], trap_stack_fault);
    trap_setup_handler(13, &trap_tss[13], trap_general_protection_fault);
    trap_setup_handler(14, &trap_tss[14], trap_page_fault);
    trap_setup_handler(15, &trap_tss[15], trap_klotis_fault);
    trap_setup_handler(16, &trap_tss[16], trap_floating_point_error_fault);
    trap_setup_handler(17, &trap_tss[17], trap_alignment_check_fault);
    trap_setup_handler(18, &trap_tss[18], trap_machine_check_abort);
}
