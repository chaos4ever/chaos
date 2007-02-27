/* $Id$ */
/* Abstract: This is the startup point of storm. It is executed right
   after the assembly language init code has set up the GDT, kernel
   stack, etc. Here, we initialise everything in the storm, like
   IRQ/exception handling, the timer hardware, the memory facilities
   of the host CPU and multitasking. It is also responsible for
   starting the servers loaded by the Multiboot compliant boot
   loader. */

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
   USA. */

#include <storm/generic/debug.h>
#include <storm/generic/main.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/multiboot.h>
#include <storm/ia32/exception.h>
#include <config.h>

/* Do the bootup procedure. */

void main_bootup (int number_of_arguments UNUSED, char **argument UNUSED)
{
  /* Initialize the debugging functions. */

  debug_init ();

  /* Print some information, if we are running in debug mode. */

  debug_print ("%s %s ", PACKAGE_NAME, PACKAGE_VERSION);
  debug_print ("(compiled by %s on %s %s).\n", CREATOR, __DATE__, __TIME__);
  debug_print ("Running in debug mode...\n");

  /* Detect CPU type and capabilities. */

  cpu_init ();

  /* Setup physical memory allocation structures and reserve reserved
     regions. */

  memory_physical_init ();
  
  /* Create an address space for the idle-thread and enable paging. */

  memory_virtual_init ();

  /* Initialise the global memory slab system. */
  
  memory_global_init ();

  /* Initialise the process support. */

  process_init ();

  /* Initalise the thread structures and create the kernel thread. */

  thread_init ();

  /* Set up exception handlers. */

  exception_init ();

  debug_print ("Number of allocated pages: %u.\n",
               memory_physical_get_allocated_pages ());
}
