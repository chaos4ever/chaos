/* $Id$ */
/* Abstract: TCP prototypes. */
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

#ifndef __TCP_H__
#define __TCP_H__

/* See RFC 793 for more information about what these are for. */

typedef struct
{
  /* The source port number. */

  u16 source_port;

  /* The destination port number. */

  u16 destination_port;

  /* The sequence number of the first data octet. */

  u32 sequence_number;

  /* If ack is set, contains the value of the next sequence number. */

  u32 acknowledgement_number;
  u16 reserved : 4;

  /* The number of 32 bit words in the TCP header. */

  u16 data_offset : 4;

  /* No more data from sender. */

  u16 finish : 1;

  /* Synchronise sequence numbers. */

  u16 synchronise : 1;

  /* Reset the connection. */

  u16 reset : 1;

  /* Push function. */

  u16 push : 1;

  /* Acknowledgement field significant. */

  u16 acknowledge : 1;

  /* Urgent pointer field signicant. */

  u16 urgent : 1;
  u16 reserved2 : 2;

  /* The number of data octets the sender is willing to accept. */

  u16 window;

  /* Checksum of header, data and the 'pseudo header (see RFC for more
     information). */

  u16 checksum;

  /* Points to the sequence number of the octet following the urgent
     data. */

  u16 urgent_pointer;

  /* End of header; start of options. */

} __attribute__ ((packed)) tcp_header_type;

/* Pseudo header used for calculating checksums. */

typedef struct
{
  u32 source_address;
  u32 destination_address;
  u8 protocol_type;

  /* Must be zero! */

  u8 zero;
  u16 tcp_length;
} __attribute__ ((packed)) tcp_pseudo_header_type;

/* Function prototypes. */

extern void tcp_packet_receive (ipv4_interface_type *interface,
                                ipv4_ethernet_header_type *old_ethernet_header,
                                int length __attribute__ ((unused)),
                                mailbox_id_type output_mailbox_id);

#endif /* !__TCP_H__ */
