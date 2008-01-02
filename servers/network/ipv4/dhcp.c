/* $Id$ */
/* Abstract: DHCP (client) support. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#include "config.h"
#include "ipv4.h"
#include "udp.h"
#include "dhcp.h"

/* DHCP functions. */

void dhcp_assign (ipv4_interface_type *ipv4_interface,
                  ipc_structure_type *ethernet_structure)
{
  dhcp_packet_type dhcp_packet;
  dhcp_option_message_type dhcp_option_message;
  dhcp_option_requested_ip_type dhcp_option_requested_ip;
  u8 *dhcp_options;
  dhcp_packet_type *dhcp_reply;
  message_parameter_type message_parameter;
  u32 *data;
  u32 **data_pointer = &data;
  unsigned int data_size = 4096;
  u32 transaction_id;
  unsigned int index = 0;

  /* Get a random value to use as transaction ID. */

  transaction_id = random (0xFFFF);
 
  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "ID: %u.", (unsigned int) transaction_id);

  /* Allocate memory for the reply. */

  memory_allocate ((void **) data_pointer, data_size);
  dhcp_reply = (dhcp_packet_type *) data;

  /* Zap the DHCP packet over to the enemy. */
  
  memory_set_u8 ((u8 *) &dhcp_packet, 0, sizeof (dhcp_packet_type));

  /* Request IP settings from a DHCP server. */

  ipv4_ethernet_header_create (ethernet_broadcast,
                               ipv4_interface->hardware_address,
                               IPV4_ETHERNET_PROTOCOL_IPV4,
                               &dhcp_packet.ethernet_header);
  
  ipv4_header_create (MAX_U32, 0, IP_PROTOCOL_UDP, sizeof (udp_header_type) +
                      sizeof (dhcp_message_type) +
                      sizeof (dhcp_option_message_type),
                      &dhcp_packet.ipv4_header);
  
  udp_header_create (UDP_DHCP_REQUEST, UDP_DHCP_REQUEST,
                     sizeof (dhcp_message_type) + 
                     sizeof (dhcp_option_message_type),
                     &dhcp_packet.udp_header);

  dhcp_packet.dhcp_message.operation = BOOTREQUEST;
  dhcp_packet.dhcp_message.transaction_id = transaction_id;

  /* 10Base-X. */
  /* FIXME */
 
  dhcp_packet.dhcp_message.hardware_type = 1;                          

  /* Length of hardware address. */

  dhcp_packet.dhcp_message.header_length = IPV4_ETHERNET_ADDRESS_LENGTH;
  dhcp_packet.dhcp_message.magic_cookie[0] = 99;
  dhcp_packet.dhcp_message.magic_cookie[1] = 130;
  dhcp_packet.dhcp_message.magic_cookie[2] = 83;
  dhcp_packet.dhcp_message.magic_cookie[3] = 99;

  dhcp_option_message.code = DHCP_OPTION_MESSAGE_TYPE;
  dhcp_option_message.length = 1;
  dhcp_option_message.type = DHCPDISCOVER;

  memory_copy (&dhcp_packet.dhcp_message.client_hardware_address,
               &ipv4_interface->hardware_address, 6);

  /* Send this packet to the ethernet server. */

  memory_set_u8 ((u8 *) data, 0, data_size);
  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
  message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
  message_parameter.length = (sizeof (dhcp_packet) +
                              sizeof (dhcp_option_message_type));
  message_parameter.data = data;
  message_parameter.block = TRUE;

  memory_copy ((u8 *) data, &dhcp_packet, sizeof (dhcp_packet_type));
  memory_copy ((u8 *) data + sizeof (dhcp_packet_type),
                &dhcp_option_message, sizeof (dhcp_option_message_type));
  
  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);

  /* Wait for a reply. */

  memory_set_u8 ((u8 *) data, 0, data_size);

  do
  {
    message_parameter.length = data_size;
    message_parameter.message_class = IPC_CLASS_NONE;
    ipc_receive (ethernet_structure->input_mailbox_id, &message_parameter,
                 &data_size);
  }
  while (!(message_parameter.message_class == IPC_ETHERNET_PACKET_RECEIVED &&
           (dhcp_reply->ethernet_header.protocol_type ==
            system_byte_swap_u16 (IPV4_ETHERNET_PROTOCOL_IPV4)) &&
           dhcp_reply->ipv4_header.protocol == IP_PROTOCOL_UDP &&
           (dhcp_reply->udp_header.destination_port == 
            system_byte_swap_u16 (UDP_DHCP_REPLY)) &&
           dhcp_reply->dhcp_message.transaction_id == transaction_id));

  /* Tell the DHCP server we want this IP address. But first, save
     information about the IP, netmask, and gateway addresses. */

  /* Parse the DHCP options. */

  dhcp_options =
    (u8 *) &((dhcp_packet_type *) data)->dhcp_message.dhcp_options;

  /* End of options. */

  while (dhcp_options[index] != 255)
  {
    /* Ignore any zero bytes. */

    if (dhcp_options[index] == 0)
    {
      index++;
      continue;
    }

    switch (dhcp_options[index])
    {
      /* Netmask. */

      case 1:
      {
        ipv4_interface->netmask = *((u32 *) (&dhcp_options[index + 2]));
        break;
      }

      /* Gateway. */

      case 3:
      {
        ipv4_interface->gateway = *((u32 *) (&dhcp_options[index + 2]));
        break;
      }
    }

    /* Go to next option */
    
    dhcp_options += dhcp_options[index + 1] + 2;
  }

  /* Tell the server we want this address. */

  ipv4_header_create (MAX_U32, 0, IP_PROTOCOL_UDP, sizeof (udp_header_type) +
                      sizeof (dhcp_message_type) +
                      sizeof (dhcp_option_message_type) +
                      sizeof (dhcp_option_requested_ip),
                      &dhcp_packet.ipv4_header);
  
  udp_header_create (UDP_DHCP_REQUEST, UDP_DHCP_REQUEST,
                     sizeof (dhcp_message_type) +
                     sizeof (dhcp_option_message_type) +
                     sizeof (dhcp_option_requested_ip),
                     &dhcp_packet.udp_header);

  dhcp_option_message.code = DHCP_OPTION_MESSAGE_TYPE;
  dhcp_option_message.length = 1;
  dhcp_option_message.type = DHCPREQUEST;

  dhcp_option_requested_ip.code = DHCP_OPTION_REQUESTED_IP;
  dhcp_option_requested_ip.length = 4;
  dhcp_option_requested_ip.ip =
    ((dhcp_packet_type *) data)->dhcp_message.assigned_ip_address;

  /* Send this packet to the ethernet server. */

  message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
  message_parameter.length = (sizeof (dhcp_packet) +
                              sizeof (dhcp_option_message_type) +
                              sizeof (dhcp_option_requested_ip_type));
  
  memory_copy ((u8 *) data, &dhcp_packet, sizeof (dhcp_packet_type));

  memory_copy (((u8 *) data) + sizeof (dhcp_packet_type),
               &dhcp_option_message, sizeof (dhcp_option_message_type));
  
  memory_copy (((u8 *) data) + sizeof (dhcp_packet_type) +
               sizeof (dhcp_option_message_type),
               &dhcp_option_requested_ip,
               sizeof (dhcp_option_requested_ip_type));

  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);

  /* ...and wait for an answer */

  memory_set_u8 ((u8 *) data, 0, 1024);

  message_parameter.message_class = IPC_CLASS_NONE;

  do
  {
    system_call_mailbox_receive (ethernet_structure->input_mailbox_id,
                                 &message_parameter);

  }
  while (!((message_parameter.message_class == IPC_ETHERNET_PACKET_RECEIVED &&
            dhcp_reply->ethernet_header.protocol_type ==
            system_byte_swap_u16 (IPV4_ETHERNET_PROTOCOL_IPV4) &&
            dhcp_reply->ipv4_header.protocol == IP_PROTOCOL_UDP &&
            (dhcp_reply->udp_header.destination_port ==
             system_byte_swap_u16 (UDP_DHCP_REPLY)) &&
            dhcp_reply->dhcp_message.transaction_id == transaction_id)));

  ipv4_interface->ip_address = dhcp_option_requested_ip.ip;

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "DHCP response:");
  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "IP address: %lu.%lu.%lu.%lu, netmask: %lu.%lu.%lu.%lu, gateway: %lu.%lu.%lu.%lu", 
                       ipv4_interface->ip_address & 0xFF,
                       (ipv4_interface->ip_address >> 8) & 0xFF, 
                       (ipv4_interface->ip_address >> 16) & 0xFF, 
                       ipv4_interface->ip_address >> 24,
                       ipv4_interface->netmask & 0xFF,
                       (ipv4_interface->netmask >> 8) & 0xFF, 
                       (ipv4_interface->netmask >> 16) & 0xFF, 
                       ipv4_interface->netmask >> 24,
                       ipv4_interface->gateway & 0xFF,
                       (ipv4_interface->gateway >> 8) & 0xFF, 
                       (ipv4_interface->gateway >> 16) & 0xFF, 
                       ipv4_interface->gateway >> 24);

  /* Indicate that this interface is using DHCP. */

  ipv4_interface->dhcp = TRUE;
  ipv4_interface->up = TRUE;
}
