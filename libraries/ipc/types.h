/* $Id$ */
/* Abstract: IPC library types. */
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

#ifndef __LIBRARY_IPC_TYPES_H__
#define __LIBRARY_IPC_TYPES_H__

#include <system/system.h>

/* Type definitions. */

typedef struct
{
  mailbox_id_type input_mailbox_id;
  mailbox_id_type output_mailbox_id;
  
  /* FIXME: Fields for vendor stuff and service ID. */

} ipc_structure_type;

#endif /* !__LIBRARY_IPC_TYPES_H__ */
