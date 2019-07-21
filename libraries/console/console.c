// Abstract: Console library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

// See The chaos Programming Reference Manual for more information about the functions in this library.

#include <stdarg.h>
#include <console/console.h>
#include <ipc/ipc.h>
#include <memory/memory.h>
#include <string/string.h>

#define CONSOLE_MAILBOX_SIZE    32768

// Initialise a connection between the application and the console service.
return_type console_init(console_structure_type *console_structure, tag_type *tag, unsigned int connection_class)
{
    mailbox_id_type mailbox_id[10];
    unsigned int services = 10;
    message_parameter_type message_parameter;

    memory_set_uint8_t((uint8_t *) console_structure, 0, sizeof(console_structure_type));

    console_structure->initialised = FALSE;

    if (ipc_service_resolve("console", mailbox_id, &services, 5, tag) != IPC_RETURN_SUCCESS)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    console_structure->ipc_structure.output_mailbox_id = mailbox_id[0];

    // Connect to this service.
    if (ipc_service_connection_request(&console_structure->ipc_structure) != IPC_RETURN_SUCCESS)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    console_structure->initialised = TRUE;

    // Send the connection class.
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = IPC_CONSOLE_CONNECTION_CLASS_SET;
    message_parameter.data = &connection_class;
    message_parameter.length = sizeof (unsigned int);
    message_parameter.block = TRUE;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}

// Allocate and open a new console with the specified attributes.
return_type console_open(console_structure_type *console_structure,
                         ipc_console_attribute_type console_attribute)
{
    if (!console_structure->initialised)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    message_parameter_type message_parameter;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.length = sizeof (ipc_console_attribute_type);
    message_parameter.message_class = IPC_CONSOLE_OPEN;
    message_parameter.data = (void *) &console_attribute;
    message_parameter.block = TRUE;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    if (console_attribute.enable_buffer)
    {
        if (console_attribute.mode_type == VIDEO_MODE_TYPE_TEXT)
        {
            memory_allocate((void **) &console_structure->buffer,
                            console_attribute.width *
                            console_attribute.height *
                            sizeof(console_character_type));

            // Ensure the newly allocated buffer is cleared. Note: we clear with zeroes instead 0f
            // 0x0700 here (as is done on regular consoles by the console server), to deliberately
            // avoid showing the cursor. Double-buffered applications are unlikely to want to
            // display a text cursor anyway.
            memory_set_uint16_t((uint16_t *) console_structure->buffer, 0,
                                console_attribute.width * console_attribute.height);
        }
        else
        {
            // Double-buffering is currently only supported for text-mode consoles.
            return CONSOLE_RETURN_INVALID_ARGUMENT;
        }
    }

    console_structure->opened = TRUE;
    console_structure->width = console_attribute.width;
    console_structure->height = console_attribute.height;

    return CONSOLE_RETURN_SUCCESS;
}

// Change the attributes of the current console.
return_type console_mode_set(console_structure_type *console_structure, unsigned int width,
                             unsigned int height, unsigned int depth, int mode_type)
{
    if (!console_structure->initialised)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    ipc_console_attribute_type console_attribute;
    console_attribute.width = width;
    console_attribute.height = height;
    console_attribute.depth = depth;
    console_attribute.mode_type = mode_type;

    message_parameter_type message_parameter;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.length = sizeof (ipc_console_attribute_type);
    message_parameter.message_class = IPC_CONSOLE_MODE_SET;
    message_parameter.data = (void *) &console_attribute;
    message_parameter.block = TRUE;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}

// Change the size (text rows and columns, not mode!) of the current console.
return_type console_resize(console_structure_type *console_structure, unsigned int width,
                           unsigned int height)
{
    if (!console_structure->initialised)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    ipc_console_resize_type ipc_console_resize;
    ipc_console_resize.width = width;
    ipc_console_resize.height = height;

    message_parameter_type message_parameter;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.length = sizeof (ipc_console_resize_type);
    message_parameter.message_class = IPC_CONSOLE_RESIZE;
    message_parameter.data = (void *) &ipc_console_resize;
    message_parameter.block = TRUE;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}

// Tell the console server that we want input from the keyboard sent to us.
return_type console_use_keyboard(console_structure_type *console_structure, bool which, int type)
{
    message_parameter_type message_parameter;

    // Shall we use the keyboard at all?

    message_parameter.block = TRUE;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = which ? IPC_CONSOLE_ENABLE_KEYBOARD :
                                              IPC_CONSOLE_DISABLE_KEYBOARD;
    message_parameter.length = 0;
    message_parameter.data = NULL;
    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    // Extended or normal target?
    switch (type)
    {
        case CONSOLE_KEYBOARD_NORMAL:
        {
            message_parameter.message_class = IPC_CONSOLE_KEYBOARD_NORMAL;
            break;
        }

        case CONSOLE_KEYBOARD_EXTENDED:
        {
            message_parameter.message_class = IPC_CONSOLE_KEYBOARD_EXTENDED;
            break;
        }

        default:
        {
            return CONSOLE_RETURN_INVALID_ARGUMENT;
        }
    }

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}

// Tell the console server that we want input from the mouse sent to us.
return_type console_use_mouse(console_structure_type *console_structure, bool which)
{
    message_parameter_type message_parameter;

    message_parameter.block = TRUE;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = which ? IPC_CONSOLE_ENABLE_MOUSE :
                                              IPC_CONSOLE_DISABLE_MOUSE;
    message_parameter.length = 0;
    message_parameter.data = NULL;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}

// Print a string to the console service.
return_type console_print(console_structure_type *console_structure, const char *string)
{
    message_parameter_type message_parameter;

    if (!console_structure->initialised)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.length = string_length(string) + 1;
    message_parameter.message_class = IPC_CONSOLE_OUTPUT;
    message_parameter.data = (void *) string;
    message_parameter.block = TRUE;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}

return_type console_print_formatted(console_structure_type *console_structure, const char *format_string, ...)
{
    va_list arguments;

    // FIXME: Don't have a hardwired buffer like this!
    char output[1024];

    if (format_string == NULL)
    {
        output[0] = '\0';

        return CONSOLE_RETURN_INVALID_ARGUMENT;
    }

    va_start(arguments, format_string);
    string_print_va(output, format_string, arguments);
    va_end(arguments);

    return console_print(console_structure, output);
}

return_type console_clear(console_structure_type *console_structure)
{
    return console_print(console_structure, "\e[2J\e[1;1H");
}

return_type console_cursor_move(console_structure_type *console_structure, unsigned int x, unsigned int y)
{
    // FIXME: Should store the size of the console somewhere.
    if (x > 1000 || y > 1000)
    {
        return CONSOLE_RETURN_INVALID_ARGUMENT;
    }
    else
    {
        return console_print_formatted(console_structure, "\e[%d;%dH", x + 1, y + 1);
    }
}

// Set the attributes.
// FIXME: There should be a function for each of the attributes.
return_type console_attribute_set(console_structure_type *console_structure, int foreground, int background, int attributes)
{
    return console_print_formatted(console_structure, "\e[%d;%d;%dm", attributes,
                                   foreground + 30,
                                   background + 40);
}

// Set the cursor appearance.
return_type console_cursor_appearance_set(console_structure_type *console_structure, bool visibility, bool block)
{
    // FIXME: Add ANSI commands for setting the console appearance. (We can not really _add_ ANSI commands, can we? ;)

    if (!visibility)
    {
        // Hide the cursor.
    }
    else if (block)
    {
        // Make it a block one.
    }
    else
    {
        // Make it a thin line.
    }

    console_structure = console_structure;

    return CONSOLE_RETURN_SUCCESS;
}

// Wait or check for an event on the console. Mouse or keyboard.
return_type console_event_wait(console_structure_type *console_structure, void *event_data, int *type, bool block)
{
    message_parameter_type message_parameter;

    message_parameter.block = block;

    // FIXME: Should be max_of_two(sizeof(console_mouse_event_type),
    //                             sizeof(console_keyboard_event_type);

    message_parameter.length = sizeof (keyboard_packet_type);
    message_parameter.data = event_data;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = IPC_CLASS_NONE;

    system_call_mailbox_receive(console_structure->ipc_structure.input_mailbox_id, &message_parameter);

    if (message_parameter.message_class == IPC_CONSOLE_KEYBOARD_EVENT)
    {
        *type = CONSOLE_EVENT_KEYBOARD;
    }
    else if (message_parameter.message_class == IPC_CONSOLE_MOUSE_EVENT)
    {
        *type = CONSOLE_EVENT_MOUSE;
    }
    else
    {
        return CONSOLE_RETURN_BAD_DATA_RETURNED;
    }

    return CONSOLE_RETURN_SUCCESS;
}

return_type console_buffer_print(console_structure_type *console_structure,
                                 int x, int y, uint8_t attribute,
                                 const char *string)
{
    unsigned int position = (y * console_structure->width) + x;
    unsigned int max_position = console_structure->width * console_structure->height;

    for (unsigned int i = 0; i < string_length(string); i++)
    {
        if (position + i > max_position)
        {
            // We are past the end of the buffer. Abort and return an error to the caller.
            return CONSOLE_RETURN_INVALID_ARGUMENT;
        }

        console_structure->buffer[position + i].character = string[i];
        console_structure->buffer[position + i].attribute = attribute;
    }

    return CONSOLE_RETURN_SUCCESS;
}

return_type console_buffer_print_formatted(
    console_structure_type *console_structure, int x, int y,
    uint8_t attribute, const char *format_string, ...)
{
    va_list arguments;

    // FIXME: Don't have a hardwired buffer like this!
    char output[1024];

    if (format_string == NULL)
    {
        output[0] = '\0';

        return CONSOLE_RETURN_INVALID_ARGUMENT;
    }

    va_start(arguments, format_string);
    string_print_va(output, format_string, arguments);
    va_end(arguments);

    return console_buffer_print(console_structure, x, y, attribute, output);
}

return_type console_flip(console_structure_type *console_structure)
{
    message_parameter_type message_parameter;

    if (!console_structure->initialised)
    {
        return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
    }

    if (console_structure->width <= 0 ||
        console_structure->height <= 0)
    {
        return CONSOLE_RETURN_INVALID_ARGUMENT;
    }

    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.length = console_structure->width *
                               console_structure->height *
                               sizeof(console_character_type);
    message_parameter.message_class = IPC_CONSOLE_OUTPUT_ALL;
    message_parameter.data = (void *) console_structure->buffer;
    message_parameter.block = TRUE;

    system_call_mailbox_send(console_structure->ipc_structure.output_mailbox_id, &message_parameter);

    return CONSOLE_RETURN_SUCCESS;
}
