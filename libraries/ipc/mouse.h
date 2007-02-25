/* $Id$ */
/* Abstract: Protocol used for communicating with mouse hardware
   servers. */
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

#ifndef __LIBRARY_IPC_MOUSE_H__
#define __LIBRARY_IPC_MOUSE_H__

enum
{
  /* The mouse position or button state was changed. */

  IPC_MOUSE_EVENT = (IPC_PROTOCOL_MOUSE << 16),

  /* Register ourselves as receiver of mouse events. */

  IPC_MOUSE_REGISTER_TARGET,
  
  /* Unregister ourselves as receiver of mouse events. */

  IPC_MOUSE_UNREGISTER_TARGET,
};

typedef struct
{
  unsigned int x;
  unsigned int y;
  unsigned int button;
} ipc_mouse_event_type;

#endif /* !__LIBRARY_IPC_MOUSE_H__ */
