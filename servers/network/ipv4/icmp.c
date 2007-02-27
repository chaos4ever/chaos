/* $Id$ */
/* Abstract: ICMP module. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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
#include "ipv4.h"
#include "icmp.h"

/* Receive an ICMP packet. */

void icmp_packet_receive (ipv4_interface_type *interface,
                          ipv4_ethernet_header_type *ethernet_header,
                          int length,
                          mailbox_id_type output_mailbox_id)
{
  /* Check if we've received an ICMP echo request (aka 'ping').  If so,
     reply. Perhaps send a winnuke too. ;) */
  
  ipv4_header_type *ipv4_header = (ipv4_header_type *) &ethernet_header->data;
  icmp_header_type *icmp_header =
    (icmp_header_type *) ((u8 *) ipv4_header + ipv4_header->header_length * 4);
  message_parameter_type message_parameter;

  switch (icmp_header->type)
  {
    /* A ping request. */

    case ICMP_ECHO_REQUEST:
    {
      icmp_echo_type *icmp_echo = (icmp_echo_type *) icmp_header;
      char source_address[16];
      char destination_address[16];

      ipv4_address_to_string (source_address, ipv4_header->source_address);
      ipv4_address_to_string (destination_address, 
                              ipv4_header->destination_address);
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "%s: ICMP: echo request from %s for %s.",
                           interface->identification,
                           source_address, destination_address);
                       
      /* Reply to this. */
      
      memory_copy (ethernet_header->destination_address,
                   ethernet_header->source_address, 6);
      memory_copy (ethernet_header->source_address, 
                   interface->hardware_address, 6);
      
      icmp_echo->header.type = ICMP_ECHO_REPLY;
      
      /* Since we know the previous checksum, and our changes are very
         minor, we can do the checksum calculation in a little "ugly"
         (but more optimized) way. */
      
      icmp_echo->checksum += ICMP_ECHO_REQUEST - ICMP_ECHO_REPLY;
      ipv4_header->destination_address = ipv4_header->source_address;
      ipv4_header->source_address = interface->ip_address;
      ipv4_header->time_to_live = 255;
      ipv4_header->checksum = 0;
      ipv4_header->checksum = ipv4_checksum ((u16 *) ipv4_header,
                                             sizeof (ipv4_header_type));
      
      message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
      message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
      message_parameter.length = length;
      system_call_mailbox_send (output_mailbox_id, &message_parameter);
      break;
    }
    
    /* A ping reply. */

    case ICMP_ECHO_REPLY:
    {
      break;
    }
  }
}
