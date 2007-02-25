/* $Id$ */
/* Abstract: Ethernet library type defintions and function
   prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_IPV4_ETHERNET_H__
#define __LIBRARY_IPV4_ETHERNET_H__

#include <system/system.h>

/* IEEE 802.3 ethernet magic constants. The frame sizes omit the
   preamble and FCS/CRC (frame check sequence). */

#define IPV4_ETHERNET_ADDRESS_LENGTH         6
#define IPV4_ETHERNET_HEADER_LENGTH          14

/* Min. octets in frame minus FCS. */

#define IPV4_ETHERNET_MINIMUM_LENGTH         60  

/* Max. octets in payload. */
            
#define IPV4_ETHERNET_DATA_LENGTH            1500

/* Max. octets in frame minus FCS. */

#define IPV4_ETHERNET_FRAME_LENGTH           1514

/* These are the protocol ID's we support. */

#define IPV4_ETHERNET_PROTOCOL_IPV4          0x0800
#define IPV4_ETHERNET_PROTOCOL_ARP           0x0806
#define IPV4_ETHERNET_PROTOCOL_IPV6          0x86DD

/* This is an ethernet frame header. */
 
typedef struct
{
  u8 destination_address[IPV4_ETHERNET_ADDRESS_LENGTH];
  u8 source_address[IPV4_ETHERNET_ADDRESS_LENGTH];
  u16 protocol_type;
  u8 data[0];
} __attribute__ ((packed)) ipv4_ethernet_header_type;

#endif /* !__LIBRARY_IPV4_ETHERNET_H__ */
