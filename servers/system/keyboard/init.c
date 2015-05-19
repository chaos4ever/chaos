// Abstract: Keyboard initialisation code.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#include "config.h"
#include "common.h"
#include "controller.h"
#include "init.h"
#include "keyboard.h"
#include "mouse.h"

// Initialise the server.
bool init(void)
{
    const char *message;

    memory_init();
    system_process_name_set(PACKAGE_NAME);

    /* Initialise a connection to the log service. */
    //  log_init (&log_structure, PACKAGE_NAME);

    if (system_call_port_range_register(CONTROLLER_PORT_BASE, CONTROLLER_PORTS, "Keyboard controller") != STORM_RETURN_SUCCESS)
    {
        //    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
        //               "Could not allocate portrange 0x60 - 0x6F.");
        return FALSE;
    }

    // Flush any pending input.
    keyboard_clear_input();

    message = keyboard_init();
    if (message != NULL)
    {
        //log_print (&log_structure, LOG_URGENCY_ERROR, message);
    }

    // Initialise a PS/2 mouse port, if found.
    mouse_init();

    return TRUE;
}

int main(void)
{
    system_process_name_set("keyboard");
    system_thread_name_set("Initialising");

    // Detect whether a keyboard and/or mouse is present, and if so, put them into a usable state.

    if (!init())
    {
        //    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
        //               "Failed initialisation.");
        return 0;
    }

    //  log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
    //             "Keyboard found at I/O 0x60-0x6F, IRQ 1.");

    if (has_mouse)
    {
        //    log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
        //               "Mouse found at IRQ 12.");
    }

    // Handle the IRQs.
    if (system_thread_create() == SYSTEM_RETURN_THREAD_NEW)
    {
        keyboard_irq_handler();
    }

    if (has_mouse && system_thread_create() == SYSTEM_RETURN_THREAD_NEW)
    {
        mouse_irq_handler();
    }

    // Handle the services.
    if (system_thread_create() == SYSTEM_RETURN_THREAD_NEW)
    {
        if (keyboard_main())
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    if (has_mouse && system_thread_create() == SYSTEM_RETURN_THREAD_NEW)
    {
        mouse_main();
    }

    system_call_process_parent_unblock();

    return 0;
}
