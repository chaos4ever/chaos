/* $Id$ */
/* Abstract: IPv4 protocol */
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

#ifndef __LIBRARY_IPC_IPV4_H__
#define __LIBRARY_IPC_IPV4_H__

enum
{
  IPC_IPV4_INTERFACE_CONFIGURE = (IPC_PROTOCOL_IPV4 << 16),
  IPC_IPV4_INTERFACE_QUERY,

  /* FIXME: Think about how this should really work. */
  
  IPC_IPV4_CONNECT,
  IPC_IPV4_RECONNECT,
  IPC_IPV4_LISTEN,
  IPC_IPV4_SEND,
  IPC_IPV4_RECEIVE,
  IPC_IPV4_SET_HOST_NAME,
  IPC_IPV4_GET_HOST_NAME,
  IPC_IPV4_INTERFACE_GET_AMOUNT,
  IPC_IPV4_INTERFACE_GET_NUMBER,
  IPC_IPV4_SET_FLAGS,
  IPC_IPV4_GET_FLAGS,
  IPC_IPV4_ARP_GET_AMOUNT,
  IPC_IPV4_ARP_GET_NUMBER,
};

enum
{
  IPC_IPV4_PROTOCOL_UDP,
  IPC_IPV4_PROTOCOL_TCP
};

/* Flags. */

enum
{
  IPC_IPV4_FLAG_FORWARD = (1 << 0),
};

#endif /* !__LIBRARY_IPC_IPV4_H__ */
