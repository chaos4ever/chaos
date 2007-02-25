/* $Id$ */
/* Abstract: Serial library. */
/* Author: Martin Alvarez <malvarez@aapsa.es> */

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

/* See The chaos Programming Reference Manual for more information
   about the functions in this library. */

#include <ipc/ipc.h>
#include <memory/memory.h>
#include <serial/serial.h>
#include <string/string.h>
#include <system/system.h>

/* Initialize a connection between the application and the serial
   service. */

return_type serial_init (ipc_structure_type *ipc_structure,
                         tag_type *tag)
{
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  /* Try to resolve the serial service. */

  if (ipc_service_resolve ("serial", mailbox_id, &services, 5, tag) !=
      IPC_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }

  ipc_structure->output_mailbox_id = mailbox_id[0];

  /* Connect to this service. */

  if (ipc_service_connection_request (ipc_structure) != IPC_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }

  return SERIAL_RETURN_SUCCESS;
}


/* Read data from the serial service. */

return_type serial_read (ipc_structure_type *ipc_structure,
                         unsigned char *data, unsigned int size)
{
  serial_data_type serial_data;
  serial_data_type *serial_data2;
  message_parameter_type message_parameter;

  /* Check requested size. */

  if (size == 0 || size > SERIAL_BUFFER_SIZE)
  {
    return SERIAL_RETURN_SIZE_INVALID;
  }

  serial_data.size = size;

  message_parameter.protocol = IPC_PROTOCOL_SERIAL;
  message_parameter.message_class = IPC_SERIAL_READ;
  message_parameter.data = (void *) &serial_data;
  message_parameter.length = sizeof (serial_data_type);
  message_parameter.block = TRUE;

  if (system_call_mailbox_send (ipc_structure->output_mailbox_id,
                                &message_parameter) != STORM_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }

  message_parameter.block = TRUE;
  message_parameter.length = sizeof (serial_data_type);
  
  if (system_call_mailbox_receive (ipc_structure->input_mailbox_id,
                                  &message_parameter) != STORM_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_DATA_UNAVAILABLE;
  }

  serial_data2 = message_parameter.data;
  
  if (serial_data2->status == SERIAL_RETURN_SUCCESS)
  {
    memory_copy (data, serial_data2->data, size);
  }
  
  return serial_data2->status;
}


/* Write data to the serial service. */

return_type serial_write (ipc_structure_type *ipc_structure,
                          void *data, unsigned int size)
{
  serial_data_type serial_data;
  serial_data_type *serial_data2;
  message_parameter_type message_parameter;

  /* Check requested size. */

  if (size == 0 || size > SERIAL_BUFFER_SIZE)
  {
    return SERIAL_RETURN_SIZE_INVALID;
  }

  serial_data.size = size;
  memory_copy (serial_data.data, data, size);
  
  message_parameter.protocol = IPC_PROTOCOL_SERIAL;
  message_parameter.length = sizeof (serial_data_type);
  message_parameter.message_class = IPC_SERIAL_WRITE;
  message_parameter.data = (void *) &serial_data;
  message_parameter.block = TRUE;

  if (system_call_mailbox_send (ipc_structure->output_mailbox_id,
                                &message_parameter) != STORM_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }

  message_parameter.block = TRUE;
  message_parameter.length = sizeof (serial_data_type);
  
  if (system_call_mailbox_receive (ipc_structure->input_mailbox_id,
                                  &message_parameter) != STORM_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }

  serial_data2 = message_parameter.data;

  return serial_data2->status;
}


/* Configure serial service. */

return_type serial_config_line (ipc_structure_type *ipc_structure,
                           unsigned int baudrate, unsigned int data_bits,
                           unsigned int stop_bits, unsigned int parity)
{
  serial_data_type serial_data;
  serial_data_type *serial_data2;
  message_parameter_type message_parameter;

  serial_data.baudrate = baudrate;
  serial_data.data_bits = data_bits;
  serial_data.stop_bits = stop_bits;
  serial_data.parity = parity;
    
  serial_data.mask = SERIAL_BAUDRATE_MASK | SERIAL_DATA_BITS_MASK | 
                    SERIAL_STOP_BITS_MASK | SERIAL_PARITY_MASK;

  message_parameter.protocol = IPC_PROTOCOL_SERIAL;
  message_parameter.length = sizeof (serial_data_type);
  message_parameter.message_class = IPC_SERIAL_CONFIG_WRITE;
  message_parameter.data = (void *) &serial_data;
  message_parameter.block = TRUE;

  if (system_call_mailbox_send (ipc_structure->output_mailbox_id,
                                &message_parameter) != STORM_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }


  message_parameter.block = TRUE;
  message_parameter.length = sizeof (serial_data_type);
  
  if (system_call_mailbox_receive (ipc_structure->input_mailbox_id,
                                  &message_parameter) != STORM_RETURN_SUCCESS)
  {
    return SERIAL_RETURN_SERVICE_UNAVAILABLE;
  }

  serial_data2 = message_parameter.data;

  return serial_data2->status;
}
