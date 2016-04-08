// Abstract: Console server. Will eventually be 100% ANSI escape sequence compatible.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#include <memory/memory.h>
#include <video/video.h>

#include "console.h"
#include "console_output.h"

volatile unsigned int number_of_consoles = 0;
character_type *screen = (character_type *) NULL;
volatile console_type *current_console = NULL;
console_type *console_list = NULL;
ipc_structure_type video_structure;
volatile bool has_video = FALSE;
volatile unsigned int console_id = 0;
volatile console_type *console_shortcut[12] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// An empty tag list.
tag_type empty_tag =
{
    0, 0, ""
};

// Link in this console into our linked list of consoles.
void console_link(console_type *console)
{
    if (console_list == NULL)
    {
        console_list = console;
        console_list->next = NULL;
    }
    else
    {
        console->next = (struct console_type *) console_list;
        console_list = console;
    }

    console_shortcut[number_of_consoles] = console;
}

// Function:    console_flip ()
// Purpose:     Save the console state to current_console->buffer and
//              copy the contents of console->buffer to the screen.
// Returns:     Nothing.
// Parameters:  Pointer to console-struct for console to flip to.
// TODO:        Support switching of video mode.
void console_flip(console_type *console)
{
    video_cursor_type video_cursor;
    message_parameter_type message_parameter;
    video_mode_type video_mode;

    console->output = screen;
    current_console->output = current_console->buffer;

    if (current_console->type == VIDEO_MODE_TYPE_TEXT)
    {
        memory_copy(current_console->buffer, screen,
                    current_console->width * current_console->height * sizeof(character_type));
    }

    if (has_video)
    {
        // Set the correct videomode for this console.
        if (console->width != current_console->width ||
            console->height != current_console->height ||
            console->depth != current_console->depth ||
            console->type != current_console->type)
        {
            video_mode.width = console->width;
            video_mode.height = console->height;
            video_mode.depth = console->depth;
            video_mode.mode_type = console->type;

            if (video_mode_set(&video_structure, &video_mode) != VIDEO_RETURN_SUCCESS)
            {
                // FIXME: Fail and return here.
            }
        }

        // Move the cursor.
        // FIXME: library_video should have a function for this.
        video_cursor.x = console->cursor_x;
        video_cursor.y = console->cursor_y;
        message_parameter.data = &video_cursor;
        message_parameter.block = FALSE;
        message_parameter.length = sizeof (video_cursor_type);
        message_parameter.protocol = IPC_PROTOCOL_VIDEO;
        message_parameter.message_class = IPC_VIDEO_CURSOR_PLACE;

        ipc_send(video_structure.output_mailbox_id, &message_parameter);
    }

    if (console->type == VIDEO_MODE_TYPE_TEXT)
    {
        memory_copy(console->output, console->buffer,
                    console->width * console->height * sizeof(character_type));
    }

    current_console = console;
}

int main(void)
{
    ipc_structure_type ipc_structure;

    memory_init();
    system_process_name_set("console");
    system_thread_name_set("Initialising");

    if (ipc_service_create("console", &ipc_structure, &empty_tag) != IPC_RETURN_SUCCESS)
    {
        return -1;
    }

    character_type **screen_pointer = &screen;
    system_call_memory_reserve(CONSOLE_VIDEO_MEMORY, CONSOLE_VIDEO_MEMORY_SIZE, (void **) screen_pointer);

    system_thread_name_set("Service handler");

    system_call_process_parent_unblock();

    while (TRUE)
    {
        ipc_service_connection_wait(&ipc_structure);
        mailbox_id_type reply_mailbox_id = ipc_structure.output_mailbox_id;

        system_thread_create((thread_entry_point_type *) handle_connection, &reply_mailbox_id);
    }
}
