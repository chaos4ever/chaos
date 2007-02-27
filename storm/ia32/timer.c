/* $Id$ */
/* Abstract: Interrupt handler for the timer interrupt and associated
   stuff. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */

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

/* This file sets up the 8254 chip correctly. I don't think it will
   work with 8253:s, but that shouldn't really be a big problem... */

#include <storm/generic/bit.h>
#include <storm/generic/debug.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/port.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <storm/generic/return_values.h>
#include <storm/state.h>
#include <storm/ia32/timer.h>
#include <storm/ia32/tss.h>

/* Define as TRUE if you want debug information. */

#define DEBUG FALSE

u32 hz = 32;
timer_event_type *timer_event_list = NULL;
volatile time_type uptime = 0;

/* Read the timer chip. */

static inline void rdtsc (u32 *low, u32 *high)
{
  asm volatile 
  (\
   "rdtsc" 
   : "=a" (*low), "=d" (*high));
}

/* Initialise PIT channels 0 and 2. */

void timer_init (void)
{
  /* Channel 0. */
  
  port_out_u8 (PIT_MODE_PORT, 
               (COUNTER_0_SELECT | ACCESS_LOW_COUNTER_U8 |
                ACCESS_HIGH_COUNTER_U8 | MODE_3_SELECT | BINARY_COUNTER));

  /* LSB first, then MSB. */

  port_out_u8 (PIT_COUNTER_DIVISOR, LOW_U8 (COUNTER_DIVISOR (hz)));
  port_out_u8 (PIT_COUNTER_DIVISOR, HIGH_U8 (COUNTER_DIVISOR (hz)));

  /* Channel 2. We use this to get a good timer. Or, more correctly,
     we should use it. ;) FIXME */

  port_out_u8 (PIT_MODE_PORT,
               (COUNTER_2_SELECT | ACCESS_LOW_COUNTER_U8 |
                ACCESS_HIGH_COUNTER_U8 | MODE_3_SELECT | BINARY_COUNTER));
  port_out_u8 (PIT_COUNTER_2, LOW_U8 (COUNTER_DIVISOR (1000)));
  port_out_u8 (PIT_COUNTER_2, HIGH_U8 (COUNTER_DIVISOR (1000)));
}

/* Add an event. */

void timer_add_event (time_type milliseconds, u32 action, storm_tss_type *tss)
{
  timer_event_type *event = memory_global_allocate (sizeof (timer_event_type));
  timer_event_type *node = timer_event_list;

  event->timeslice = timeslice + ((milliseconds * hz) / 1000) + 1;
  event->action = action;
  event->tss = tss;

#if FALSE
  while (node != NULL)
  {
    if (event->timeslice >= node->timeslice)
    {
      break;
    }

    node = (timer_event_type *) node->next;
  }
#endif

  /* Is there no list? */

  if (node == NULL)
  {
    timer_event_list = event;
    event->previous = event->next = NULL;
  }
  else
  {
#if FALSE
    /* If this is in the middle of the list, modify the previous entry
       to point at this one. */

    if (node->previous != NULL)
    {
      ((timer_event_type *) node->previous)->next = 
        (struct timer_event_type *) event;
      event->previous = node->previous;
    }

    /* Otherwise, modify the list to put us up front. */

    else
#endif
    {
      timer_event_list = event;
      event->previous = NULL;
    }
    
    /* Update the 'next' fields. */

    event->next = (struct timer_event_type *) node;
    node->previous = (struct timer_event_type *) event;
  }
}

/* Called from the task switcher. */

void timer_check_events (void)
{
  timer_event_type *event = timer_event_list;

  while (event != NULL)
  {
    DEBUG_MESSAGE (DEBUG, "%u <= %u", (unsigned) event->timeslice, 
                   (unsigned) timeslice);

    if (event->timeslice <= timeslice)
    {
      timer_event_type *next = (timer_event_type *) event->next;
      timer_event_type *previous = (timer_event_type *) event->previous;

      //      if (event->previous != NULL)
      //      {
      //        DEBUG_HALT ("Event list not sorted properly.");
      //      }

      /* Check out what to do. */

      if (event->action == TIMER_EVENT_THREAD_WAKEUP)
      {
        DEBUG_MESSAGE (DEBUG, "Waking TID %u up", event->tss->thread_id);
	event->tss->state = STATE_DISPATCH;
      }
      else
      {
        DEBUG_HALT ("Unknown event action.");
      }

      /* Remove this event counter. */

      if (next != NULL)
      {
        next->previous = (struct timer_event_type *) previous;
      }

      if (previous != NULL)
      {
        previous->next = (struct timer_event_type *) next;
      }
      else
      {
        timer_event_list = next;
      }

      //memory_global_deallocate (event);
    }
    //    else
    //    {
    //      break;
    //    }

    event = (timer_event_type *) event->next;
  }
}
