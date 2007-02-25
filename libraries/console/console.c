/* $Id$ */
/* Abstract: Console library. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

/* See The chaos Programming Reference Manual for more information
   about the functions in this library. */

#include <stdarg.h>
#include <console/console.h>
#include <ipc/ipc.h>
#include <string/string.h>

#include "config.h"

#define CONSOLE_MAILBOX_SIZE    32768

/* Initialise a connection between the application and the console
   service. */

return_type console_init (console_structure_type *console_structure,
                          tag_type *tag, unsigned int connection_class)
{
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;
  message_parameter_type message_parameter;

  console_structure->initialised = FALSE;

  /* Try to resolve the console service. */

  if (ipc_service_resolve ("console", mailbox_id, &services, 5, tag) != 
      IPC_RETURN_SUCCESS)
  {
    return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
  }

  console_structure->ipc_structure.output_mailbox_id = mailbox_id[0];

  /* Connect to this service. */

  if (ipc_service_connection_request (&console_structure->ipc_structure) != 
      IPC_RETURN_SUCCESS)
  {
    return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
  }

  console_structure->initialised = TRUE;

  /* Send the connection class. */

  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.message_class = IPC_CONSOLE_CONNECTION_CLASS_SET;
  message_parameter.data = &connection_class;
  message_parameter.length = sizeof (unsigned int);
  message_parameter.block = TRUE;

  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);

  return CONSOLE_RETURN_SUCCESS;
}

/* Allocate and open a new console with the specified attributes. */
/* FIXME: If input parameters are all zero (except for mode_type), a
   default mode with the requested type should be set and the
   attributes of that mode should be returned. */

return_type console_open (console_structure_type *console_structure,
                          unsigned int width, unsigned int height,
                          unsigned int depth, int mode_type)
{
  message_parameter_type message_parameter;
  ipc_console_attribute_type console_attribute;

  if (!console_structure->initialised)
  {
    return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
  }

  /* Try to open a console with the requested attributes. */
  /* FIXME: Handle return values. */

  console_attribute.width = width;
  console_attribute.height = height;
  console_attribute.depth = depth;
  console_attribute.mode_type = mode_type;

  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.length = sizeof (ipc_console_attribute_type);
  message_parameter.message_class = IPC_CONSOLE_OPEN;
  message_parameter.data = (void *) &console_attribute;
  message_parameter.block = TRUE;
  
  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);

  /* FIXME: Wait for return value. */

  return CONSOLE_RETURN_SUCCESS;
}

/* Change the attributes of the current console. */

return_type console_mode_set (console_structure_type *console_structure,
                              unsigned int width, unsigned int height,
                              unsigned int depth, int mode_type)
{
  message_parameter_type message_parameter;
  ipc_console_attribute_type console_attribute;

  if (!console_structure->initialised)
  {
    return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
  }

  console_attribute.width = width;
  console_attribute.height = height;
  console_attribute.depth = depth;
  console_attribute.mode_type = mode_type;

  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.length = sizeof (ipc_console_attribute_type);
  message_parameter.message_class = IPC_CONSOLE_MODE_SET;
  message_parameter.data = (void *) &console_attribute;
  message_parameter.block = TRUE;

  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);
  return CONSOLE_RETURN_SUCCESS;
}

/* Change the size (text rows and columns, not mode!) of the current
   console. */

return_type console_resize (console_structure_type *console_structure,
                            unsigned int width, unsigned int height)
{
  ipc_console_resize_type ipc_console_resize;
  message_parameter_type message_parameter;

  if (!console_structure->initialised)
  {
    return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
  }

  ipc_console_resize.width = width;
  ipc_console_resize.height = height;

  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.length = sizeof (ipc_console_resize_type);
  message_parameter.message_class = IPC_CONSOLE_RESIZE;
  message_parameter.data = (void *) &ipc_console_resize;
  message_parameter.block = TRUE;
  
  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);

  return CONSOLE_RETURN_SUCCESS;
}

/* Tell the console server that we want input from the keyboard sent to us. */

return_type console_use_keyboard (console_structure_type *console_structure,
                                  bool which, int type)
{
  message_parameter_type message_parameter;

  /* Shall we use the keyboard at all? */
  
  message_parameter.block = TRUE;
  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.message_class = which ? IPC_CONSOLE_ENABLE_KEYBOARD :
    IPC_CONSOLE_DISABLE_KEYBOARD ;
  message_parameter.length = 0;
  message_parameter.data = NULL;
  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);
  
  /* Extended or normal target? */
  
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

  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);
  return CONSOLE_RETURN_SUCCESS;
}

/* Tell the console server that we want input from the mouse sent to us. */

return_type console_use_mouse (console_structure_type *console_structure,
                               bool which)
{
  message_parameter_type message_parameter;

  message_parameter.block = TRUE;
  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.message_class = which ? IPC_CONSOLE_ENABLE_MOUSE :
    IPC_CONSOLE_DISABLE_MOUSE ;
  message_parameter.length = 0;
  message_parameter.data = NULL;
  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);
  return CONSOLE_RETURN_SUCCESS;
}

/* Print a string to the console service. */

return_type console_print (console_structure_type *console_structure,
                           const char *string)
{
  message_parameter_type message_parameter;

  if (!console_structure->initialised)
  {
    return CONSOLE_RETURN_SERVICE_UNAVAILABLE;
  }

  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.length = string_length (string) + 1;
  message_parameter.message_class = IPC_CONSOLE_OUTPUT;
  message_parameter.data = (void *) string;
  message_parameter.block = TRUE;

  system_call_mailbox_send (console_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);

  return CONSOLE_RETURN_SUCCESS;
}

/* Prints the given data to the console, a la printf and friends. */

return_type console_print_formatted (console_structure_type *console_structure,
                                     const char *format_string, ...)
{
  va_list arguments;
  return_type return_value;

  /* FIXME! */

  char output[1024];

  if (format_string == NULL)
  {
    output[0] = '\0';
    return CONSOLE_RETURN_INVALID_ARGUMENT;
  }
  
  va_start (arguments, format_string);
  return_value = string_print_va (output, format_string, arguments);
  va_end (arguments);

  return console_print (console_structure, output);
}

return_type console_clear (console_structure_type *console_structure)
{
  return console_print (console_structure, "\e[2J\e[1;1H");
}

/* Move the cursor. Zero indexed!!! */

return_type console_cursor_move (console_structure_type *console_structure,
                                 unsigned int x, unsigned int y)
{
  /* FIXME: Should store the size of the console somewhere. */

  if (x > 1000 || y > 1000)
  {
    return CONSOLE_RETURN_INVALID_ARGUMENT;
  }
  else
  {
    return console_print_formatted (console_structure,
                                    "\e[%d;%dH", x + 1, y + 1);
  }
}

/* Set the attributes. */
/* FIXME: There should be a function for each of the attributes. */

return_type console_attribute_set (console_structure_type *console_structure,
                                   int foreground, int background,
                                   int attributes)
{
  return console_print_formatted (console_structure, "\e[%d;%d;%dm",
                                  attributes, foreground + 30,
                                  background + 40);
}

/* Set the cursor appearance. */

return_type console_cursor_appearance_set
  (console_structure_type *console_structure, bool visibility, bool block)
{
  /* FIXME: Add ANSI commands for setting the console appearance. We
     can not really _add_ ANSI commands, can we? ;) */

  if (!visibility)
  {
    /* Hide the cursor. */

  }
  else if (block)
  {
    /* Make it a block one. */

  }
  else
  {
    /* Make it a thin line. */

  }

  console_structure = console_structure;

  return CONSOLE_RETURN_SUCCESS;
}

/* Wait or check for an event on the console. Mouse or keyboard. */

return_type console_event_wait (console_structure_type *console_structure,
                                void *event_data, int *type, bool block)
{
  message_parameter_type message_parameter;

  message_parameter.block = block;

  /* FIXME: Should be max_of_two (sizeof (console_mouse_event_type),
                                  sizeof (console_keyboard_event_type); */

  message_parameter.length = sizeof (keyboard_packet_type);
  message_parameter.data = event_data;
  message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
  message_parameter.message_class = IPC_CLASS_NONE;

  system_call_mailbox_receive
    (console_structure->ipc_structure.input_mailbox_id, &message_parameter);

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
