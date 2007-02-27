/* $Id$ */
/* Abstract: UDP prototypes and structure definitions. */
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

#ifndef __UDP_H__
#define __UDP_H__

/* Read more about UDP in RFC 768. */

#include <system/system.h>
#include "socket.h"

/* Defines/enums. */

/* UDP ports. */

enum
{
  UDP_DHCP_REQUEST = 67,
  UDP_DHCP_REPLY = 68,
  UDP_TFTP = 69,
  UDP_RWHO = 513,
};

enum
{
  UDP_RETURN_SUCCESS,
  UDP_RETURN_INVALID_ARGUMENT,
  UDP_RETURN_DESTINATION_UNREACHABLE,
};

/* Type definitions. */

/* An UDP header. */

typedef struct
{
  u16 source_port;
  u16 destination_port;
  u16 length;

  /* The checksum is for all of the packet. */

  u16 check;

  /* Data starts here. */

  u8 data[0];
} __attribute__ ((packed)) udp_header_type;

/* Function prototypes. */

extern void udp_header_create 
  (u16 destination_port, u16 source_port,
   u16 length, udp_header_type *udp_header);

extern void udp_packet_receive
  (ipv4_interface_type *interface,
   ipv4_ethernet_header_type *ethernet_header,
   int length __attribute__ ((unused)), mailbox_id_type output_mailbox_id);

return_type udp_connect 
  (u32 destination, unsigned int destination_port, unsigned int *socket_id);

extern void udp_init (void);

extern return_type udp_send 
  (void *data, unsigned int length, socket_type *socket);

#endif /* !__UDP_H__ */
