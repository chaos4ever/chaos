/* $Id$ */
/* Abstract: Console server IPC message types. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development */

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

#ifndef __LIBRARY_IPC_CONSOLE_H__
#define __LIBRARY_IPC_CONSOLE_H__

/* Message types. */

enum 
{
  IPC_CONSOLE_CONNECTION_CLASS_SET = (IPC_PROTOCOL_CONSOLE << 16),
  IPC_CONSOLE_CURSOR_MOVE,
  IPC_CONSOLE_OPEN,
  IPC_CONSOLE_MODE_SET,
  IPC_CONSOLE_CLOSE,
  IPC_CONSOLE_OUTPUT,
  IPC_CONSOLE_RESIZE,
  IPC_CONSOLE_ENABLE_KEYBOARD,
  IPC_CONSOLE_DISABLE_KEYBOARD,
  IPC_CONSOLE_KEYBOARD_EXTENDED,
  IPC_CONSOLE_KEYBOARD_NORMAL,
  IPC_CONSOLE_ENABLE_MOUSE,
  IPC_CONSOLE_DISABLE_MOUSE,

  /* An input event on the keyboard. */

  IPC_CONSOLE_KEYBOARD_EVENT,
  
  /* A mouse event. */

  IPC_CONSOLE_MOUSE_EVENT
};

/* Connection classes. */

enum
{
  IPC_CONSOLE_CONNECTION_CLASS_NONE,
  IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_KEYBOARD,
  IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_MOUSE,
  IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_JOYSTICK,
  IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_VIDEO,
  IPC_CONSOLE_CONNECTION_CLASS_CLIENT,
};

/* Resize the current console. */

typedef struct
{
  unsigned int width;
  unsigned int height;
} ipc_console_resize_type;

/* Physical console attribute type. */

typedef struct
{
  unsigned int width;
  unsigned int height;
  unsigned int depth;
  unsigned int mode_type;
} ipc_console_attribute_type;

#endif /* !__LIBRARY_IPC_CONSOLE_H__ */
