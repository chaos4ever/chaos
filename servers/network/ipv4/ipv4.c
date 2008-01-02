/* $Id$ */
/* Abstract: A server for handling the Internet Protocol. (v4) */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#include "config.h"
#include "arp.h"
#include "dhcp.h"
#include "forward.h"
#include "icmp.h"
#include "ipv4.h"
#include "socket.h"
#include "tcp.h"
#include "udp.h"

/* Globals. */

log_structure_type log_structure;
ipv4_interface_list_type *interface_list = NULL;
mutex_type interface_list_mutex = MUTEX_UNLOCKED;
u8 ethernet_broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/* Locals. */

static ipv4_protocol_type ipv4_protocol[] =
{
  { &icmp_packet_receive, IP_PROTOCOL_ICMP },
  { &tcp_packet_receive, IP_PROTOCOL_TCP },
  { &udp_packet_receive, IP_PROTOCOL_UDP },

  /* This must be here to tell where the saga ends. */

  { NULL, 0 }
};

static char host_name[IPV4_HOST_NAME_LENGTH] = "localhost";

/* An empty tag list. */

static tag_type empty_tag =
{
  0, 0, ""
};

/* FIXME: This is awkward... */

static volatile unsigned int interface_counter = 0;

/* Add an interface to the list of interfaces. This function does NOT
   make a copy of the interface; it generates a reference to the
   already existing one. */

static void interface_add (ipv4_interface_type *interface,
                           ipc_structure_type *ethernet_structure)
{
  ipv4_interface_list_type *entry;
  ipv4_interface_list_type **entry_pointer = &entry;

  memory_allocate ((void **) entry_pointer, sizeof (ipv4_interface_list_type));
  entry->interface = interface;
  entry->ethernet_structure = ethernet_structure;

  /* Add this entry into the list. */

  mutex_wait (interface_list_mutex);
  entry->next = (struct ipv4_interface_list_type *) interface_list;
  interface_list = entry;
  mutex_signal (interface_list_mutex);
}

/* Get the interface matching the given identification. */

static ipv4_interface_type *interface_get (char *identification)
{
  ipv4_interface_list_type *entry;

  mutex_wait (interface_list_mutex);
  entry = interface_list;

  while (entry != NULL)
  {
    if (string_compare (entry->interface->identification, identification) == 0)
    {
      mutex_signal (interface_list_mutex);
      return entry->interface;
    }

    entry = (ipv4_interface_list_type *) entry->next;
  }

  mutex_signal (interface_list_mutex);

  return NULL;
}

/* Get the amount of interfaces used. */

static unsigned int interface_get_amount (void)
{
  ipv4_interface_list_type *entry;
  unsigned int counter = 0;

  mutex_wait (interface_list_mutex);
  entry = interface_list;

  while (entry != NULL)
  {
    counter++;
    entry = (ipv4_interface_list_type *) entry->next;
  }

  mutex_signal (interface_list_mutex);

  return counter;
}

/* Get the data for the given interface number. */

static ipv4_interface_type *interface_get_number (unsigned int number)
{
  ipv4_interface_list_type *entry;
  unsigned int counter = 0;

  mutex_wait (interface_list_mutex);
  entry = interface_list;

  while (entry != NULL && counter < number)
  {
    counter++;
    entry = (ipv4_interface_list_type *) entry->next;
  }

  mutex_signal (interface_list_mutex);
  if (entry == NULL)
  {
    return NULL;
  }
  else
  {
    return entry->interface;
  }
}

/* Calculate the IP checksum for an IP header. */

u16 ipv4_checksum (u16 *data, unsigned int length)
{
  u32 sum = 0;
  unsigned int index;

  /* If we have an odd length, fill the last byte with a zero. */

  if (length % 2 != 0) 
  {
    ((u8 *) data)[length] = 0;
    length++;
  }
  
  length >>= 1;

  for (index = 0; index < length; index++)
  {
    sum += data[index];
    if (sum > 0xFFFF)
    {
      sum -= 0xFFFF;
    }
  }

  return ((~sum) & 0x0000FFFF);
}

/* Create an ethernet header. */

void ipv4_ethernet_header_create (void *destination_address,
                                  void *source_address, 
                                  u16 protocol_type,
                                  ipv4_ethernet_header_type *ethernet_header)
{
  memory_copy (ethernet_header->destination_address, destination_address, 6);
  memory_copy (ethernet_header->source_address, source_address, 6);
  ethernet_header->protocol_type = system_byte_swap_u16 (protocol_type);
}

/* Create an IPv4 header. */

void ipv4_header_create (u32 destination_address, u32 source_address,
                         u8 protocol_type, unsigned int length,
                         ipv4_header_type *ipv4_header)
{
  ipv4_header->destination_address = destination_address;
  ipv4_header->source_address = source_address;
  ipv4_header->protocol = protocol_type;
  ipv4_header->version = 4;
  ipv4_header->time_to_live = 255;
  ipv4_header->header_length = sizeof (ipv4_header_type) / 4;
  ipv4_header->total_length = system_byte_swap_u16 (sizeof (ipv4_header_type) +
                                                    length);
  ipv4_header->type_of_service = 0;
  ipv4_header->id = 0;
  ipv4_header->fragment_offset = 0;
  ipv4_header->checksum = 0;

  /* Calculate the header checksum. */

  ipv4_header->checksum = ipv4_checksum ((u16 *) ipv4_header,
                                         sizeof (ipv4_header_type));
}

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id)
{
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  bool done = FALSE;
  u32 *data;
  u32 **data_pointer = &data;
  unsigned int data_size = 1024;
  
  memory_allocate ((void **) data_pointer, data_size);

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  while (!done)
  {
    message_parameter.data = data;
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    message_parameter.block = TRUE;
    
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (message_parameter.message_class)
    {
      /* Configure this IPv4 interface according to the parameters
         given. */

      case IPC_IPV4_INTERFACE_CONFIGURE:
      {
        ipv4_interface_type *new_interface = (ipv4_interface_type *) data;
        ipv4_interface_type *interface = 
          interface_get (new_interface->identification);

        if (interface == NULL)
        {
          /* FIXME: Notify the other end about this. */
        }
        else
        {
          interface->dhcp = new_interface->dhcp;
          interface->up = new_interface->up;

          if (interface->dhcp)
          {
            if (interface->up)
            {
              /* TODO. */
              
              //            dhcp_assign (interface, &ethernet_structure);
              
              //            log_print (&log_structure, LOG_URGENCY_DEBUG,
              //                       "Got DHCP response...");
            }
          }
          else
          {
            interface->ip_address = new_interface->ip_address;
            interface->netmask = new_interface->netmask;
            interface->gateway = new_interface->gateway;
          }
        }

        break;
      }

      /* Get information about this interface. */

      case IPC_IPV4_INTERFACE_QUERY:
      {
        char *identification = (char *) data;
        ipv4_interface_type *interface = interface_get (identification);
        
        if (interface == NULL)
        {
          message_parameter.length = 0;
        }
        else
        {
          message_parameter.length = sizeof (ipv4_interface_type);
        }

        message_parameter.data = interface;
        message_parameter.block = FALSE;
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        message_parameter.data = data;
        break;
      }

      /* Get information about the number of interfaces currently in
         use. */

      case IPC_IPV4_INTERFACE_GET_AMOUNT:
      {
        unsigned int amount = interface_get_amount ();

        message_parameter.data = &amount;
        message_parameter.length = sizeof (unsigned int);
        message_parameter.block = FALSE;
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        message_parameter.data = data;
        break;
      }

      /* Get information about the interface with the given number. */
      
      case IPC_IPV4_INTERFACE_GET_NUMBER:
      {
        unsigned int *number = (unsigned int *) data;
        ipv4_interface_type *interface = interface_get_number (*number);

        message_parameter.data = interface;
        message_parameter.length = sizeof (ipv4_interface_type);
        message_parameter.block = FALSE;
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        message_parameter.data = data;
        break;
      }

      /* Set/get host name. */

      case IPC_IPV4_SET_HOST_NAME:
      {
        string_copy_max (host_name, message_parameter.data, 
                         IPV4_HOST_NAME_LENGTH);
        break;
      }

      case IPC_IPV4_GET_HOST_NAME:
      {
        message_parameter.data = host_name;
        message_parameter.length = string_length (host_name);
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }
      
      /* Connect to a remote host. */

      case IPC_IPV4_CONNECT:
      {
        ipv4_connect_type *connect = (ipv4_connect_type *) data;
        ipv4_socket_id_type socket_id = -1;

        udp_connect (connect->address, connect->port, &socket_id);
        message_parameter.data = &socket_id;
        message_parameter.length = sizeof (ipv4_socket_id_type);
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);

        break;
      }

      /* Change the target address and/or port of a given
         socket. (Only works with UDP) */
      
      case IPC_IPV4_RECONNECT:
      {
        ipv4_reconnect_type *reconnect = (ipv4_reconnect_type *) data;
        socket_type *socket = socket_find (reconnect->socket_id);

        if (socket != NULL && socket->protocol_type == IPC_IPV4_PROTOCOL_UDP)
        {
          socket->destination_port = reconnect->port;
          socket->destination_address = reconnect->address;
        }

        break;
      }

      /* Listen for incoming connections on the given port (in the
         case of TCP), or incoming data (in the case of UDP). */

      case IPC_IPV4_LISTEN:
      {
        break;
      }

      /* Send data to the given socket. */

      case IPC_IPV4_SEND:
      {
        ipv4_send_type *send = (ipv4_send_type *) data;

        socket_send (send->socket_id, send->length, &send->data);

        /* FIXME: ACK or NACK. */

        break;
      }

      /* Read data from the given socket. */

      case IPC_IPV4_RECEIVE:
      {
        ipv4_socket_id_type *socket_id = (ipv4_socket_id_type *) data;

        socket_receive (*socket_id, ipc_structure.output_mailbox_id);
        break;
      }

      /* Set flags in the IPv4 server. */

      case IPC_IPV4_SET_FLAGS:
      {
        unsigned int *flags = (unsigned int *) data;

        if ((*flags & IPC_IPV4_FLAG_FORWARD) == IPC_IPV4_FLAG_FORWARD)
        {
          forwarding = TRUE;
        }
        else
        {
          forwarding = FALSE;
        }

        break;
      }
      
      /* Get flags. */

      case IPC_IPV4_GET_FLAGS:
      {
        unsigned int flags = 0;

        if (forwarding)
        {
          flags |= IPC_IPV4_FLAG_FORWARD;
        }

        message_parameter.data = &flags;
        message_parameter.length = sizeof (unsigned int);
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }

      /* Read information about/from the ARP table. */

      case IPC_IPV4_ARP_GET_AMOUNT:
      {
        unsigned int amount = arp_get_number_of_entries ();

        message_parameter.data = &amount;
        message_parameter.length = sizeof (unsigned int);
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }

      case IPC_IPV4_ARP_GET_NUMBER:
      {
        unsigned int *which = (unsigned int *) data;
        arp_cache_entry_type *entry = arp_get_entry (*which);
        ipv4_arp_entry_type arp_entry;

        arp_entry.ip_address = entry->ip_address;
        memory_copy (arp_entry.ethernet_address, entry->ethernet_address,
                     IPV4_ETHERNET_ADDRESS_LENGTH);
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }
    }
  }
}

/* Handle the connection to the ethernet service. */

static bool handle_ethernet (mailbox_id_type mailbox_id)
{
  ipc_structure_type *ethernet_structure;
  ipc_structure_type **ethernet_structure_pointer = &ethernet_structure;
  message_parameter_type message_parameter;
  u32 *data;
  u32 **data_pointer = &data;
  bool done = FALSE;
  ipv4_interface_type *interface;
  ipv4_interface_type **interface_pointer = &interface;
  unsigned int data_size = 1024;

  memory_allocate ((void **) data_pointer, data_size);
  memory_allocate ((void **) interface_pointer, sizeof (ipv4_interface_type));
  memory_allocate ((void **) ethernet_structure_pointer, sizeof (ipc_structure_type));

  memory_set_u8 ((u8 *) interface, 0, sizeof (ipv4_interface_type));

  /* Set name. */

  system_thread_name_set ("Ethernet packet handler");

  /* Connect to the ethernet service. */
    
  ethernet_structure->output_mailbox_id = mailbox_id;
  if (ipc_service_connection_request (ethernet_structure) != 
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't connect to ethernet service.");
    return FALSE;
  }
  
  /* Read the ethernet hardware address. */

  message_parameter.length = 0;
  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
  message_parameter.message_class = IPC_ETHERNET_ADDRESS_GET;
  message_parameter.data = data;
  message_parameter.block = TRUE;
  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);
  message_parameter.data = &interface->hardware_address;
  message_parameter.length = IPV4_ETHERNET_ADDRESS_LENGTH;
  ipc_receive (ethernet_structure->input_mailbox_id, &message_parameter,
               NULL);

  /* Request ourselves as receiver of IPv4 packets. */

  message_parameter.message_class = IPC_ETHERNET_REGISTER_TARGET;
  message_parameter.length = 4;
  message_parameter.data = data;

  data[0] = IPV4_ETHERNET_PROTOCOL_IPV4;
  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);

  /* ...and ARP. */

  data[0] = IPV4_ETHERNET_PROTOCOL_ARP;
  ipc_send (ethernet_structure->output_mailbox_id, &message_parameter);

  /* FIXME: Handle this in a better way. */

  string_print (interface->identification, "ethernet%u", interface_counter);
  interface_counter++;
  interface_add (interface, ethernet_structure);

  message_parameter.block = TRUE;

  /* Read and process ethernet packets. */
  
  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
    if (ipc_receive (ethernet_structure->input_mailbox_id, &message_parameter,
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (message_parameter.message_class)
    {
      case IPC_ETHERNET_PACKET_RECEIVED:
      {
        ipv4_ethernet_header_type *ethernet_header =
          (ipv4_ethernet_header_type *) data;
        
        switch (system_byte_swap_u16 (ethernet_header->protocol_type))
        {
          /* ARP packet. */
          
          case IPV4_ETHERNET_PROTOCOL_ARP:
          {
            arp_packet_receive (interface, ethernet_header,
                                message_parameter.length,
                                ethernet_structure->output_mailbox_id);
            break;
          }
          
          /* IPv4 packet. */
          
          case IPV4_ETHERNET_PROTOCOL_IPV4:
          {
            int index;
            ipv4_header_type *ipv4_header =
              (ipv4_header_type *) &ethernet_header->data;

            if (!interface->up)
            {
              break;
            }

            /* It could be to our broadcast or IP address. */

            if ((ipv4_header->destination_address & ~interface->netmask) == 0 ||
                (ipv4_header->destination_address & ~interface->netmask) == ~interface->netmask ||
                interface->ip_address == ipv4_header->destination_address)
            {
              /* Check if there's a function specified to take care of
                 this protocol. */
              
              for (index = 0; ipv4_protocol[index].function != NULL;
                   index++)
              {
                if (ipv4_protocol[index].type == ipv4_header->protocol)
                {
                  ipv4_protocol[index].function
                    (interface, ethernet_header,
                     message_parameter.length,
                     ethernet_structure->output_mailbox_id);
                  break;
                }
              }
            }

            /* Check if this packet is destined for the IP address of
               this interface. If it's not, we may forward or drop
               it. */
            
            else
            {
              forward_packet (ethernet_header, message_parameter.length);
            }

          }
        }
        break;
      }    
    }
  }

  return TRUE;
}

/* Main function */

int main (void)
{
  ipc_structure_type ipc_structure;
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;
  unsigned int index;
  time_type seed;
  bool done = FALSE;

  system_call_timer_read ((time_type *) &seed);
  random_init (seed);

  /* Initialise the different subsystems of the server. */

  socket_init ();
  udp_init ();

  /* Set our name and create the service. */

  system_process_name_set ("ipv4");
  system_thread_name_set ("Initialising");

  /* Wait for cards to probe. FIXME: Remove this. */

  //  system_sleep (2000);

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);

  /* Check if we have some available ethernet services. */
  
  if (ipc_service_resolve ("ethernet", mailbox_id, &services, 0,
                           &empty_tag) == IPC_RETURN_SUCCESS)
  {
    /* Create one thread per service. */

    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                         "Found %u ethernet interfaces.", services);
    
    for (index = 0; index < services; index++)
    {
      //      system_sleep (2000);

      if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
      {
        if (handle_ethernet (mailbox_id[index]))
        {
          return 0;
        }
        else
        {
          return -1;
        }
      }
    }
  }    

  /* Now, handle the service. */
  
  system_thread_name_set ("Service handler");
  
  /* Create a service. */
  
  if (ipc_service_create ("ipv4", &ipc_structure, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create ipv4 service.");
    return -1;
  }

  system_call_process_parent_unblock ();
  
  while (!done)
  {
    mailbox_id_type reply_mailbox_id;
    
    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;
    
    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_thread_name_set ("Handling connection");
      handle_connection (reply_mailbox_id);
    }
  }    
  
  return 0;
}
