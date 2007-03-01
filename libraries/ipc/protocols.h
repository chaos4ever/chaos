/* $Id$ */
/* Abstract: Definition of the protocol types used in the chaos
   system. Also define all the message types those protocols
   support. */
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

#ifndef __LIBRARY_IPC_PROTOCOLS_H__
#define __LIBRARY_IPC_PROTOCOLS_H__

/* Those are in chronologic order. When adding protocols, it must of
   course be put at the end to avoid recompiling everything... */

enum
{
  /* No protocol. */

  IPC_PROTOCOL_NONE,

  /* The general IPC protocol is used for communicating with the
     service mailboxes. */

  IPC_PROTOCOL_GENERAL,

  IPC_PROTOCOL_MOUSE,
  IPC_PROTOCOL_KEYBOARD,
  IPC_PROTOCOL_CONSOLE,
  IPC_PROTOCOL_VIDEO,
  IPC_PROTOCOL_PCI,
  IPC_PROTOCOL_ETHERNET,
  IPC_PROTOCOL_IPV4,
  IPC_PROTOCOL_BLOCK,
  IPC_PROTOCOL_FILE,
  IPC_PROTOCOL_LOG,
  IPC_PROTOCOL_SERIAL,
  IPC_PROTOCOL_SOUND,
};

/* Ugly, but those rely on the previous defines... so we can't do in a
   nicer way. */

#include <ipc/block.h>
#include <ipc/console.h>
#include <ipc/ethernet.h>
#include <ipc/file.h>
#include <ipc/general.h>
#include <ipc/ipv4.h>
#include <ipc/keyboard.h>
#include <ipc/log.h>
#include <ipc/mouse.h>
#include <ipc/pci.h>
#include <ipc/serial.h>
#include <ipc/sound.h>
#include <ipc/video.h>

#endif /* !__LIBRARY_IPC_PROTOCOLS_H__ */
