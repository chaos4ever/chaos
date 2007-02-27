/* $Id$ */
/* Abstract: IPv4 prototypes and structure definitions. */
/* Author: Per Lundberg <plundis@chaosdev.org>. */

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

#ifndef __IPV4_H__
#define __IPV4_H__

#include "config.h"

/* Structures. */
/* IP protocol types. */

enum
{
  /* Dummy protocol for TCP. */

  IP_PROTOCOL_IP = 0,

  /* IPv6 Hop-by-Hop options. */

  IP_PROTOCOL_HOPOPTS = 0,

  /* Internet Control Message Protocol. */

  IP_PROTOCOL_ICMP = 1,

  /* Internet Group Management Protocol. */

  IP_PROTOCOL_IGMP = 2,

  /* IPIP tunnels (older KA9Q tunnels use 94). */

  IP_PROTOCOL_IPIP = 4,

  /* Transmission Control Protocol. */

  IP_PROTOCOL_TCP = 6,

  /* Exterior Gateway Protocol. */

  IP_PROTOCOL_EGP = 8,

  /* PUP protocol. */

  IP_PROTOCOL_PUP = 12,

  /* User Datagram Protocol. */

  IP_PROTOCOL_UDP = 17,

  /* XNS IDP protocol. */

  IP_PROTOCOL_IDP = 22,

  /* SO Transport Protocol Class 4. */

  IP_PROTOCOL_TP = 29,

  /* IPv6 header. */

  IP_PROTOCOL_IPV6 = 41,

  /* IPv6 routing header. */

  IP_PROTOCOL_ROUTING = 43,

  /* IPv6 fragmentation header. */

  IP_PROTOCOL_FRAGMENT = 44,

  /* Reservation Protocol. */

  IP_PROTOCOL_RSVP = 46,

  /* General Routing Encapsulation. */

  IP_PROTOCOL_GRE = 47,

  /* Encapsulating security payload. */

  IP_PROTOCOL_ESP = 50,

  /* Authentication header. */

  IP_PROTOCOL_AH = 51,

  /* ICMPv6. */

  IP_PROTOCOL_ICMPV6 = 58,

  /* IPv6 no next header. */

  IP_PROTOCOL_NONE = 59,

  /* IPv6 destination options. */

  IP_PROTOCOL_DSTOPTS = 60,

  /* Multicast Transport Protocol. */

  IP_PROTOCOL_MTP = 92,

  /* Encapsulation Header. */

  IP_PROTOCOL_ENCAP = 98,

  /* Protocol Independent Multicast. */

  IP_PROTOCOL_PIM = 103,

  /* Compression Header Protocol. */

  IP_PROTOCOL_COMP = 108,

  /* Raw IP packets. */

  IP_PROTOCOL_RAW = 255,
};

/* An IP header. See RFC 791 for more information. */

typedef struct
 {
   /* Header length, in u32:s. */

   u8 header_length: 4;
   u8 version: 4;
   u8 type_of_service;
   u16 total_length;
   u16 id;
   u16 fragment_offset;
   u8 time_to_live;
   u8 protocol;

   /* Two-complement additive checksum. */

   u16 checksum;
   u32 source_address;
   u32 destination_address;

   /* Options start here. */

   u8 options[0];

   /* After the options, the data follows. This element can only be
      used if no options are used, of course. */

   u8 data[0];
} __attribute__ ((packed)) ipv4_header_type;

/* IP otions. */
/* FIXME: Clean up this crap. */

typedef struct 
{
  /* Saved first hop address. */

  u32 first_hop_address;
  u8 optlen;
  u8 srr;
  u8 rr;
  u8 ts;

  /* Set by setsockopt? */

  u8 is_setbyuser: 1;

  /* Options in __data, rather than skb. */

  u8 is_data: 1;

  /* Strict source route. */

  u8 is_strictroute: 1;

  /* Packet destination address was our one. */

  u8 srr_is_hit: 1;

  /* IP checksum more not valid. */

  u8 is_changed: 1;

  /* Need to record addr of outgoing device. */

  u8 rr_needaddr: 1;

  /* Need to record timestamp. */

  u8 ts_needtime: 1;

  /* Need to record addr of outgoing device. */

  u8 ts_needaddr: 1;
  u8 router_alert;
  u8 __pad1;
  u8 __pad2;

  /* Data start here. */

  u8 data[0]; 
} __attribute__ ((packed)) ipv4_options_type;

/* A generic IP header. */

typedef struct
{
  ipv4_header_type header;
  ipv4_options_type options;

  /* Data starts here. */

  u8 data[0];
} __attribute__ ((packed)) ipv4_packet_type;

/* A protocol type definition. This provides the modular core of the
   IPv4 server. */

typedef struct
{
  void (*function)(ipv4_interface_type *interface,
                   ipv4_ethernet_header_type *ethernet_header, int length,
                   mailbox_id_type output_mailbox_id);
  u8 type;
} ipv4_protocol_type;

/* A node in the IPv4 interface list. */

typedef struct
{
  struct ipv4_interface_list_type *next;
  ipv4_interface_type *interface;
  ipc_structure_type *ethernet_structure;
} ipv4_interface_list_type;

/* Global variables. */

extern log_structure_type log_structure;
extern ipv4_interface_list_type *interface_list;
extern mutex_type interface_list_mutex;
extern u8 ethernet_broadcast[];

/* Function prototypes. */

extern void ipv4_header_create 
  (u32 destination, u32 source, u8 protocol_type,
   unsigned int length, ipv4_header_type *ipv4_header);

extern void ipv4_ethernet_header_create
  (void *destination, void *source, u16 protocol_type,
   ipv4_ethernet_header_type *buffer);

extern u16 ipv4_checksum (u16 *data, unsigned int length);
  
#endif /* !__IPV4_H__ */
