/* $Id$ */
/* Abstract: Console server. Will eventually be 100% ANSI escape
             sequence compatible. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include "config.h"
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

/* An empty tag list. */

tag_type empty_tag =
{
  0, 0, ""
};

/* Link in this console into our linked list of consoles. */

void console_link (console_type *console)
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

/* Function:    console_flip ()
   Purpose:     Save the console state to current_console->buffer and
                copy the contents of console->buffer to the screen.
   Returns:     Nothing.
   Parameters:  Pointer to console-struct for console to flip to.
   TODO:        Support switching of video mode. */

void console_flip (console_type *console)
{
  video_cursor_type video_cursor;
  message_parameter_type message_parameter;
  video_mode_type video_mode;

  console->output = screen;
  current_console->output = current_console->buffer;

  if (current_console->type == VIDEO_MODE_TYPE_TEXT)
  {
    memory_copy (current_console->buffer, screen,
                 current_console->width * current_console->height *
                 sizeof (character_type));
  }

  if (has_video)
  {
    /* Set the correct videomode for this console. */

    if (console->width != current_console->width ||
        console->height != current_console->height ||
        console->depth != current_console->depth ||
        console->type != current_console->type)
    {
      video_mode.width = console->width;
      video_mode.height = console->height;
      video_mode.depth = console->depth;
      video_mode.mode_type = console->type;
      
      if (video_mode_set (&video_structure, &video_mode) !=
          VIDEO_RETURN_SUCCESS)
      {
        /* FIXME: Fail and return here. */
      }
    }

    /* Move the cursor. */
    /* FIXME: library_video should have a function for this. */

    video_cursor.x = console->cursor_x;
    video_cursor.y = console->cursor_y;
    message_parameter.data = &video_cursor;
    message_parameter.block = FALSE;
    message_parameter.length = sizeof (video_cursor_type);
    message_parameter.protocol = IPC_PROTOCOL_VIDEO;
    message_parameter.message_class = IPC_VIDEO_CURSOR_PLACE;
    
    ipc_send (video_structure.output_mailbox_id, &message_parameter);
  }

  if (console->type == VIDEO_MODE_TYPE_TEXT)
  {
    memory_copy (console->output, console->buffer,
                 console->width * console->height *
                 sizeof (character_type));
  }

  current_console = console;
} 

/* Main function. */

int main (void)
{
  ipc_structure_type ipc_structure;

  memory_init ();
  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initialising");

  if (ipc_service_create ("console", &ipc_structure, &empty_tag) != 
      IPC_RETURN_SUCCESS)
  {
    return -1;
  }
  
  system_call_memory_reserve (CONSOLE_VIDEO_MEMORY, CONSOLE_VIDEO_MEMORY_SIZE,
                              (void **) &screen);
                         
  system_thread_name_set ("Service handler");

  /* Let the other processes run now. */

  system_call_process_parent_unblock ();

  /* Main loop. */

  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;

    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;

    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_thread_name_set ("Handling connection");
      handle_connection (reply_mailbox_id);
    }
  }    
}
