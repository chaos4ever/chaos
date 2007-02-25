/* $Id$ */
/* Abstract: IPv4 library function prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#ifndef __LIBRARY_IPV4_FUNCTIONS_H__
#define __LIBRARY_IPV4_FUNCTIONS_H__

#include <ipc/ipc.h>
#include <system/system.h>

extern return_type ipv4_init 
  (ipc_structure_type *ipv4_structure, tag_type *tag);

extern return_type ipv4_host_name_get
  (ipc_structure_type *ipv4_structure, char *host_name);

extern return_type ipv4_string_to_binary_ip_address
  (u8 *string, u32 *ip_address);

extern return_type ipv4_connect 
  (ipc_structure_type *ipv4_structure, ipv4_connect_type *ipv4_connect, 
   ipv4_socket_id_type *socket_id);

return_type ipv4_reconnect
  (ipc_structure_type *ipv4_structure, ipv4_reconnect_type *connect);

extern return_type ipv4_send 
  (ipc_structure_type *ipv4_structure, ipv4_send_type *ipv4_send);

extern return_type ipv4_receive
  (ipc_structure_type *ipv4_structure, ipv4_receive_type *receive, 
   ipv4_socket_id_type socket_id, void **data, unsigned int *length);

extern return_type ipv4_set_flags 
  (ipc_structure_type *ipv4_structure, unsigned int flags);

extern return_type ipv4_get_flags
  (ipc_structure_type *ipv4_structure, unsigned int *flags);

extern return_type ipv4_address_to_string
  (char *string, u32 ipv4_address);

#endif /* !__LIBRARY_IPV4_FUNCTIONS_H__ */
