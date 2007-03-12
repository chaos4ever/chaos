/* $Id$ */
/* Abstract: Function prototypes and structures used by the process
   support. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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
   USA */

#ifndef __STORM_GENERIC_PROCESS_H__
#define __STORM_GENERIC_PROCESS_H__

#include <storm/process.h>
#include <storm/current-arch/tss.h>
#include <storm/generic/types.h>
#include <storm/generic/memory_virtual.h>

/* Type definitions. */

typedef struct
{
  struct process_info_type *next;

  /* The process ID for the given process. */

  process_id_type process_id;

  /* Process name. */

  char *name;

  /* A linked list over the threads in this process. */

  tss_list_type *thread_list;
  unsigned int number_of_threads;
} process_info_type;

typedef struct
{
  storm_tss_type *tss;
  struct kernel_child_list_type *next;
} kernel_child_list_type;

/* Function prototypes. */

extern void process_init (void) INIT_CODE;
extern return_type process_create (process_create_type *process_data);
extern return_type process_name_set (char *name);
extern return_type process_parent_unblock(void);
extern process_info_type *process_find (process_id_type process_id);
extern void process_link (process_info_type *process_info);

/* External variables. */

extern storm_tss_type *kernel_tss;
extern process_id_type *process_id_array;
extern unsigned int number_of_processes;
extern process_info_type *process_list;

#endif /* !__STORM_GENERIC_PROCESS_H__ */
