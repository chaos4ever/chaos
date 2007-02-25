/* $Id$ */
/* Abstract: Startup code file for chaos binaries. This code is what executes
   the main () function. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include <memory/memory.h>
#include <system/system.h>

/* FIXME: Write a mutex library. */

enum
{
  MUTEX_UNLOCKED,
  MUTEX_LOCKED
};

/* This is a little bit ugly, but it would be even uglier to have each
   process declare this separately... or whatever. */

memory_structure_type memory_structure;
int memory_mutex = MUTEX_UNLOCKED;

/* Function prototypes. */

extern int main (int argc, char **argv);
void startup (void);

/* Start of code. */

void startup (void)
{
  kernelfs_self_type kernelfs_self;

  system_call_init ();

  /* FIXME: Reimplement the command line passing. It was made in an
     extremely ugly way, so we removed it for now. */

  main (0, (char **) NULL);

  kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
  system_call_kernelfs_entry_read (&kernelfs_self);
  system_call_thread_control (kernelfs_self.thread_id, THREAD_TERMINATE, 0);
  while (TRUE);
}
