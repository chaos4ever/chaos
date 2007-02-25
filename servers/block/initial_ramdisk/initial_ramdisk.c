/* $Id$ */
/* Abstract: Initial ramdisk server. */
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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include "config.h"
#include "ramdisk.h"
#include "ramdisk-auto.h"

/* Define as TRUE if you are debugging this server. */

#undef DEBUG

/* Information about this block device. */

static ipc_block_info_type ipc_block_info =
{
  BLOCK_SIZE, 
  NUMBER_OF_BLOCKS,
  
  /* Writable. */

  TRUE,

  /* Readable. */

  TRUE,
  
  /* Label. */

  "Initial ramdisk " PACKAGE_VERSION
};

static log_structure_type log_structure;
static tag_type empty_tag = 
{
  0, 0, "" 
};

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id)
{
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  bool done = FALSE;
  u32 *data;
  unsigned int data_size = 1024;

  memory_allocate ((void **) &data, data_size);

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  message_parameter.block = TRUE;

  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_BLOCK;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    message_parameter.data = data;
  
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (message_parameter.message_class)
    {
      case IPC_BLOCK_READ:
      {
        ipc_block_read_type *ipc_block_read = (ipc_block_read_type *) data;

        /* Do some boundary checking. */
        
        if (ipc_block_read->start_block_number +
            ipc_block_read->number_of_blocks > ipc_block_info.number_of_blocks)
        {
          /* FIXME: Should we do the error handling in some other way? */

          message_parameter.data = NULL;
          message_parameter.length = 0;
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Tried to read out of range.");
        }
        else
        {
#ifdef DEBUG
          log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                               "Reading blocks %u-%u data.", 
                               ipc_block_read->start_block_number,
                               ipc_block_read->start_block_number +
                               ipc_block_read->number_of_blocks);
#endif
          message_parameter.data =
            (u8 *) &ramdisk[ipc_block_read->start_block_number *
                           ipc_block_info.block_size];
          message_parameter.length = (ipc_block_read->number_of_blocks *
                                      ipc_block_info.block_size);
        }
        message_parameter.block = TRUE;
        
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }
      
      case IPC_BLOCK_WRITE:
      {
        /* FIXME: Implement this. */

        break;
      }
      
      case IPC_BLOCK_GET_INFO:
      {
        /* FIXME: Implement this. */

        break;
      }
    }
  }
}

/* Main function. */

int main (void)
{
  ipc_structure_type ipc_structure;

  /* Set the name of the server. */

  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initialising");

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);
  
  /* Create the service. */

  if (ipc_service_create ("block", &ipc_structure, &empty_tag) != 
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create block service.");
    return -1;
  }

  system_call_process_parent_unblock ();
  
  /* Main loop. */

  system_thread_name_set ("Service handler");
  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;

    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;

    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_call_thread_name_set ("Handling connection");
      handle_connection (reply_mailbox_id);
    }
  }    
}
