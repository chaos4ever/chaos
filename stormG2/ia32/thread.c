/* $Id$ */
/* Abstract: Thread support. */

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

#include <storm/generic/debug.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/id.h>
#include <storm/generic/memory_global.h> 
#include <storm/generic/memory_virtual.h> 
#include <storm/generic/thread.h>
#include <storm/ia32/gdt.h>
#include <storm/state.h>

#define DEBUG TRUE

thread_type *kernel_thread;
unsigned int number_of_threads = 0;

/* Initialise the thread structures and create the kernel thread. */

void thread_init (void)
{
  debug_print ("Initializing thread structures...\n");

  kernel_thread = memory_global_allocate (sizeof (thread_type));
  
  if (kernel_thread == NULL)
  {
    DEBUG_HALT ("Out of memory!");
  }

  /* Set this thread list in the kernel process. */

  kernel_process->thread_list = (list_type *) kernel_thread;

  kernel_thread->cpu_task = memory_global_allocate (sizeof (cpu_task_type));
  
  if (kernel_thread->cpu_task == NULL)
  {
    DEBUG_HALT ("Out of memory!");
  }

  kernel_thread->process = kernel_process;
  kernel_thread->state = STATE_IDLE;
  kernel_thread->id = id_allocate ();
  kernel_thread->cpu_task->cr3 = (u32) kernel_page_directory;
  kernel_thread->name = "Starting up";
  number_of_threads++;

  // FIXME: thread_link (kernel_thread);
  current_thread = kernel_thread;

  DEBUG_MESSAGE (DEBUG, "Created kernel idle thread with ID %u.",
                 kernel_thread->id);

  /* Prepare for taskswitching by loading TR and setting up a TSS for
     the kernel. Must be done after paging is enabled. */
  
  gdt_setup_tss_descriptor (SELECTOR_TSS1 >> 3, kernel_thread->cpu_task, 0,
                            sizeof (cpu_task_type));
  cpu_set_tr (SELECTOR_TSS1);
}
