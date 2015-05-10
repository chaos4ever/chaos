// Abstract: This is the startup point of storm. It is executed right after the assembly language init code has set up the GDT,
// kernel stack, etc. Here, we initialise everything in the storm, like IRQ/exception handling, the timer hardware, the memory
// facilities of the host CPU and multitasking. It is also responsible for starting the servers loaded by the Multiboot compliant
// boot loader.
//
// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1998-2000, 2007, 2013, 2015 chaos development.

// Define this as TRUE if you are debugging this part of the kernel.
#define DEBUG FALSE

#include <storm/generic/avl_debug.h>
#include <storm/generic/cpu.h>
#include <storm/generic/dataarea.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/elf.h>
#include <storm/generic/irq.h>
#include <storm/generic/limits.h>
#include <storm/generic/mailbox.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/time.h>
#include <storm/generic/types.h>

#include <storm/ia32/cpuid.h>
#include <storm/ia32/dma.h>
#include <storm/ia32/system_calls.h>
#include <storm/ia32/timer.h>
#include <storm/ia32/trap.h>
#include <storm/ia32/tss.h>

// Global variables.
bool initialised = FALSE;
dataarea_type dataarea;

// Local variables.
static u32 server_process_id INIT_DATA;
static u32 help INIT_DATA = 0;

// Struct to define kernel arguments.
typedef struct
{
    const char *name;
    bool has_value;
    u32 *variable;
} kernel_argument_type;

// This is the structure of valid kernel arguments.
static kernel_argument_type kernel_argument[] INIT_DATA =
{
#if !OPTION_RELEASE

    // Set the attributes of what is being printed from the kernel.
    { "--attribute-text", TRUE, &debug_text_attribute },
    { "--attribute-background", TRUE, &debug_background_attribute },

#endif

    // Set the sizes of the global memory. The defaults should be fine, though.
    { "--global-heap", TRUE, &limit_global_heap },
    { "--global-nodes", TRUE, &limit_global_nodes },

    // Force a different startup task switch rate.
    { "--hz", TRUE, &hz },

    // Number of entries in the mailbox hash-table.
    { "--mailbox_hash", TRUE, &limit_mailbox_hash_entries },

    // Limit system memory. Could be used if your motherboard reports bad memory values.
    // FIXME: We should have a "real" memory detection scheme too.
    { "--limit_memory", TRUE, &limit_memory },

    // Do we want a listing of available commands.
    { "--help", FALSE, &help },

    // No more parameters.
    { NULL, FALSE, NULL }
};

static void INIT_CODE parse_kernel_arguments(unsigned int arguments, char *argument[])
{
    for (unsigned int index = 0; index < arguments; index++)
    {
        bool found = FALSE;

        for (unsigned int argument_index = 0;
             kernel_argument[argument_index].name != NULL && !found;
             argument_index++)
        {
            if (string_compare(kernel_argument[argument_index].name,
                               argument[index]) == 0)
            {
                int value;

                if (kernel_argument[argument_index].has_value &&
                    arguments >= index + 1)
                {
                    index++;
                    string_to_number(argument[index], &value);
                    *kernel_argument[argument_index].variable = value;
                }
                else
                {
                    *(bool *) kernel_argument[argument_index].variable = TRUE;
                }
                found = TRUE;
            }
        }
    }
}

// This is where the fun begins. Oh, yeah, we're going to Disneyland... blah blah. Terminator 2 RULEZ!
return_type kernel_main(int arguments, char *argument[]) INIT_CODE;

return_type kernel_main(int arguments, char *argument[])
{
    // We have seen cases where these were corrupted this early, so hence these assertions. Better safe than sorry;
    // fail-fast is definitely preferable if memory regions have been corrupted somehow.
    assert(tss_tree_mutex == MUTEX_UNLOCKED, "tss_tree_mutex != MUTEX_UNLOCKED");
    assert(memory_mutex == MUTEX_UNLOCKED, "memory_mutex != MUTEX_UNLOCKED");

    int servers_started = 0;

    // Detect CPU type and flags. Must be done this early, since we want to make sure this CPU has the capabilites we require
    // (for example MMX if this is a kernel compiled for MMX).
    cpuid_init();

    parse_kernel_arguments(arguments, argument);
    debug_init();

    if (help != 0)
    {
        debug_print("Help...\n");
        cpu_halt();
    }

    memory_physical_init();

    debug_print("Starting %s (process ID %u).\n", argument[0], PROCESS_ID_KERNEL);
    debug_print("%s %s booting...\n", PACKAGE_NAME, PACKAGE_VERSION);
    debug_print("Compiled by %s on %s %s (revision %s).\n", CREATOR, __DATE__, __TIME__, REVISION);

    if (multiboot_info.has_module_info == 0 ||
        multiboot_info.number_of_modules == 0)
    {
        debug_print("No servers started. System halted.\n");
        cpu_halt();
    }

    // Prepare paging structures. Paging is not enabled yet though.
    memory_virtual_init();

    // This MUST not be executed before memory_virtual_init, since it relies on stuff that it sets up.
    trap_init();

    avl_debug_tree_check(page_avl_header, page_avl_header->root);

    timer_init();
    time_init();
    system_calls_init();

    if (cpu_info.flags.flags.tsc)
    {
        debug_print("Machine: %s at %u Hz (~%u MHz).\n",
                    parsed_cpu.name, parsed_cpu.speed,
                    parsed_cpu.speed / 1000000);
    }
    else
    {
        debug_print("Machine: %s.\n", parsed_cpu.name);
    }

    // FIXME: Report the correct memory sizes back to the enterprise.
    debug_print("Memory: Total %u MB, kernel %u KB, "
                "reserved 384 KB, free %u KB.\n",
                (memory_physical_get_number_of_pages() * SIZE_PAGE) / MB,
                (memory_physical_get_used() * SIZE_PAGE) / KB - 384,
                (memory_physical_get_free() * SIZE_PAGE) / KB);

    // Enable this if storm could not detect the cpu type, read the values and fill in the cpu type table in cpuid.c.

#if FALSE
    debug_print("CPU: Family %u, model %u.\n", cpu_info.family, cpu_info.model);
#endif

    // Set up paging and map global memory.
    memory_virtual_enable();
    debug_print("VM subsystem initialized.\n");

    memory_global_init();
    debug_print("Global memory initialized.\n");

    debug_log_enable = TRUE;

    port_init();
    debug_print("ISA-based I/O initialized.\n");

    dma_init();
    debug_print("DMA initialized.\n");

    mailbox_init();
    debug_print("IPC initialized.\n");

    // Initialise the thread code. Must be done before any threads are started.
    thread_init();
    debug_print("Thread subsystem initialized.\n");

    process_init();
    debug_print("Process subsystem initialized.\n");

    for (unsigned int index = 0; index < multiboot_info.number_of_modules; index++)
    {
        memory_virtual_map(GET_PAGE_NUMBER(BASE_MODULE),
                           GET_PAGE_NUMBER(multiboot_module_info[index].start),
                           SIZE_IN_PAGES(multiboot_module_info[index].end -
                           multiboot_module_info[index].start), PAGE_KERNEL);

        switch (elf_execute((void *) BASE_MODULE, (char *) multiboot_module_info[index].name, &server_process_id))
        {
            // The given image was not a valid IA32 ELF.
            case RETURN_ELF_UNSUPPORTED:
            {
                debug_print("Error: %s is not a supported ELF.\n",
                            multiboot_module_info[index].name);
                debug_run();
                break;
            }

            // The given image was not detected as a valid ELF image.
            case RETURN_ELF_INVALID:
            {
                debug_print("Error: %s is not ELF.\n",
                            multiboot_module_info[index].name);
                debug_run();
                break;
            }

            // If any of the ELF sections are placed outside the virtual memory space assigned to ELF sections, we get here.
            case RETURN_ELF_SECTION_MISPLACED:
            {
                debug_print("Error: %s has sections outside allowed limits.\n",
                            multiboot_module_info[index].name);
                debug_run();
                break;
            }

            // One or more of the sections was improperly page aligned.
            case STORM_RETURN_UNALIGNED_SECTION:
            {
                debug_print("Error: %s has either the .text or .data section (or both) incorrectly aligned.\n",
                            multiboot_module_info[index].name);
                debug_run();
                break;
            }

            case RETURN_ELF_SECTION_MULTIPLE_INSTANCES:
            {
              debug_print("Error: There are multiple code or data sections in the ELF binary. This is not currently supported.\n");
              debug_run();
              break;
            }

            // The ELF was successfully executed.
            case STORM_RETURN_SUCCESS:
            {
                servers_started++;
                debug_print("Started %s (process ID %u).\n",
                            multiboot_module_info[index].name, server_process_id);
                break;
            }

            default:
            {
                debug_print("Error: Failed to start server.");
                debug_run();
            }
        }
    }

    // Initial servers are started; let's deallocate the images.
    for (u32 index = 0; index < multiboot_info.number_of_modules; index++)
    {
        // FIXME: This breaks the AVL tree sometimes...
        // memory_physical_deallocate(GET_PAGE_NUMBER(multiboot_module_info[index].start));
    }

    // Initialise the dispatcher. Must be done after process initialisation.
    dispatch_init();

    // This flag can be used to check whether the system is finished booting or still in the startup phase.
    initialised = TRUE;

    // Enable the task switcher. The code following the next line will be executed when the idle task is being dispatched.
    irq_init();

    return 0;
}
