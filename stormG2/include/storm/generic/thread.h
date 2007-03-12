/* $Id$ */
/* Abstract: Thread functions. */

/* Authors: Henrik Hallin <hal@chaosdev.org>,
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

#ifndef __STORM_GENERIC_THREAD_H__
#define __STORM_GENERIC_THREAD_H__

#include <storm/generic/types.h>
#include <storm/generic/process.h>
#include <storm/current-arch/cpu.h>

/* A thread. */

typedef struct
{
  LIST_HEADER;

  /* The cpu_task contains all the process-specific task data
     (register values, etc) */
  
  cpu_task_type *cpu_task;

  /* Pointer to the process owning this thread. */

  process_type *process;
  
  /* Thread ID. */

  thread_id_type id;

  /* Priority of the thread. */
  
  u32 priority;

  /* Memory usage and other information regarding memory. */

  u32 stack_pages;
  u32 allocated_pages;

  /* State of process. */  

  state_type state;         

  /* The number of timeslices this process has got. */

  u64 timeslices;

  /* The name of this thread. */
  
  char *name;

  /* When a new thread is spawned, this flag will be set for the new
     thread. */

  bool new_thread;

  /* This is not the same as EIP; this contains the real instruction
     pointer. EIP always points into the dispatcher because of the way
     our implementation of task-switching is done. */

  u32 instruction_pointer;
} thread_type;

/* Function prototypes. */

extern void thread_init (void);

#endif /* !__STORM_GENERIC_THREAD_H__ */
