/* $Id$ */
/* Abstract: Idle thread. The idle thread does some more things in
   chaos than just wasting CPU cycles; it performs actual thread
   deletion (since it's a little hard to do when the thread is still
   active) and some other clever things. */
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
#include <storm/generic/idle.h>
#include <storm/generic/process.h>
#include <storm/generic/string.h>
#include <storm/state.h>

/* Idle task. */

void idle (void)
{
  string_copy (current_tss->thread_name, "Idle thread");

  while (TRUE)
  {
    /* Let forever be. */
    /* FIXME: If we get here, the system is totaly asleep, and we can
       safely check for tasks to do. For now, only try to find a
       thread to run. */

    dispatch_next ();
  }
}
