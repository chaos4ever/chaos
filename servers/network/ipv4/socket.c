/* $Id$ */
/* Abstract: Socket management. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#include "config.h"
#include "ipv4.h"
#include "socket.h"
#include "udp.h"

static ipv4_socket_id_type last_socket_id = 0;
static socket_type **socket_hash = NULL;

/* Get the hash for this socket ID. */

ipv4_socket_id_type socket_get_hash (ipv4_socket_id_type socket_id)
{
  return socket_id % SOCKET_HASH_SIZE;
}

/* Get the first free socket ID type. */

ipv4_socket_id_type socket_get_free (void)
{
  while (socket_find (last_socket_id) != NULL)
  {
    last_socket_id++;
  }
  
  return last_socket_id;
}

/* Initialise the socket support. */

void socket_init (void)
{
  memory_allocate ((void **) &socket_hash, 
                   SOCKET_HASH_SIZE * sizeof (socket_type *));
}

/* Insert the given socket into the hash table. */

void socket_insert (socket_type *socket)
{
  ipv4_socket_id_type socket_id = socket_get_hash (socket->socket_id);

  list_node_insert ((list_type **) &socket_hash[socket_id], 
                    (list_type *) socket);
}

/* Delete the given socket. */

void socket_delete (socket_type *socket)
{
  ipv4_socket_id_type socket_id = socket_get_hash (socket->socket_id);

  list_node_delete ((list_type **) &socket_hash[socket_id], 
                    (list_type *) socket);
}

/* Find the socket with the given socket ID. */

socket_type *socket_find (ipv4_socket_id_type socket_id)
{
  socket_type *socket;
  ipv4_socket_id_type hash_index = socket_get_hash (socket_id);

  socket = socket_hash[hash_index];

  while (socket != NULL && socket->socket_id != socket_id)
  {
    socket = (socket_type *) socket->next;
  }

  return socket;
}

/* Send data on the given socket. */

return_type socket_send 
  (ipv4_socket_id_type socket_id, unsigned int length, void *data)
{
  socket_type *socket = socket_find (socket_id);

  if (socket == NULL)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Socket %u not found!", socket_id);
    return SOCKET_RETURN_INVALID_ARGUMENT;
  }

  if (socket->protocol_type == IPC_IPV4_PROTOCOL_UDP)
  {
    udp_send (data, length, socket);
  }

  return SOCKET_RETURN_SUCCESS;
}

/* Read data from the given socket. */

void socket_receive
  (ipv4_socket_id_type socket_id, mailbox_id_type mailbox_id)
{
  socket_type *socket = socket_find (socket_id);
  message_parameter_type message_parameter;
  ipv4_receive_type receive;

  if (socket == NULL)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Socket %u not found!", socket_id);

    /* FIXME: If this occurs, the receiver will sleep forever. */

    return;
  }

  /* Wait until there is data available. */

  while (socket->packet_list == NULL)
  {
    /* FIXME: We should have some kind of semaphore system (with real
       blocking) for this. */

    system_sleep (100);
  }

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_RECEIVE;
  message_parameter.data = socket->packet_list->data;
  message_parameter.length = socket->packet_list->length;
  message_parameter.block = TRUE;
  ipc_send (mailbox_id, &message_parameter);

  receive.address = socket->packet_list->source_address;
  receive.port = socket->packet_list->source_port;
  message_parameter.data = &receive;
  message_parameter.length = sizeof (ipv4_receive_type);
  ipc_send (mailbox_id, &message_parameter);

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "ULL: %u", socket->packet_list->length);

  list_node_delete ((list_type **) &socket->packet_list, 
                    (list_type *) socket->packet_list);
}
