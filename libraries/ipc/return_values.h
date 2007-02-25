/* $Id$ */
/* Abstract: IPC library return values. */
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

#ifndef __LIBRARY_IPC_RETURN_VALUES__
#define __LIBRARY_IPC_RETURN_VALUES__

enum
{
  /* The function call completed successfully. */

  IPC_RETURN_SUCCESS,

  /* The system call ran out of memory. */

  IPC_RETURN_OUT_OF_MEMORY,
  
  /* One or more of the input arguments was invalid. */

  IPC_RETURN_INVALID_ARGUMENT,

  /* A connection to this service was requested. */

  IPC_RETURN_CONNECTION_REQUESTED,

  /* The services didn't fit into the data
     structure. (ipc_service_resolv) */

  IPC_RETURN_TOO_MANY_SERVICES,

  /* We failed on a
     system_call_mailbox_create. (ipc_service_establish) */

  IPC_RETURN_FAILED_MAILBOX_CREATE,

  /* We failed on a system_call_mailbox_send. (ipc_service_establish) */

  IPC_RETURN_FAILED_MAILBOX_SEND,

  /* We failed on a
     system_call_mailbox_receive. (ipc_service_connection_request) */

  IPC_RETURN_FAILED_MAILBOX_RECEIVE,

  /* No service of the given type was found. (ipc_service_resolve) */

  IPC_RETURN_SERVICE_UNAVAILABLE,

  /* The requested operation was not allowed. */

  IPC_RETURN_ACCESS_DENIED,

  /* The given mailbox was not available. */

  IPC_RETURN_MAILBOX_UNAVAILABLE,

  /* We did a non-blocking ipc_receive (), and the only data that was
     available was too large. */

  IPC_RETURN_MESSAGE_TOO_LARGE,

  /* We did a non-blocking ipc_receive (), and there was no data in
     the mailbox. */

  IPC_RETURN_MAILBOX_EMPTY,

  /* We did a non-blocking ipc_send (), and the mailbox we addressed
     was full. */

  IPC_RETURN_MAILBOX_FULL,
  
  /* We got an unknown IPC command and could thus not proceed. */

  IPC_RETURN_UNKNOWN_COMMAND,

  /* Something went wrong, and we could not figure out what. (probably
     because of an unimplemented kernel return value). */

  IPC_RETURN_UNKNOWN_ERROR,
};

#endif /* !__LIBRARY_IPC_RETURN_VALUES__ */
