/* $Id$ */
/* Abstract: IPv4 library. */
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

#include <ipc/ipc.h>
#include <ipv4/ipv4.h>
#include <string/string.h>

/* Initialise the IPv4 support (by contacting the IPv4 server). */

return_type ipv4_init (ipc_structure_type *ipv4_structure, tag_type *tag)
{
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  /* Try to resolve the IPv4 service. */

  if (ipc_service_resolve ("ipv4", mailbox_id, &services, 0, tag) != 
      IPC_RETURN_SUCCESS)
  {
    return IPV4_RETURN_SERVICE_UNAVAILABLE;
  }

  ipv4_structure->output_mailbox_id = mailbox_id[0];

  /* Connect to this service. */

  if (ipc_service_connection_request (ipv4_structure) != IPC_RETURN_SUCCESS)
  {
    return IPV4_RETURN_SERVICE_UNAVAILABLE;
  }

  return IPV4_RETURN_SUCCESS;
}

/* Get the host name of the system. */

return_type ipv4_host_name_get
  (ipc_structure_type *ipv4_structure, char *host_name)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_GET_HOST_NAME;
  message_parameter.data = host_name;
  message_parameter.length = 0;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  message_parameter.length = IPV4_HOST_NAME_LENGTH;
  ipc_receive (ipv4_structure->input_mailbox_id, &message_parameter,
               NULL);
  
  return IPV4_RETURN_SUCCESS;
}

/* Convert a string-based IP address to the binary notation. */

return_type ipv4_string_to_binary_ip_address (u8 *string, u32 *ip_address)
{
  unsigned int characters;
  unsigned int a, b, c, d;
  unsigned int position = 0;
  unsigned int length = string_length (string);

  if (string_to_number (string + position, &a, &characters) != 
      STRING_RETURN_SUCCESS)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (a > 255)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  position += characters + 1;

  if (position > length)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (string_to_number (string + position, &b, &characters) !=
      STRING_RETURN_SUCCESS)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (b > 255)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  position += characters + 1;

  if (position > length)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (string_to_number (string + position, &c, &characters) !=
      STRING_RETURN_SUCCESS)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (c > 255)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  position += characters + 1;

  if (position > length)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (string_to_number (string + position, &d, &characters) !=
      STRING_RETURN_SUCCESS)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  if (d > 255)
  {
    return IPV4_RETURN_INVALID_ARGUMENT;
  }

  *ip_address = a + (b << 8) + (c << 16) + (d << 24);
  return IPV4_RETURN_SUCCESS;
}

/* Connect to a remote host (in the case of TCP), or just bind the
   port (in the case of UDP). */

return_type ipv4_connect 
  (ipc_structure_type *ipv4_structure, ipv4_connect_type *connect,
   ipv4_socket_id_type *socket_id)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_CONNECT;
  message_parameter.data = connect;
  message_parameter.length = sizeof (ipv4_connect_type);
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  message_parameter.data = socket_id;
  message_parameter.length = sizeof (ipv4_socket_id_type);
  message_parameter.block = TRUE;
  ipc_receive (ipv4_structure->input_mailbox_id, &message_parameter, NULL);

  return IPV4_RETURN_SUCCESS;
}

/* Reconnect the given socket. Only works for UDP datagram streams. */

return_type ipv4_reconnect
  (ipc_structure_type *ipv4_structure, ipv4_reconnect_type *connect)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_RECONNECT;
  message_parameter.data = connect;
  message_parameter.length = sizeof (ipv4_reconnect_type);
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  return IPV4_RETURN_SUCCESS;
}

/* Send data on the given socket. */

return_type ipv4_send 
  (ipc_structure_type *ipv4_structure, ipv4_send_type *send)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_SEND;
  message_parameter.data = send;
  message_parameter.length = sizeof (ipv4_send_type) + send->length;
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  return IPV4_RETURN_SUCCESS;
}

/* Receive data on the given socket. */

return_type ipv4_receive
  (ipc_structure_type *ipv4_structure, ipv4_receive_type *receive,
   ipv4_socket_id_type socket_id, void **data, unsigned int *length)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_RECEIVE;
  message_parameter.data = &socket_id;
  message_parameter.length = sizeof (ipv4_socket_id_type);
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  message_parameter.length = *length;
  message_parameter.data = *data;
  ipc_receive (ipv4_structure->input_mailbox_id, &message_parameter,
               length);
  *data = message_parameter.data;
  *length = message_parameter.length;

  message_parameter.data = receive;
  message_parameter.length = sizeof (ipv4_receive_type);
  ipc_receive (ipv4_structure->input_mailbox_id, &message_parameter,
               NULL);

  return IPV4_RETURN_SUCCESS;
}

/* Set flags. */

return_type ipv4_set_flags 
  (ipc_structure_type *ipv4_structure, unsigned int flags)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_SET_FLAGS;
  message_parameter.data = &flags;
  message_parameter.length = sizeof (unsigned int);
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  return IPV4_RETURN_SUCCESS;
}

/* Get flags. */

return_type ipv4_get_flags 
  (ipc_structure_type *ipv4_structure, unsigned int *flags)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_GET_FLAGS;
  message_parameter.data = flags;
  message_parameter.length = 0;
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure->output_mailbox_id, &message_parameter);

  message_parameter.length = sizeof (unsigned int);
  ipc_receive (ipv4_structure->input_mailbox_id, &message_parameter,
               NULL);

  return IPV4_RETURN_SUCCESS;
}

/* Convert an IPv4 address to string form. */

return_type ipv4_address_to_string (char *string, u32 ipv4_address)
{
  string_print (string, "%lu.%lu.%lu.%lu",
                (ipv4_address >> 0) & 0xFF,
                (ipv4_address >> 8) & 0xFF, 
                (ipv4_address >> 16) & 0xFF,
                (ipv4_address >> 24) & 0xFF);

  return IPV4_RETURN_SUCCESS;
}

