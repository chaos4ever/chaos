/* $Id$ */
/* Abstract: Log protocol. */
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

#ifndef __LIBRARY_IPC_LOG_H__
#define __LIBRARY_IPC_LOG_H__

#include <ipc/protocols.h>

/* Defines and enumerations. */

#define IPC_LOG_MAX_MESSAGE_LENGTH      256
#define IPC_LOG_MAX_CLASS_LENGTH        256

enum
{
  /* Print a message to the log. The type of the message is specified in
     ipc_log_print_type. */

  IPC_LOG_PRINT = (IPC_PROTOCOL_LOG << 16),
};

/* Type definitions. */

typedef struct
{
  char message[IPC_LOG_MAX_MESSAGE_LENGTH];
  char log_class[IPC_LOG_MAX_CLASS_LENGTH];

  /* How urgent is this log message? 0 is most urgent, and higher
     numbers are of less importance. */

  unsigned int urgency;
} ipc_log_print_type;

#endif /* !__LIBRARY_IPC_LOG_H__ */
