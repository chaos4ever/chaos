/* $Id$ */
/* Abstract: IPC library functions. */
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

#ifndef __LIBRARY_IPC_FUNCTIONS_H__
#define __LIBRARY_IPC_FUNCTIONS_H__

#include <system/system.h>

extern return_type ipc_send
  (mailbox_id_type mailbox_id,
  message_parameter_type *message_parameter);

extern return_type ipc_receive 
  (mailbox_id_type mailbox_id,
  message_parameter_type *message_parameter,
  unsigned int *buffer_size);

extern return_type ipc_service_create
  (const char *service_name,
  ipc_structure_type *ipc_structure, tag_type *tag);

extern return_type ipc_service_connection_wait
  (ipc_structure_type *ipc_structure);

extern return_type ipc_service_connection_request
  (ipc_structure_type *ipc_structure);

extern return_type ipc_service_resolve
  (const char *service_name,  mailbox_id_type *mailbox_id,
   unsigned int *number_of_mailbox_ids, time_type timeout,
   tag_type *tag);

extern return_type ipc_connection_establish
  (ipc_structure_type *ipc_structure);

extern return_type ipc_connection_close 
  (ipc_structure_type *ipc_structure, bool notify);

#endif /* !__LIBRARY_IPC_FUNCTIONS_H__ */
