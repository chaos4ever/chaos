/* $Id$ */
/* Abstract: DHCP structures and function prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org>. */

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

#ifndef __DHCP_H__
#define __DHCP_H__

#include <ipc/ipc.h>
#include <system/system.h>

#include "ipv4.h"
#include "udp.h"

/* For a full description of how this works, see RFC 2131. */

enum
{
  BOOTREQUEST = 1,
  BOOTREPLY
};

/* Optional tags. */

enum
{
  DHCP_OPTION_REQUESTED_IP = 50,
  DHCP_OPTION_MESSAGE_TYPE = 53,
};

/* DHCP message types. */

enum
{
  DHCPDISCOVER = 1,
  DHCPOFFER,
  DHCPREQUEST,
  DHCPDECLINE,
  DHCPACK,
  DHCPNAK,
  DHCPRELEASE,
  DHCPINFORM
};

/* A standard DHCP message. */

typedef struct
{
  /* Message type. 1 = BOOTREQUEST, 2 = BOOTREPLY. */

  u8 operation;

  /* 1 = 10Base-X. FIXME: Support this field properly. */

  u8 hardware_type;

  u8 header_length;

  /* Set to zero by client and updated by relay agents. */

  u8 hops;

  /* Transaction ID, chosen by client and used to identify the
     session. */

  u32 transaction_id;

  /* Filled in by the client and shows the seconds elapsed since
     the address requisition started. */

  u16 seconds;
  u16 flags;

  /* Only filled in if IP is up. */

  u32 client_ip_address;

  /* Used by BOOTREPLY. */

  u32 assigned_ip_address;
  u32 next_server_ip_address;
  u32 relay_agent_ip_address;
  u8 client_hardware_address[16];

  /* Optional server host name. */

  u8 server_host_name[64];

  /* Boot file name. */

  u8 file[128];
 
  /* Should be 99, 130, 83, 99 */

  u8 magic_cookie[4];

  /* DHCP options are stored right here. */

  u8 dhcp_options[0];
} __attribute__ ((packed)) dhcp_message_type;

typedef struct
{
  /* 53. */

  u8 code;

  /* 1. */

  u8 length;
  u8 type;
} __attribute__ ((packed)) dhcp_option_message_type;

typedef struct
{
  /* 50. */

  u8 code;

  /* 1. */

  u8 length;
  u32 ip;
} __attribute__ ((packed)) dhcp_option_requested_ip_type;

typedef struct 
{
  ipv4_ethernet_header_type ethernet_header;
  ipv4_header_type ipv4_header;
  udp_header_type udp_header;
  dhcp_message_type dhcp_message;
} __attribute__ ((packed)) dhcp_packet_type;

/* Function prototypes. */

extern void dhcp_assign (ipv4_interface_type *ipv4_interface,
                         ipc_structure_type *ethernet_structure);

#endif /* !__DHCP_H__ */
