/* $Id$ */
/* Abstract: IPv4 types. */
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

#ifndef __LIBRARY_IPV4_TYPES_H__
#define __LIBRARY_IPV4_TYPES_H__

#include <system/system.h>

/* This file is a little ugly; we have to include ipv4/ipv4.h in
   the middle, because it needs some of us and we need some of
   it... FIXME: This should be resolved. */

typedef unsigned int ipv4_socket_id_type;
typedef u32 ipv4_address_type;

typedef struct
{
  ipv4_socket_id_type socket_id;
  unsigned int length;
  u8 data[0];
} ipv4_send_type;

typedef struct
{
  ipv4_address_type address;
  u16 port;
} ipv4_receive_type;

typedef struct
{
  ipv4_address_type address;
  u16 port;
  unsigned int protocol;
} ipv4_connect_type;

typedef struct
{
  ipv4_socket_id_type socket_id;
  ipv4_address_type address;
  u16 port;
} ipv4_reconnect_type;

typedef struct
{
  /* FIXME: We can't use the define yet... but we should, in some
     way. */

  u8 ethernet_address[6];
  ipv4_address_type ip_address;
} ipv4_arp_entry_type;

/* IP configuration. */

typedef struct
{
  char identification[IPV4_INTERFACE_IDENTIFICATION_LENGTH];

  /* Is the specified interface up? */

  bool up;

  /* Is the specified interface using DHCP? */

  bool dhcp;
  u32 ip_address;
  u32 netmask;
  u32 gateway;
  u8 hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
} ipv4_interface_type;

#endif /* !__LIBRARY_IPV4_TYPES_H__ */
