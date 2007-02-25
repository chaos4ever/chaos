/* $Id$ */
/* Abstract: Log library. The functions in this library are used for
   communicating to the log server. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#include <console/console.h>
#include <log/log.h>
#include <string/string.h>

#include <stdarg.h>

/* Internal function for actually printing to the log. */

static return_type log (log_structure_type *log_structure,
                        ipc_log_print_type *ipc_log_print)
{
  message_parameter_type message_parameter;

  message_parameter.data = ipc_log_print;
  message_parameter.length = sizeof (ipc_log_print_type);
  message_parameter.protocol = IPC_PROTOCOL_LOG;
  message_parameter.message_class = IPC_LOG_PRINT;
  message_parameter.block = TRUE;

  /* FIXME: Check the return value of this, and return something if it
     fails. */

  system_call_mailbox_send (log_structure->ipc_structure.output_mailbox_id,
                            &message_parameter);
  return LOG_RETURN_SUCCESS;
}

/* Internal function for initialise a connection between the
   application and the log service. */

return_type log_init 
  (log_structure_type *log_structure, char *class, tag_type *tag)
{
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  /* Try to resolve the console service. */

  if (ipc_service_resolve ("log", mailbox_id, &services, 5, tag) != 
      IPC_RETURN_SUCCESS)
  {
    return LOG_RETURN_SERVICE_UNAVAILABLE;
  }

  log_structure->ipc_structure.output_mailbox_id = mailbox_id[0];

  /* Connect to this service. */

  if (ipc_service_connection_request (&log_structure->ipc_structure) !=
      IPC_RETURN_SUCCESS)
  {
    return LOG_RETURN_SERVICE_UNAVAILABLE;
  }
  
  log_structure->log_class = class;

  return LOG_RETURN_SUCCESS;
}

/* Print a formatted string to the log. */

return_type log_print_formatted (log_structure_type *log_structure,
                                 unsigned int urgency,
                                 const char *format_string, ...)
{
  va_list arguments;
  return_type return_value;
  ipc_log_print_type ipc_log_print;

  /* FIXME! */

  char output[1024];

  if (format_string == NULL)
  {
    return LOG_RETURN_INVALID_ARGUMENT;
  }
  
  va_start (arguments, format_string);
  return_value = string_print_va (output, format_string, arguments);
  va_end (arguments);

  ipc_log_print.urgency = urgency;
  string_copy_max (ipc_log_print.log_class, log_structure->log_class,
                   IPC_LOG_MAX_CLASS_LENGTH);
  string_copy_max (ipc_log_print.message, output, IPC_LOG_MAX_MESSAGE_LENGTH);
  log (log_structure, &ipc_log_print);

  return LOG_RETURN_SUCCESS;
}

/* Print a message to the log. */

return_type log_print (log_structure_type *log_structure,
                       unsigned int urgency,
                       const char *message)
{
  ipc_log_print_type ipc_log_print;

  ipc_log_print.urgency = urgency;
  string_copy_max (ipc_log_print.log_class, log_structure->log_class,
                   IPC_LOG_MAX_CLASS_LENGTH);
  string_copy_max (ipc_log_print.message, message,
                   IPC_LOG_MAX_MESSAGE_LENGTH);
  
  return log (log_structure, &ipc_log_print);
}
