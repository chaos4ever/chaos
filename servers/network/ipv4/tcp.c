/* $Id$ */
/* Abstract: TCP routines. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#include <ipc/ipc.h>
#include <log/log.h>
#include <memory/memory.h>
#include <string/string.h>
#include <system/system.h>

#include "ipv4.h"
#include "tcp.h"

/* See RFC 793 for information about how the TCP protocol works. */

/* Process a TCP packet. For now, always terminate the connection so
   that the sender gets a 'Connection refused'. (Just dropping the
   packets is rather unpolite IMO)

   TODO: Try to make this code look better.. it's a mess right now. */

void tcp_packet_receive (ipv4_interface_type *interface,
                         ipv4_ethernet_header_type *old_ethernet_header,
                         int length __attribute__ ((unused)),
                         mailbox_id_type output_mailbox_id)
{
  u32 *packet;

  /* Those point into the packet we're processing. */

  ipv4_header_type *old_ipv4_header =
    (ipv4_header_type *) &old_ethernet_header->data;
  tcp_header_type *old_tcp_header =
    (tcp_header_type *) ((u8 *) old_ipv4_header + 
                         old_ipv4_header->header_length * 4);

  /* Those point into the new packet we're generating. (Well, not
     right now, but when the packet has been allocated, it will...) */

  ipv4_ethernet_header_type *ethernet_header;
  ipv4_header_type *ipv4_header;
  tcp_header_type *tcp_header;
  tcp_pseudo_header_type tcp_pseudo_header;
  char source_address[256], destination_address[256];
  char tmpstr[256];
  message_parameter_type message_parameter;

  ipv4_address_to_string (source_address, old_ipv4_header->source_address);
  ipv4_address_to_string (destination_address, old_ipv4_header->destination_address);

  string_print (tmpstr, "%s%s%s%s%s%s",
                old_tcp_header->finish ? "FIN " : "",
                old_tcp_header->synchronise ? "SYN " : "",
                old_tcp_header->reset ? "RST " : "",
                old_tcp_header->push ? "PSH " : "",
                old_tcp_header->acknowledge ? "ACK " : "",
                old_tcp_header->urgent ? "URG" : "");

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "TCP packet from: %s:%u to %s:%u (%s)",
                       source_address,
                       system_byte_swap_u16 (old_tcp_header->source_port),
                       destination_address,
                       system_byte_swap_u16 (old_tcp_header->destination_port),
                       tmpstr);

  /* Is anyone listening to this port? If not, politely tell the
     remote that we dropped this packet into the void. */
  /* FIXME: Support this. For now, we treat everything as being in the
     CLOSED state. */

  /* If the 'reset' bit is set, drop this packet silently. */

  if (old_tcp_header->reset == 1)
  {
    log_print (&log_structure, LOG_URGENCY_DEBUG, "Returning home early...");
    return;
  }

  /* Allocate memory for the new packet. */
  /* FIXME: We should really calculate the size needed, or at least
     have some fixed maximum. This is ugly. */

  u32 **packet_pointer = &packet;
  memory_allocate ((void **) packet_pointer, 1024);

  ethernet_header = (ipv4_ethernet_header_type *) packet;
  ipv4_header = (ipv4_header_type *) &ethernet_header->data;
  tcp_header = (tcp_header_type *) &ipv4_header->data;

  /* Create an ethernet packet. */

  ipv4_ethernet_header_create (old_ethernet_header->source_address,
                               interface->hardware_address,
                               IPV4_ETHERNET_PROTOCOL_IPV4,
                               ethernet_header);

  /* Create an IPv4 header. */

  ipv4_header_create (old_ipv4_header->source_address, interface->ip_address,
                      IP_PROTOCOL_TCP, sizeof (tcp_header_type),
                      ipv4_header);

  memory_set_u8 ((u8 *) tcp_header, 0, sizeof (tcp_header_type));

  if (old_tcp_header->acknowledge == 0)
  {
    tcp_header->sequence_number = 0;
    tcp_header->acknowledgement_number =
      system_byte_swap_u32 (system_byte_swap_u32
                            (old_tcp_header->sequence_number) + 1);
    /* FIXME:
        old_ipv4_header->total_length - (old_ipv4_header->header_length * 4) -
        (old_tcp_header->data_offset * 4)));

    */
                                      
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "%u", 
                         system_byte_swap_u16 (old_ipv4_header->total_length) -
                         (old_ipv4_header->header_length * 4) -
                         (old_tcp_header->data_offset * 4));

    tcp_header->acknowledge = 1;
  }
  else
  {
    tcp_header->sequence_number = old_tcp_header->acknowledgement_number;
    tcp_header->acknowledge = 0;
  }
  
  tcp_header->reset = 1;

  tcp_header->source_port = old_tcp_header->destination_port;
  tcp_header->destination_port = old_tcp_header->source_port;
  tcp_header->data_offset = sizeof (tcp_header_type) / 4;

  memory_set_u8 ((u8 *) &tcp_pseudo_header, 0,
                 sizeof (tcp_pseudo_header_type));
  tcp_pseudo_header.source_address = ipv4_header->source_address;
  tcp_pseudo_header.destination_address = ipv4_header->destination_address;
  tcp_pseudo_header.protocol_type = ipv4_header->protocol;
  tcp_pseudo_header.tcp_length = tcp_header->data_offset;

  tcp_header->checksum = 0;
  tcp_header->checksum =
    ipv4_checksum ((u16 *) &tcp_pseudo_header, sizeof (tcp_pseudo_header_type)) +
    ipv4_checksum ((u16 *) tcp_header, sizeof (tcp_header_type));

  /* Let's kick shell! */

  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
  message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
  message_parameter.length = (sizeof (ipv4_ethernet_header_type) +
                              sizeof (ipv4_header_type) +
                              sizeof (tcp_header_type));
  message_parameter.data = packet;
  ipc_send (output_mailbox_id, &message_parameter);

  memory_deallocate ((void **) packet_pointer);
}
