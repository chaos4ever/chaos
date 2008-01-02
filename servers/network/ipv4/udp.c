/* $Id$ */
/* Abstract: UDP routines. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development */
/* Copyright 2007 chaos development. */

#include <log/log.h>
#include <system/system.h>

#include "arp.h"
#include "ipv4.h"
#include "route.h"
#include "socket.h"
#include "udp.h"

/* Locals. */

static socket_type **udp_hash = NULL;
static unsigned int last_source_port = 1024;

/* Initialise the UDP subsystem. */

void udp_init (void)
{
  socket_type ***udp_hash_pointer = &udp_hash;
  memory_allocate ((void **) udp_hash_pointer, 
                   SOCKET_HASH_SIZE * sizeof (socket_type *));
}

/* Create a UDP header. */

void udp_header_create (u16 destination_port, u16 source_port,
                        u16 length, udp_header_type *udp_header)
{
  udp_header->destination_port = system_byte_swap_u16 (destination_port);
  udp_header->source_port = system_byte_swap_u16 (source_port);
  udp_header->length = system_byte_swap_u16 (length + sizeof (udp_header_type));
  udp_header->check = 0;
  //  udp_header->check = ipv4_checksum ((u16 *) udp_header,
  //                                     sizeof (udp_header_type));
}

/* Find the socket with the given socket ID. */

static socket_type *udp_socket_find (unsigned int source_port)
{
  socket_type *socket;
  ipv4_socket_id_type hash_index = socket_get_hash (source_port);

  socket = udp_hash[hash_index];

  while (socket != NULL && socket->source_port != source_port)
  {
    socket = (socket_type *) socket->next;
  }

  return socket;
}

/* Insert this socket to the lists. */

static void udp_socket_insert (socket_type *socket)
{
  unsigned int hash_index = socket_get_hash (socket->source_port);

  list_node_insert ((list_type **) &udp_hash[hash_index], 
                    (list_type *) socket);
}

/* Find a free source UDP port. */

static unsigned int udp_find_port (void)
{
  while (udp_socket_find (last_source_port) != NULL)
  {
    last_source_port++;
  }

  return last_source_port;
}

/* Connect... */

return_type udp_connect 
  (u32 destination, unsigned int destination_port, unsigned int *socket_id)
{
  socket_type *socket;
  socket_type **socket_pointer = &socket;

  memory_allocate ((void **) socket_pointer, sizeof (socket_type));
  socket->protocol_type = IPC_IPV4_PROTOCOL_UDP;
  socket->source_port = udp_find_port ();
  socket->destination_port = destination_port;
  socket->destination_address = destination;
  *socket_id = socket->socket_id = socket_get_free ();

  /* Add this socket to the hash tables. */

  udp_socket_insert (socket);
  socket_insert (socket);

  /* FIXME: Add this socket to the list. */

  return UDP_RETURN_SUCCESS;
}

/* Send data on the given UDP socket. */

return_type udp_send 
  (void *data, unsigned int length, socket_type *socket)
{
  u8 ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
  ipc_structure_type *ethernet_structure;
  bool direct;
  ipv4_ethernet_header_type *ethernet_header;
  ipv4_interface_type *interface;
  ipv4_header_type *ipv4_header;
  udp_header_type *udp_header;
  message_parameter_type message_parameter;
  char string[16];

  ipv4_address_to_string (string, socket->destination_address);

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "Sending UDP packet to %s:%u", string,
                       socket->destination_port);

  if (socket->protocol_type != IPC_IPV4_PROTOCOL_UDP)
  {
    return UDP_RETURN_INVALID_ARGUMENT;
  }

  /* First, find out how we should route this packet. */

  if (!route_find (socket->destination_address, &interface, 
                   &direct, &ethernet_structure))
  {
    return UDP_RETURN_DESTINATION_UNREACHABLE;
  }

  /* Get the ethernet address of the recipient. */

  while (!arp_ip_to_ethernet_address (socket->destination_address,
                                      ethernet_address))
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                         "Sending ARP who-has for %lX on %s.", 
                         socket->destination_address,
                         interface->identification);
    arp_who_has (socket->destination_address, interface, ethernet_structure);
    system_sleep (500);
  }

  /* Now, construct the package, and send it. */

  ipv4_ethernet_header_type **ethernet_header_pointer = &ethernet_header;
  memory_allocate ((void **) ethernet_header_pointer,
                   sizeof (ipv4_ethernet_header_type) +
                   sizeof (ipv4_header_type) + sizeof (udp_header_type) +
                   length);

  ipv4_ethernet_header_create 
    (ethernet_address, interface->hardware_address,
     IPV4_ETHERNET_PROTOCOL_IPV4, ethernet_header);

  ipv4_header = (ipv4_header_type *) &ethernet_header->data;

  ipv4_header_create (socket->destination_address, interface->ip_address,
                      IP_PROTOCOL_UDP, sizeof (udp_header_type) + length,
                      ipv4_header);

  udp_header = (udp_header_type *) &ipv4_header->data;
  udp_header_create (socket->destination_port, socket->source_port,
                     length, udp_header);
  memory_copy (&udp_header->data, data, length);

  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
  message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
  message_parameter.data = ethernet_header;
  message_parameter.length = (sizeof (ipv4_ethernet_header_type) + 
                              sizeof (ipv4_header_type) + 
                              sizeof (udp_header_type) + length);

  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);
  
  return UDP_RETURN_SUCCESS;
}

/* Receive an UDP packet and process it. */

void udp_packet_receive
  (ipv4_interface_type *interface __attribute__ ((unused)),
   ipv4_ethernet_header_type *ethernet_header,
   int length, mailbox_id_type output_mailbox_id __attribute__ ((unused)))
{
  ipv4_header_type *ipv4_header = (ipv4_header_type *) &ethernet_header->data;
  udp_header_type *udp_header __attribute__ ((unused)) =
    (udp_header_type *) ((u8 *) ipv4_header + ipv4_header->header_length * 4);
  char source_address[256], destination_address[256];
  socket_type *socket;

  ipv4_address_to_string (source_address, ipv4_header->source_address);
  ipv4_address_to_string (destination_address, 
                          ipv4_header->destination_address);

  log_print_formatted 
    (&log_structure, LOG_URGENCY_DEBUG, "%s: UDP packet: %s:%u -> %s:%u.",
     interface->identification, source_address,
     system_big_endian_to_native_u16 (udp_header->source_port),
     destination_address, 
     system_big_endian_to_native_u16 (udp_header->destination_port));
  
  /* Check if this packet should be delivered somewhere. */

  socket = udp_socket_find 
    (system_big_endian_to_native_u16 (udp_header->destination_port));

  if (socket != NULL)
  {
    packet_list_type *packet;
    packet_list_type **packet_pointer = &packet;

    log_print (&log_structure, LOG_URGENCY_DEBUG, "Buffering packet");

    memory_allocate ((void **) packet_pointer, sizeof (packet_list_type));
    packet->length = (length - sizeof (ipv4_ethernet_header_type) -
                      sizeof (ipv4_header_type) - sizeof (udp_header_type));
    packet->source_address = ipv4_header->source_address;
    packet->source_port = 
      system_big_endian_to_native_u16 (udp_header->source_port);
    memory_allocate ((void **) &packet->data, packet->length);
    memory_copy (packet->data, udp_header->data, packet->length);

    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "ull: %u",
                         packet->length);

    packet_list_type **packet_list_pointer = &socket->packet_list;
    list_node_insert ((list_type **) packet_list_pointer,
                      (list_type *) packet);
  }
}
