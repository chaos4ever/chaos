/* $Id$ */
/* Abstract: ICMP prototypes. */
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
   USA */

#ifndef __ICMP_H__
#define __ICMP_H__

enum
{
  ICMP_ECHO_REPLY = 0,
  ICMP_ECHO_REQUEST = 8,
};

/* The ICMP header type */

typedef struct
{
  u8 type;
  u8 data[0];
} __attribute__ ((packed)) icmp_header_type;

/* An ICMP echo packet. */

typedef struct
{
  icmp_header_type header;
  u8 code;
  u16 checksum;
  u16 identifier;
  u16 sequence_number;
  u8 data[0];
} __attribute__ ((packed)) icmp_echo_type;

/* Function prototypes. */

extern void icmp_packet_receive (ipv4_interface_type *interface,
                                 ipv4_ethernet_header_type *ethernet_header,
                                 int length,
                                 mailbox_id_type output_mailbox_id);

#endif /* !__ICMP_H__ */
