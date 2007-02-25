/* $Id$ */
/* Abstract: Protocol used for communicating with block devices. */
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

#ifndef __LIBRARY_IPC_BLOCK_H__
#define __LIBRARY_IPC_BLOCK_H__

#include <ipc/protocols.h>

enum
{
  IPC_BLOCK_READ = (IPC_PROTOCOL_BLOCK << 16),
  IPC_BLOCK_WRITE,
  IPC_BLOCK_GET_INFO,
};

/* This structure is passed into IPC_BLOCK_READ. */

typedef struct
{
  unsigned int start_block_number;
  unsigned int number_of_blocks;
} ipc_block_read_type;

/* IPC_BLOCK_GET_INFO returns this structure. */

typedef struct
{
  unsigned int block_size;
  unsigned int number_of_blocks;
  bool writable;
  bool readable;
  char label[64];
} ipc_block_info_type;

#endif /* !__LIBRARY_IPC_BLOCK_H__ */
