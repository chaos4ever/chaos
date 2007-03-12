/* $Id$ */
/* Abstract: Definitions of task states. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __STORM_STATE_H__
#define __STORM_STATE_H__

/* FIXME: Define exactly which states are needed. */

enum
{
  /* The thread is ready to be dispatched. */

  STATE_DISPATCH = 0,

  /* The thread is waiting to have something sent. */

  STATE_MAILBOX_SEND,

  /* The thread is waiting for something to be received. */

  STATE_MAILBOX_RECEIVE,

  /* The thread is blocked on a kernel mutex. */

  STATE_MUTEX_KERNEL,

  /* The thread is blocked on a user mutex. */
  
  STATE_MUTEX_USER,

  /* The thread is an idle thread, and is thus idle. :-) */

  STATE_IDLE,

  /* Thread is in a zombie state and is waiting to be removed by the
     idle thread. */

  STATE_ZOMBIE,

  /* The thread is waiting for an interrupt. */

  STATE_INTERRUPT_WAIT,

  /* The thread is under creation. */

  STATE_UNREADY,
  
  /* The thread is sleeping unconditionally. */

  STATE_SLEEP,

  /* The thread is blocked, waiting for a kernel log event. */

  STATE_LOG_READ,

  /* The thread is blocked on its parent. */

  STATE_BLOCKED_PARENT,

  /* The thread is blocked during bootup. */

  STATE_BLOCKED_BOOT,
};

#endif /* !__STORM_STATE_H__ */
