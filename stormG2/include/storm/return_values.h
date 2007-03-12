/* $Id$ */
/* Abstract: Return values */

/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> 
            Anders Ohrt <doa@chaosdev.org> */

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

#ifndef __STORM_RETURN_VALUES_H__
#define __STORM_RETURN_VALUES_H__

/* External error numbers. */

enum 
{
  /* The system call succeeded. */

  STORM_RETURN_SUCCESS = 0,

  /* The code for the specified system call isn't finished yet... */

  STORM_RETURN_FUNCTION_UNFINISHED,

  /* We ran out of memory. */

  STORM_RETURN_OUT_OF_MEMORY,

  /* Access to the given action was denied for some reason. */

  STORM_RETURN_ACCESS_DENIED,

  /* Whatever you requested, it is busy. ;) */

  STORM_RETURN_BUSY,

  /* A limit has been overrun. */

  STORM_RETURN_LIMIT_OVERRUN,

  /* The current thread is the new thread. (Returned by
     system_call_thread_create) */

  STORM_RETURN_THREAD_NEW,

  /* The current thread is the old thread. (Returned by
     system_call_thread_create) */
  
  STORM_RETURN_THREAD_OLD,

  /* The DMA channel specified does not exist. */

  STORM_RETURN_INVALID_DMA_CHANNEL,

  /* Some, or all, of the sections was not page aligned. */

  STORM_RETURN_UNALIGNED_SECTION,

  /* The requested mailbox was not available. */

  STORM_RETURN_MAILBOX_UNAVAILABLE,

  /* The message could not be put in the mailbox since it was full. */

  STORM_RETURN_MAILBOX_FULL,

  /* The message was too large to fit into the given buffer. */

  STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE,

  /* The mailbox was empty. */

  STORM_RETURN_MAILBOX_EMPTY,

  /* The protocol was not available. (service_get) */

  STORM_RETURN_PROTOCOL_UNAVAILABLE,

  /* All the services didn't fit into the service_parameter_type
     structure. (service_get) */

  STORM_RETURN_TOO_MANY_SERVICES,

  /* We tried to memory_deallocate () a pointer that wasn't
     referencing to any valid block. */

  STORM_RETURN_MEMORY_NOT_ALLOCATED,

  /* One or more of the input arguments was invalid. */

  STORM_RETURN_INVALID_ARGUMENT,

  /* One or more of the sections were placed outside the limits of the
     process addressing space. */

  STORM_RETURN_SECTION_MISPLACED,
};

#endif /* !__STORM_RETURN_VALUES_H__ */
