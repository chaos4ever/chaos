/* $Id$ */
/* Abstract: Support for the Address Resolution Protocol. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development */

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
#include "arp.h"

/* The ARP cache is a linked list with an entry for each host the
   machine has been talking to recently. */

static arp_cache_entry_type *arp_cache = NULL;
static mutex_type arp_mutex = MUTEX_UNLOCKED;

/* Get the ethernet address for the given IP address. */

bool arp_ip_to_ethernet_address (u32 ip_address, u8 ethernet_address[])
{
  arp_cache_entry_type *entry;

  mutex_wait (arp_mutex);

  entry = arp_cache;
  while (entry != NULL)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "%s: %lX %lX %p", __FUNCTION__,
                         entry->ip_address, ip_address, entry);

    if (entry->ip_address == ip_address)
    {
      memory_copy (ethernet_address, entry->ethernet_address,
                   IPV4_ETHERNET_ADDRESS_LENGTH);
      mutex_signal (arp_mutex);
      return TRUE;
    }

    entry = (arp_cache_entry_type *) entry->next;
  }

  mutex_signal (arp_mutex);
  return FALSE;
}

/* Insert an entry into the list. */

void arp_insert_entry (u32 ip_address, u8 ethernet_address[])
{
  arp_cache_entry_type *entry;

  memory_allocate ((void **) &entry, sizeof (arp_cache_entry_type));
  entry->ip_address = ip_address;
  memory_copy (entry->ethernet_address, ethernet_address,
               IPV4_ETHERNET_ADDRESS_LENGTH);
  entry->time = time_get ();

  mutex_wait (arp_mutex);
  entry->next = (struct arp_cache_entry_type *) arp_cache;
  arp_cache = entry;
  mutex_signal (arp_mutex);
}

/* Get the number of entries in the list. */

unsigned int arp_get_number_of_entries (void)
{
  unsigned int counter = 0;
  arp_cache_entry_type *entry = arp_cache;

  while (entry != NULL)
  {
    counter++;
    entry = (arp_cache_entry_type *) entry->next;
  }

  return counter;
}

/* Get an entry by number in the list. */

arp_cache_entry_type *arp_get_entry (unsigned int which)
{
  unsigned int counter = 0;
  arp_cache_entry_type *entry = arp_cache;

  while (entry != NULL && counter < which)
  {
    counter++;
    entry = (arp_cache_entry_type *) entry->next;
  }

  return entry;
}

/* Parse an ARP packet. Called whenever an ARP packet is received. */

void arp_packet_receive 
  (ipv4_interface_type *interface, 
   ipv4_ethernet_header_type *ethernet_header,
   int length __attribute__ ((unused)), mailbox_id_type output_mailbox_id)
{
  arp_packet_type *arp_packet =
    (arp_packet_type *) &ethernet_header->data;
  message_parameter_type message_parameter;
  char tmpstr[16], tmpstr2[16];

  ipv4_address_to_string (tmpstr, arp_packet->target_protocol_address);
  ipv4_address_to_string (tmpstr2, arp_packet->sender_protocol_address);

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "%s: ARP %s: for %s from %s.",
                       arp_packet->opcode == system_byte_swap_u16 (ARP_REQUEST) ?
                       "who-has" : "reply", interface->identification, 
                       tmpstr, tmpstr2);

  /* If this is an ARP reply, save it in our ARP cache. */
  
  if (arp_packet->opcode == system_byte_swap_u16 (ARP_REPLY))
  {
    u8 ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    
    if (!arp_ip_to_ethernet_address (arp_packet->sender_protocol_address, 
                                     ethernet_address))
    {
      arp_insert_entry (arp_packet->sender_protocol_address,
                        arp_packet->sender_hardware_address);
    }
  }

  /* If someone is asking for us, then reply. FIXME: Implement support
     for proxying ARP requests for other hosts, which might sometimes
     be requested. (bridging being the most notable example) */

  else if (arp_packet->opcode == system_byte_swap_u16 (ARP_REQUEST) &&
           arp_packet->target_protocol_address == interface->ip_address)
  {
    char string[16];
    
    ipv4_address_to_string (string, arp_packet->sender_protocol_address);

    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Returning ARP reply to %s. Address %02X:%02X:%02X:%02X:%02X:%02X",
                         string,
                         interface->hardware_address[0],
                         interface->hardware_address[1],
                         interface->hardware_address[2],
                         interface->hardware_address[3],
                         interface->hardware_address[4],
                         interface->hardware_address[5]);

    arp_packet->hardware_address_length = IPV4_ETHERNET_ADDRESS_LENGTH;
    arp_packet->protocol_address_length = 4;
    
    arp_packet->opcode = system_byte_swap_u16 (ARP_REPLY);
    arp_packet->target_protocol_address =
      arp_packet->sender_protocol_address;
    arp_packet->sender_protocol_address =
      interface->ip_address;
    
    memory_copy (arp_packet->target_hardware_address,
                 arp_packet->sender_hardware_address, 6);
    memory_copy (arp_packet->sender_hardware_address,
                 interface->hardware_address, 6);
    
    ipv4_ethernet_header_create (arp_packet->target_hardware_address,
                                 interface->hardware_address,
                                 IPV4_ETHERNET_PROTOCOL_ARP,
                                 ethernet_header);
    
    message_parameter.data = ethernet_header;
    message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
    message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
    message_parameter.length = (sizeof (ipv4_ethernet_header_type) +
                                sizeof (arp_packet_type));
    ipc_send (output_mailbox_id, &message_parameter);
  }
}

/* Send an ARP packet for the given IP address. */

void arp_who_has (u32 ip_address, ipv4_interface_type *interface,
                  ipc_structure_type *ethernet_structure)
{
  ipv4_ethernet_header_type *ethernet_header;
  arp_packet_type *arp_packet;
  message_parameter_type message_parameter;

  memory_allocate ((void **) &ethernet_header, 
                   sizeof (ipv4_ethernet_header_type) + 
                   sizeof (arp_packet_type));
  
  memory_copy (ethernet_header->source_address, interface->hardware_address,
               IPV4_ETHERNET_ADDRESS_LENGTH);
  memory_copy (ethernet_header->destination_address, ethernet_broadcast,
               IPV4_ETHERNET_ADDRESS_LENGTH);
  ethernet_header->protocol_type = 
    system_byte_swap_u16 (IPV4_ETHERNET_PROTOCOL_ARP);
  arp_packet = (arp_packet_type *) &ethernet_header->data;

  /* Ethernet. */

  arp_packet->hardware_address_space = system_byte_swap_u16 (1);

  arp_packet->protocol_address_space = 
    system_byte_swap_u16 (IPV4_ETHERNET_PROTOCOL_IPV4);
  arp_packet->hardware_address_length = IPV4_ETHERNET_ADDRESS_LENGTH;
  arp_packet->protocol_address_length = 4;
  arp_packet->opcode = system_byte_swap_u16 (ARP_REQUEST);
  memory_copy (arp_packet->sender_hardware_address, 
               interface->hardware_address, IPV4_ETHERNET_ADDRESS_LENGTH);
  arp_packet->sender_protocol_address = interface->ip_address;
  arp_packet->target_protocol_address = ip_address;

  message_parameter.data = ethernet_header;
  message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
  message_parameter.length = (sizeof (ipv4_ethernet_header_type) +
                              sizeof (arp_packet_type));
  message_parameter.block = TRUE;
  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);
  memory_deallocate ((void **) &ethernet_header);
}
