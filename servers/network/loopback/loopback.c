/* $Id$ */
/* Abstract: Loopback ethernet server for chaos. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#include "config.h"

static log_structure_type log_structure;

/* An empty tag list. */

tag_type empty_tag =
{
  0, 0, ""
};

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id)
{
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  bool done = FALSE;
  unsigned int data_size = 1024;
  u32 *data;
  u32 **data_pointer = &data;

  memory_allocate ((void **) data_pointer, data_size);

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  message_parameter.data = data;
  message_parameter.block = TRUE;
  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;

  while (!done)
  {
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     &data_size) !=
        STORM_RETURN_SUCCESS)
    {
      continue;
    }

    switch (message_parameter.message_class)
    {
      case IPC_ETHERNET_REGISTER_TARGET:
      {
#if FALSE
        /* FIXME: Check if the protocol is already registered */

        device->target[device->number_of_targets].mailbox_id = 
          ipc_structure.output_mailbox_id;
        device->target[device->number_of_targets].protocol_type =
          system_byte_swap_u16 (data[0]);
        device->number_of_targets++;
#endif
        break;
      }

      case IPC_ETHERNET_PACKET_SEND:
      {
#if FALSE
        if (!el3_start_transmit (data, message_parameter.length, device))
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Failed to send an ethernet packet.");

          /* FIXME: Do something. */
        }
#endif
        break;
      }

      case IPC_ETHERNET_ADDRESS_GET:
      {
#if FALSE
        memory_copy (data, &device->ethernet_address, 6);
        message_parameter.length = 6;
        system_call_mailbox_send (ipc_structure.output_mailbox_id,
                                  &message_parameter);
#endif
        break;
      }

      default:
      {
        log_print (&log_structure, LOG_URGENCY_ERROR,
                   "Unknown IPC command received.");
        break;
      }
    }
  }
}

/* Main function. */

int main (void)
{
  ipc_structure_type ipc_structure;

  /* Set up our name. */

  system_call_process_name_set (PACKAGE_NAME);
  system_call_thread_name_set ("Initialising");

  log_init (&log_structure, PACKAGE_NAME, &empty_tag); 

  /* Create the service. */

  if (ipc_service_create ("ethernet", &ipc_structure, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create ethernet service.");
    return -1;
  }

  /* Main loop. */

  system_call_thread_name_set ("Service handler");

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
