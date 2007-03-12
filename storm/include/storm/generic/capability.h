/* $Id$ */
/* Abstract: Thread capabilities. */ 
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

#ifndef __STORM_GENERIC_CAPABILITY_H__
#define __STORM_GENERIC_CAPABILITY_H__

typedef struct
{
  /* Is this thread allowed to create and delete services? */

  u32 modify_services: 1;

  /* Is this thread allowed to register and unregister I/O ports, DMA
     channels and IRQ levels? */

  u32 modify_hardware : 1;

  /* Is this thread allowed to do thread_control on threads owned by
     someone else? (for example, to modify priority, suspend/resume
     threads, etc) */

  u32 thread_control_others : 1;

  /* Is this thread allowed to kill threads owned by someone else? */

  u32 kill_other_threads : 1;
} capability_type;

#endif /* !__STORM_GENERIC_CAPABILITY_H__ */
