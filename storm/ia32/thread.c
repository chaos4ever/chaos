/* $Id$ */
/* Abstract: Thread routines. Part of the process system. Responsible for
   adding and removing threads under a cluster. */
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

/* FIXME: Move most of this to the generic subdirectory. */

/* Define this as TRUE when debugging this part of the kernel. */

#define DEBUG FALSE

#include <storm/state.h>

#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <storm/generic/mutex.h>

#include <storm/ia32/gdt.h>
#include <storm/ia32/timer.h>
#include <storm/ia32/tss.h>

static tss_list_type **tss_hash;
mutex_kernel_type tss_tree_mutex = MUTEX_UNLOCKED;

volatile u32 number_of_tasks = 0;
storm_tss_type *kernel_tss;

/* A linked list of all the threads in the system. Used by the
   dispatcher. */

tss_list_type *tss_list = NULL;
tss_list_type *idle_tss_node = NULL;

/* The lowest free thread ID. */

volatile thread_id_type free_thread_id = 1;

/* Gets the hash value for the given mailbox ID. */

static unsigned int hash (thread_id_type thread_id)
{
  return (thread_id % limit_thread_hash_entries);
}

/* Initialise the thread code. */

void thread_init (void)
{
  tss_hash = memory_global_allocate (sizeof (tss_list_type *) * 
                                     limit_thread_hash_entries);
  memory_set_u8 ((u8 *) tss_hash, 0, sizeof (tss_list_type *) * 
                 limit_thread_hash_entries);
}

/* Links the given TSS into the TSS list. */

tss_list_type *thread_link_list (tss_list_type **list,
                                 storm_tss_type *tss)
{
  tss_list_type *tss_list_node;

  DEBUG_MESSAGE (DEBUG, "list = %p", list);

  /* Add this entry into the list. */

  tss_list_node = memory_global_allocate (sizeof (tss_list_type));
  tss_list_node->thread_id = tss->thread_id;
  tss_list_node->next = (struct tss_list_type *) (*list);
  tss_list_node->previous = NULL;
  tss_list_node->tss = tss;

  /* If this is the first entry, presume it is the idle thread. A
     little ugly... but it works. ;) */

  if (idle_tss_node == NULL)
  {
    idle_tss_node = tss_list_node;
  }

  if (*list != NULL)
  {
    (*list)->previous = (struct tss_list_type *) tss_list_node;
  }

  *list = tss_list_node;

  return tss_list_node;
}

/* Links the given TSS into the TSS hash and list. */

tss_list_type *thread_link (storm_tss_type *tss)
{
  unsigned int entry = hash (tss->thread_id);
  
  thread_link_list (&tss_hash[entry], tss);
  return thread_link_list (&tss_list, tss);
}

/* Unlinks the given entry in the given TSS list. */

void thread_unlink_list (tss_list_type **list, thread_id_type thread_id)
{
  tss_list_type *tss_list_node = *list;
  tss_list_type *previous, *next;
 
  DEBUG_MESSAGE (DEBUG, "Unlinking %u", thread_id);

  /* Find our boy in the list. */

  while (tss_list_node->thread_id != thread_id &&
         tss_list_node != NULL)
  {
    tss_list_node = (tss_list_type *) tss_list_node->next;
  }

  if (tss_list_node == NULL)
  {
    DEBUG_HALT ("Tried to unlink a thread which did not exist.");
  }

  previous = (tss_list_type *) tss_list_node->previous;
  next = (tss_list_type *) tss_list_node->next;

  if (previous == NULL)
  {
    *list = next;
  }
  else
  {
    previous->next = (struct tss_list_type *) next;
  }

  if (next != NULL)
  {
    next->previous = (struct tss_list_type *) previous;
  }


  /* Let's deallocate some memory. */

  memory_global_deallocate (tss_list_node);
}

/* Unlinks the TSS with the given thread ID from the TSS tree and
   list. */

void thread_unlink (thread_id_type thread_id)
{
  tss_list_type *tss_list_node = tss_list;
  unsigned int entry = hash (thread_id);
  tss_list_type *previous, *next;

  thread_unlink_list (&tss_list, thread_id);
  
  tss_list_node = tss_hash[entry];
  while (tss_list_node->thread_id != thread_id)
  {
    tss_list_node = (tss_list_type *) tss_list_node->next;
  }

  previous = (tss_list_type *) tss_list_node->previous;
  next = (tss_list_type *) tss_list_node->next;

  /* Was this the first entry in the list? If so, update the entry in
     the hash table. */

  if (previous == NULL)
  {
    tss_hash[entry] = next;
  }
  else
  {
    previous->next = (struct tss_list_type *) next;
  }

  if (next != NULL)
  {
    next->previous = (struct tss_list_type *) previous;
  }

  /* Let's deallocate some memory. */

  memory_global_deallocate (tss_list_node);
}

/* Get the TSS address for the thread with the given ID. This function
   presumes that the tree is already locked. */

storm_tss_type *thread_get_tss (thread_id_type thread_id)
{
  unsigned int entry = hash (thread_id);
  tss_list_type *node = tss_hash[entry];

  while (node != NULL)
  {
    if (node->thread_id == thread_id)
    {
      return node->tss;
    }
    
    node = (tss_list_type *) node->next;
  }
  
  DEBUG_HALT ("Thread didn't exist in the data storage.");
}

/* Get the first available thread ID. */
/* FIXME: Implement free-ID-pool. */
  
thread_id_type thread_get_free_id (void)
{
  thread_id_type thread_id;

  thread_id = free_thread_id++;
  DEBUG_MESSAGE (DEBUG, "thread_id = %u", thread_id);

  return thread_id;
}

/* Create a thread under the current cluster. Returns
   STORM_RETURN_THREAD_ORIGINAL for calling thread and
   STORM_RETURN_THREAD_NEW for new thread. */

/* FIXME: Some regions used by the kernel for temporary mappings need
   to be unique (or mutexed) to each thread under a cluster. Discuss
   how this is best done! Right now, we lock everything, which is
   sub-optimal. */

return_type thread_create (void)
{
  storm_tss_type *new_tss;
  page_directory_entry_page_table *new_page_directory =
    (page_directory_entry_page_table *) BASE_PROCESS_TEMPORARY;
  page_table_entry *new_page_table =
    (page_table_entry *) (BASE_PROCESS_TEMPORARY + SIZE_PAGE); 
  u32 stack_physical_page, page_directory_physical_page, 
    page_table_physical_page;
  int index;
  process_info_type *process_info;

  /* FIXME: We shouldn't have to do like this. */

  DEBUG_MESSAGE (DEBUG, "Disabling interrupts");
  cpu_interrupts_disable ();
  
  /* Add the new task to the task list, so we can map for the new
     thread. */

  mutex_kernel_wait (&memory_mutex);

  /* FIXME: Check return value. */

  memory_physical_allocate (&page_directory_physical_page, 1, 
                            "Thread page directory.");
  memory_physical_allocate (&page_table_physical_page, 1,
                            "Thread page table.");

  /* Map the page directory and the lowest page table. */

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY),
                      page_directory_physical_page, 1, PAGE_KERNEL);
  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY) + 1,
                      page_table_physical_page, 1, PAGE_KERNEL);

  /* Allocate memory for a TSS. */

  new_tss = ((storm_tss_type *) memory_global_allocate
             (sizeof (storm_tss_type) + current_tss->iomap_size));

  mutex_kernel_signal (&memory_mutex);

  /* Clone the TSS. */

  memory_copy ((u8 *) new_tss, (u8 *) current_tss,
               sizeof (storm_tss_type) + current_tss->iomap_size);

  /* FIXME: tss_tree_mutex should be changed to a 'dispatcher_mutex',
     or something... This looks a little weird if you don't know why
     it's written this way. */

  mutex_kernel_wait (&tss_tree_mutex);
  new_tss->thread_id = thread_get_free_id ();
  mutex_kernel_signal (&tss_tree_mutex);

  /* What has changed in the TSS is the ESP/ESP0 and the EIP. We must
     update those fields. */

  new_tss->eip = (u32) &&new_thread_entry;
  new_tss->cr3 = page_directory_physical_page * SIZE_PAGE;

  //  debug_print ("thread: %u\n", new_tss->thread_id);

  /* Clone the page directory and the lowest page table. */

  memory_copy ((u8 *) new_page_directory, (u8 *) BASE_PROCESS_PAGE_DIRECTORY,
               SIZE_PAGE);
  memory_copy ((u8 *) new_page_table, (u8 *) BASE_PROCESS_PAGE_TABLES,
               SIZE_PAGE);

  /* Set the stack as non-present. */
  /* FIXME: defines. */

  for (index = 1024 - 16; index < 1024; index++)
  {
    new_page_directory[index].present = 0;
  }

  /* Map the thread's page directory and update the mapping for the
     first pagetable. */

#if FALSE
  new_page_directory[0].page_table_base = page_table_physical_page;
  memory_virtual_map_other (new_tss, 
                            GET_PAGE_NUMBER (BASE_PROCESS_PAGE_DIRECTORY),
                            page_directory_physical_page, 1, PAGE_KERNEL);

  /* The 4 MB region where the pagetables are mapped also need to be
     unique. */

  memory_physical_allocate (&page_table_physical_page, 1);

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY) + 1,
                      page_table_physical_page, 1, PAGE_KERNEL);

  memory_copy ((u8 *) new_page_table, (u8 *) BASE_PROCESS_PAGE_TABLES,
               SIZE_PAGE);

  new_page_directory[8].page_table_base = page_table_physical_page;
  memory_virtual_map_other (new_tss,
                            GET_PAGE_NUMBER (BASE_PROCESS_PAGE_TABLES),
                            page_table_physical_page, 1, PAGE_KERNEL);
#endif

  /* FIXME: Map into all sister threads address spaces when creating a
     new page table for a thread. */

  /* Start by creating a PL0 stack. Remember that the lowest page of
     the stack area is the PL0 stack. */

  mutex_kernel_wait (&memory_mutex);

  /* FIXME: Check return value. */

  memory_physical_allocate (&stack_physical_page, 1, "Thread PL0 stack.");

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_CREATE),
                      stack_physical_page, 1, PAGE_KERNEL);

  memory_copy ((u8 *) BASE_PROCESS_CREATE, (u8 *) BASE_PROCESS_STACK,
               SIZE_PAGE * 1);

  memory_virtual_map_other (new_tss, GET_PAGE_NUMBER (BASE_PROCESS_STACK),
                            stack_physical_page, 1, PAGE_KERNEL);

  /* Phew... Finished setting up a PL0 stack. Lets take a deep breath
     and do the same for the PL3 stack, which is slightly more
     complicated. */

  /* FIXME: Check return value. */

  memory_physical_allocate (&stack_physical_page, current_tss->stack_pages,
                            "Thread PL3 stack.");

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_CREATE),
                      stack_physical_page, current_tss->stack_pages,
                      PAGE_KERNEL);

  memory_copy ((u8 *) BASE_PROCESS_CREATE,
               (u8 *) ((MAX_PAGES - current_tss->stack_pages) * SIZE_PAGE),
               current_tss->stack_pages * SIZE_PAGE);

  memory_virtual_map_other (new_tss, MAX_PAGES - current_tss->stack_pages,
                            stack_physical_page, current_tss->stack_pages,
                            PAGE_WRITABLE | PAGE_NON_PRIVILEGED);
  mutex_kernel_signal (&memory_mutex);

  new_tss->ss = new_tss->ss0;
  new_tss->cs = SELECTOR_KERNEL_CODE;
  new_tss->new_thread = TRUE;
  new_tss->eflags = THREAD_NEW_EFLAGS;
  new_tss->timeslices = 0;
  string_copy (new_tss->thread_name, "unnamed");

  mutex_kernel_wait (&tss_tree_mutex);
  process_info = (process_info_type *) new_tss->process_info;
  thread_link_list (&process_info->thread_list, new_tss);
  thread_link (new_tss);
  number_of_tasks++;
  new_tss->esp = cpu_get_esp ();
  process_info->number_of_threads++;

  mutex_kernel_signal (&tss_tree_mutex);

  /* Indicate whether this is the original thread or the new one. */
  
 new_thread_entry:

  DEBUG_MESSAGE (DEBUG, "Enabling interrupts");
  cpu_interrupts_enable ();

  if (current_tss->new_thread)
  {
    current_tss->new_thread = FALSE;
    
    /* FIXME: This shouldn't have to be. */

    dispatch_next ();

    DEBUG_MESSAGE (DEBUG, "Exiting, case 1.");

    return STORM_RETURN_THREAD_NEW;
  }
  else
  {
    DEBUG_MESSAGE (DEBUG, "Exiting, case 2.");
    return STORM_RETURN_THREAD_OLD;
  }
}

/* Delete a thread. If all threads under a cluster are deleted, the
   cluster is removed. If all clusters under a process is deleted,
   the whole process is deleted.

   This function takes for granted that tss_array_mutex is already
   locked. */

static return_type thread_delete (storm_tss_type *tss)
{
  process_info_type *process_info = (process_info_type *) tss->process_info;

  /* Free IRQs we might have allocated. */

  irq_free_all (tss->thread_id);

  /* Free port ranges possible allocated. */

  /* FIXME: This doesn't seem to really work... */
  //  port_range_free_all (thread_id);
  
  thread_unlink_list (&process_info->thread_list, tss->thread_id);
  process_info->number_of_threads--;

  tss->state = STATE_ZOMBIE;

  if (process_info->number_of_threads == 0)
  {
    process_parent_unblock ();
  }

  /* FIXME: Add this thread to a list of threads that the idle thread
     should delete, and implement this in the idle thread. */

  return RETURN_SUCCESS;
}

/* Control the state of a thread. */

return_type thread_control (thread_id_type thread_id, unsigned int class,
                            unsigned int parameter __attribute__ ((unused)))
{
  storm_tss_type *tss;

  /* Find out the task number for this thread. We need to lock the TSS
     tree mutex during this, to make sure the task ID:s won't get
     changed. */

  mutex_kernel_wait (&tss_tree_mutex);
  tss = thread_get_tss (thread_id);
  if (tss == NULL)
  {
    mutex_kernel_signal (&tss_tree_mutex);
    return STORM_RETURN_INVALID_ARGUMENT;
  }  

  switch (class)
  {
    /* Terminate the target thread. */
    
    case THREAD_TERMINATE:
    {
      /* If we try to kill the kernel, reboot. */

      if (thread_id == THREAD_ID_KERNEL)
      {
        cpu_reset ();
      }

      thread_delete (tss);

      /* If this is the current thread, call the dispatcher so that we
         get out of the way as soon as possible. */

      if (tss->thread_id == current_tss->thread_id)
      {
        mutex_kernel_signal (&tss_tree_mutex);
        dispatch_next ();
      }

      break;
    }     

    /* Put a process to sleep. */

    case THREAD_SLEEP:
    {
      //#undef DEBUG
      //#define DEBUG TRUE
      DEBUG_MESSAGE (DEBUG, "Putting TID %u to sleep for %u milliseconds",
                     thread_id, parameter);
      //#undef DEBUG
      //#define DEBUG FALSE

      tss->state = STATE_SLEEP;

      /* Add an event timer to start the thread again. */
      
      timer_add_event (parameter, TIMER_EVENT_THREAD_WAKEUP, tss);

      /* Give up the rest of the time-slice. */

      if (thread_id == current_thread_id)
      {
        mutex_kernel_signal (&tss_tree_mutex);
        dispatch_next ();
        return STORM_RETURN_SUCCESS;
      }

      break;
    }
  }

  mutex_kernel_signal (&tss_tree_mutex);
  return STORM_RETURN_SUCCESS;
}

/* FIXME: Optimise this whole block/unblock stuff, perhaps with a
   linked list or whatever... */
/* Block a thread on a kernel mutex. */

void thread_block_kernel_mutex (storm_tss_type *tss,
                                mutex_kernel_type *mutex_kernel)
{
#if FALSE
  u32 esp = cpu_get_esp ();

  DEBUG_MESSAGE (DEBUG, "Called from %p", ((u32 *) esp)[8]);
#endif

  /* FIXME: Security. */

  DEBUG_MESSAGE (DEBUG, "Called");
  DEBUG_MESSAGE (DEBUG, "Blocking %x", mutex_kernel);

  tss->state = STATE_MUTEX_KERNEL;
  tss->mutex_kernel = mutex_kernel;
  tss->mutex_time = timeslice;

  if (tss->thread_id == current_tss->thread_id)
  {
    dispatch_next ();
  }
}

/* Unblock a thread which is blocked on a user mutex. */

return_type thread_unblock_kernel_mutex (mutex_kernel_type *mutex_kernel)
{
  time_type lowest_time = MAX_TIME;
  storm_tss_type *lowest_tss = NULL;
  tss_list_type *tss_node;

  DEBUG_MESSAGE (DEBUG, "Called");

  /* FIXME: We can't do this with a mutex. So how do we do? */

  //  while (tss_tree_mutex == MUTEX_LOCKED);

  tss_node = tss_list;

  while (tss_node != NULL)
  {
    if (tss_node->tss->state == STATE_MUTEX_KERNEL &&
        tss_node->tss->mutex_time < lowest_time &&
        tss_node->tss->mutex_kernel == mutex_kernel)
    {
      lowest_tss = tss_node->tss;
      lowest_time = tss_node->tss->mutex_time;
    }

    tss_node = (tss_list_type *) tss_node->next;
  }

  if (lowest_tss != NULL)
  {
    lowest_tss->state = STATE_DISPATCH;
    
    return RETURN_THREAD_UNBLOCKED;
  }
  else
  {
    return RETURN_NO_THREAD_UNBLOCKED;
  }
}

/* FIXME: Those two could be put into one. */

/* Unblock a thread which is blocked on a mailbox send. */
/* FIXME: Unblock the oldest thread. This could lead to starvation. */

void thread_unblock_mailbox_send (mailbox_id_type mailbox_id)
{
  tss_list_type *tss_node;

  tss_node = tss_list;

  while (tss_node != NULL)
  {
    if (tss_node->tss->state == STATE_MAILBOX_SEND &&
        tss_node->tss->mailbox_id == mailbox_id)
    {
       tss_node->tss->state = STATE_DISPATCH;
      break;
    }

    tss_node = (tss_list_type *) tss_node->next;
  }
}

/* Unblock a thread which is blocked on a mailbox receive. */

void thread_unblock_mailbox_receive (mailbox_id_type mailbox_id)
{
  tss_list_type *tss_node;

  tss_node = tss_list;

  DEBUG_MESSAGE (DEBUG, "Unlocking thread locked on mailbox ID %u", mailbox_id);

  while (tss_node != NULL)
  {
    if (tss_node->tss->state == STATE_MAILBOX_RECEIVE &&
        tss_node->tss->mailbox_id == mailbox_id)
    {
      DEBUG_MESSAGE (DEBUG, "Unlocking thread %u", tss_node->thread_id);
      tss_node->tss->state = STATE_DISPATCH;
      break;
    }

    tss_node = (tss_list_type *) tss_node->next;
  }
}
