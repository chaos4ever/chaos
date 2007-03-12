/* $Id$ */
/* Abstract: Thread related stuff. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#ifndef __STORM_THREAD_H__
#define __STORM_THREAD_H__

/* Those are used by system_call_thread_control. */

enum
{
  /* Tell the kernel to kill the process unconditionally. */

  THREAD_TERMINATE = 0,

  /* Suspend a process until resumed by THREAD_RESUME. */

  THREAD_SUSPEND,

  /* Resumes a process suspended with THREAD_SUSPEND. */

  THREAD_RESUME,

  /* Puts a process to sleep for a user specified amount of time (in
     milliseconds). */

  THREAD_SLEEP,
  
  /* Set the priority of a process. If U32_MAX is given, it will be
     treated as an "idle" task (i.e. only dispatched when no other
     tasks are running). FIXME: NOT U32_MAX!!!!!!!!!!!!!!!!!!!!!! */

  THREAD_PRIORITY_SET,

  /* The rest are up to the process to handle or not. */
  /* This is to be forward-compatible. */

  THREAD_SHUTDOWN = 4096,

  /* Tells a process to reload its configuration file(s). */

  THREAD_RELOAD_CONFIGURATION,

  /* Tells a process to restart and possibly re-probe for
     hardware. This makes hotswapping easy. */

  THREAD_RESTART,

  /* Tells a process to update its (virtual) screen, possibly because
     of resize. */

  THREAD_UPDATE_SCREEN,

  /* Some kind of fault occured during execution. The handler should
     shutdown the program immediately. */

  THREAD_EXECUTION_FAULT
};

#endif /* !__STORM_THREAD_H__ */
