/* $Id$ */
/* Abstract: Packet list. */
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

#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct
{
  struct packet_list *next;
  struct packet_list *previous;

  unsigned int length;
  void *data;
  
  /* The source port and address of this packet, since they may be
     just anything for UDP packets. */

  ipv4_address_type source_address;
  u16 source_port;
} packet_list_type;

#endif /* !__PACKET_H__ */
