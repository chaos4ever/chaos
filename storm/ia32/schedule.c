/* $Id$ */
/* Abstract: The scheduler. Round robbin for now. */

/* Copyright 1999 chaos development. */

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


#if FALSE

void schedule (void)
{
  u32 counter;

  secure_printf ("ssh ulle@webwarez.net -v -c arcfour\n");
  haltcpu ();

  spin_lock (schedule_mutex);

  task_queue_length = 0;
  //  task_queue_position = 0;
  
  for (counter = 0; counter < threads; counter++)
  {
    if (tss[counter].state == STATE_DISPATCH)
    {
      task_queue[task_queue_length] = counter;
      task_queue_length++;
    }
  }

  /* If no other tasks are launched, fall back to the idle
     process(es). */

  if (task_queue_length == 0)
  {
    for (counter = 0; counter < threads; counter++)
    {
      if (tss[counter].state == STATE_IDLE)
      {
        task_queue[task_queue_length] = counter;
        task_queue_length++;
      }
    }
  }

  secure_printf ("task_queue_length = %lu, task queue: [", task_queue_length);

#ifdef DEBUG
  {
    int c;

    for (c = 0; c < task_queue_length; c++)
    {
      secure_printf (" %lu", task_queue[c]);
    }
  }
  secure_printf ("]\n");
#endif

  spin_unlock (schedule_mutex);
}

#endif
