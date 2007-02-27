/* $Id$ */
/* Abstract: Handle port I/O. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
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
   USA */

/* Define as TRUE to get lots of debug information. */

#define DEBUG FALSE

#include <storm/state.h>

#include <storm/generic/bit.h>
#include <storm/generic/debug.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>

#include <storm/ia32/gdt.h>
#include <storm/ia32/tss.h>

/* Linked list over allocated I/O regions. */

port_range_type *port_list = NULL;

/* Mutex for the port list. */

//static mutex_kernel_type port_mutex = MUTEX_UNLOCKED;

/* Link in a port range into the list of allocated ports. */

bool port_range_link (unsigned int start, unsigned int ports, 
                      char *description)
{
  port_range_type *node = port_list;
  port_range_type *new_node;

  /* Check if the port range is already allocated. We have two cases
     when we have to deny the request. This is when the requested
     range is inside another portrange, or another portrange starts or
     ends inside the requested region. */
  
  while (node != NULL)
  {
    /* The first case. */

    if (start >= node->start && (start + ports) <=
        (node->start + node->length))
    {
      return FALSE;
    }

    /* The other case. */

    if ((node->start >= start &&
         node->start <= (start + ports)) ||
        ((node->start + node->length) >= start &&
         (node->start + node->length) <= (start + ports)))
    {
      return FALSE;
    }    
    
    node = (port_range_type *) node->next;
  }

  DEBUG_MESSAGE (DEBUG, "Region free.");

  /* OK, the requested region is free, we can open up the ports in the
     thread's I/O-map. But first, let's mark this region as used. */

  new_node = (port_range_type *) 
    memory_global_allocate (sizeof (port_range_type));

  new_node->start = start;
  new_node->length = ports;
  new_node->process_id = current_tss->process_id;
  new_node->cluster_id = current_tss->cluster_id;
  new_node->thread_id = current_tss->thread_id;
  new_node->description = memory_global_allocate (string_length (description));
  string_copy (new_node->description, description);

  /* Add it to the list. */

  new_node->previous = NULL;
  new_node->next = (struct port_range_type *) port_list;

  if (port_list != NULL)
  {
    port_list->previous = (struct port_range_type *) new_node;
  }

  port_list = new_node;
  return TRUE;
}

/* Set up data structures for port allocation. */

void port_init (void)
{
  /* Allocate some reserved regions. FIXME: maybe a struct holding all the
     regions and a loop? */

  /* Interrupt controllers. */

  port_range_link (0x0020, 32, "Primary interrupt controller");
  port_range_link (0x00A0, 32, "Secondary interrupt controller");

  /* FPU. */

  port_range_link (0x00F8, 16, "Floating point unit");
}

/* Register a port range. */

return_type port_range_register (unsigned int start, unsigned int ports,
                                 char *description)
{
  unsigned int port;
  int index;

  /* Make sure we're not out of bounds. */

  if (start + ports > MAX_U16)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  mutex_kernel_wait (&tss_tree_mutex);

  /* First of all, we have to check if the process has access to any
     ports at all. FIXME: Should be a capability check instead. */

  if (current_tss->process_type != PROCESS_TYPE_SERVER)
  {
    return STORM_RETURN_ACCESS_DENIED;
  }

  DEBUG_MESSAGE (DEBUG, "port_allocate: %u %u.", start, ports);
  
  /* Try to add this port range into the list. */

  if (!port_range_link (start, ports, description))
  {
    mutex_kernel_signal (&tss_tree_mutex);
    return STORM_RETURN_BUSY;
  }

  /* If the TSS is too small, expand it. */
  
  if (current_tss->iomap_size < BIT_IN_BYTES (start + ports - 1))
  {
    int old_iomap_size = current_tss->iomap_size;
    storm_tss_type *old_tss = current_tss;

    DEBUG_MESSAGE (DEBUG, "%u bytes wasn't enough to hold this I/O map (will be %u bytes).",
                   current_tss->iomap_size, BIT_IN_BYTES (start + ports - 1));
    DEBUG_MESSAGE (DEBUG, "Reallocating TSS.");

    current_tss = memory_global_allocate (sizeof (storm_tss_type) +
                                          BIT_IN_BYTES (start + ports));
    
    DEBUG_MESSAGE (DEBUG, "Allocating %u bytes.",
                   (sizeof (storm_tss_type) + BIT_IN_BYTES (start + ports - 1)));

    memory_copy (current_tss, old_tss,
                 sizeof (storm_tss_type) + old_iomap_size);
                                                
    current_tss->iomap_size = BIT_IN_BYTES (start + ports);
    memory_set_u8 (current_tss->iomap + old_iomap_size,
                   0xFF, current_tss->iomap_size - old_iomap_size);
    DEBUG_MESSAGE (DEBUG, "Clearing %u bytes", current_tss->iomap_size -
                   old_iomap_size)
    thread_unlink (old_tss->thread_id);
    current_tss_node = thread_link (current_tss);

    switch (dispatch_task_flag)
    {
      case 0:
      {
        gdt_setup_tss_descriptor (SELECTOR_TSS1 >> 3, current_tss, 3,
                                  sizeof (storm_tss_type) + 
                                  current_tss->iomap_size);
        cpu_set_tr (SELECTOR_TSS1);
        break;
      }
      
      case 1: 
      {
        gdt_setup_tss_descriptor (SELECTOR_TSS2 >> 3, current_tss, 3,
                                  sizeof (storm_tss_type) + 
                                  current_tss->iomap_size);
        cpu_set_tr (SELECTOR_TSS2);
        break;
      }
    }

    memory_global_deallocate (old_tss);
  }

  mutex_kernel_signal (&tss_tree_mutex);

  /* Add the port range to the callers I/O-map. */

  for (port = start; port < start + ports; port++)
  {
    DEBUG_MESSAGE (DEBUG, "Clearing bit %u in the I/O map", port);
    BIT_CLEAR (current_tss->iomap[port / 8],
               port % 8);
  }

  /* Since this thread has got a new TSS, we have to check if it is an
     IRQ handler. If so, we must update the TSS pointer. */

  for (index = 0; index < IRQ_LEVELS; index++)
  {
    if (irq[index].allocated && irq[index].thread_id == current_thread_id)
    {
      irq[index].tss = current_tss;
    }
  }

  return STORM_RETURN_SUCCESS;
}

/* Unregister a port range. */

return_type port_range_unregister (unsigned int start)
{
  port_range_type *port = port_list;

  while (port != NULL && port->start != start)
  {
    port = (port_range_type *) port->next;
  }

  /* Did we run out of the list? If so, this thread has not previously
     allocated a port range starting at this base, so just abort
     silently. */

  if (port == NULL)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  /* Unlink this entry from the list. First, check if it is the only
     entry. If so, everything gets very simple. */
  
  if (port->previous == NULL &&
      port->next == NULL)
  {
    port_list = NULL;
  }
  
  /* If it doesn't have a previous pointer, we can assume that this is
     the first entry. If so, make the next entry the start of the
     list. */
  
  else if (port->previous == NULL)
  {
    port_list = (port_range_type *) port->next;
    
    port_list->previous = NULL;
  }
  else if (port->next == NULL)
  {
    /* This entry is the last one. */
    
    port_range_type *previous = (port_range_type *) port->previous;
    
    previous->next = NULL;
  }
  else
  {
    /* We are in the middle of nowhere... umm, I mean, the list. */
    
    port_range_type *previous = (port_range_type *) port->previous;
    port_range_type *next = (port_range_type *) port->next;
    
    previous->next = (struct port_range_type *) next;
    next->previous = (struct port_range_type *) previous;
  }

  return STORM_RETURN_SUCCESS;
}

/* Free all ports allocated by the given task. */
/* FIXME: This function should take a thread_id as argument. */
/* FIXME: Find a better name for this. */

void port_range_free_all (thread_id_type thread_id)
{
  port_range_type *port = port_list;

  /* FIXME: This algorithm feels a little sub-optimal... or is it just
     me? */

  if (port == NULL)
  {
    return;
  }

  /* Loop until we find no more matches. */
  
  while (TRUE)
  {
    while (port->next != NULL && thread_id != port->thread_id)
    {
      port = (port_range_type *) port->next;
    }
    
    if (thread_id != port->thread_id)
    {
      return;
    }
    
    /* Unlink this entry from the list. First, check if it is the only
       entry. If so, everything gets very simple. */

    if (port->previous == NULL &&
        port->next == NULL)
    {
      port_list = NULL;
      return;
    }

    /* If it doesn't have a previous pointer, we can assume that this
       is the first entry. If so, make the next entry the start of the
       list. */
    
    else if (port->previous == NULL)
    {
      port_list = (port_range_type *) port->next;
      
      port_list->previous = NULL;
    }
    else if (port->next == NULL)
    {
      /* This entry is the last one. */
      
      port_range_type *previous = (port_range_type *) port->previous;
      
      previous->next = NULL;
      return;
    }
    else
    {
      /* We are in the middle of nowhere... umm, I mean, the list. */
      
      port_range_type *previous = (port_range_type *) port->previous;
      port_range_type *next = (port_range_type *) port->next;
      
      previous->next = (struct port_range_type *) next;
      next->previous = (struct port_range_type *) previous;
    }
  }
}
