/* $Id$ */
/* Abstract: Function prototypes and structures used by the process
   support. */

/* Author: Per Lundberg <plundis@chaosdev.org>
           Henrik Hallin <hal@chaosdev.org> */

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

#ifndef __STORM_GENERIC_PROCESS_H__
#define __STORM_GENERIC_PROCESS_H__

#include <storm/process.h>
#include <storm/generic/list.h>
#include <storm/generic/types.h>
#include <storm/current-arch/defines.h>

/* Type definitions. */

typedef struct
{
  LIST_HEADER;

  /* A list of all the threads belonging to this process. */

  list_type *thread_list;

  /* The process ID for the given process. */

  process_id_type id;

  /* Process name. */

  char *name;

  /* Priority. */

  u32 priority;

  /* Number of threads this process has. */

  unsigned int number_of_threads;
} process_type;

/* External variables. */

extern process_type *kernel_process;

/* Function prototypes. */

extern void process_init (void);

#endif /* !__STORM_GENERIC_PROCESS_H__ */
