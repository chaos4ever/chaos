/* $Id$ */
/* Abstract: Process support. */

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

#include "config.h"
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/id.h>
#include <storm/generic/list.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/process.h>

#define DEBUG TRUE

/* The kernel process. */

process_type *kernel_process;

/* The process list. */

process_type *process_list = NULL;

/* Initialise the process support. */

void process_init (void)
{
  debug_print ("Initializing process structures...\n");

  kernel_process = memory_global_allocate (sizeof (process_type));
  
  if (kernel_process == NULL)
  {
    DEBUG_HALT ("Out of memory!");
  }

  kernel_process->id = id_allocate ();
  kernel_process->name = PACKAGE_NAME " " PACKAGE_VERSION;
  kernel_process->number_of_threads = 1;
  list_insert ((list_type **) &process_list, (list_type *) kernel_process);
  
  DEBUG_MESSAGE (DEBUG, "Created kernel process with ID %u.",
                 kernel_process->id);
}
