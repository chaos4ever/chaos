/* $Id$ */
/* Abstract: Server initialisation and connection handling for the ATA
   server. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#include "ata.h"
#include "config.h"

/* Handle a client connection. */

void handle_connection (mailbox_id_type reply_mailbox_id,
                        service_type *service)
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

  message_parameter.data = data;
  message_parameter.block = TRUE;

  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_BLOCK;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
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
            ipc_block_read->number_of_blocks > service->number_of_sectors)
        {
          /* FIXME: Should we do the error handling in some other way? */

          message_parameter.data = NULL;
          message_parameter.length = 0;
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Tried to read out of range.");
        }
        else
        {
          if (!ata_read_sectors (service->interface, service->device,
                                 ipc_block_read->start_block_number + service->start_sector,
                                 ipc_block_read->number_of_blocks,
                                 message_parameter.data))
          {
            log_print (&log_structure, LOG_URGENCY_ERROR,
                       "Could not read requested sector(s).");
          }

          message_parameter.length = (ipc_block_read->number_of_blocks * 512);
        }
        message_parameter.block = TRUE;
        
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }

      case IPC_BLOCK_WRITE:
      {
        break;
      }
      
      case IPC_BLOCK_GET_INFO:
      {
        break;
      }
    }
  }
}

/* Create a service and listen to it. */

void handle_service (service_type *service)
{
  ipc_structure_type ipc_structure;

  if (ipc_service_create ("block", &ipc_structure, 
                          &empty_tag) != IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_ERROR,
               "Couldn't create block service.");
    system_exit ();
  }

  /* Main loop. */
  
  system_thread_name_set ("Service handler");
  
  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;
    
    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;
    
    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_thread_name_set ("Handling connection");
      handle_connection (reply_mailbox_id, service);
    }
  }    
}

int main (void)
{
  unsigned int interface_index;
  bool found_device = FALSE;

  interface_type interface[2] =
  {
    {
      14,
      0x1f0,
      MAILBOX_ID_NONE,
      {
        NULL,
        NULL
      }
    },
    {
      15,
      0x170,
      MAILBOX_ID_NONE,
      {
        NULL,
        NULL
      }
    }
  };

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);

  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initialising");

  /* Try to probe for devices on the interfaces. */

  for (interface_index = 0;
       interface_index < sizeof (interface) / sizeof (interface_type);
       interface_index++)
  {
    if (ata_init_interface (&interface[interface_index]))
    {
      /* We have at least one device on this interface. */

      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Initialisation of ATA interface at 0x%X succeeded.",
                           interface[interface_index].io_base);

      found_device = TRUE;
    }
    else
    {
      log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                           "Initialisation of ATA interface at 0x%X failed.",
                           interface[interface_index].io_base);
    }
  }

  /* If we didn't find any devices at all, exit. */

  if (!found_device)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY, "No ATA devices found.");
    system_exit ();
  }

  /* We are finished with the initialisation. */

  system_call_process_parent_unblock ();

  return 0;
}
