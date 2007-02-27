/* $Id$ */
/* Abstract: Functions for setting up the system calls. */
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

#include <storm/ia32/defines.h>
#include <storm/ia32/gdt.h>
#include <storm/ia32/system_calls.h>
#include <storm/ia32/types.h>

/* Initialise system calls. */

void system_calls_init (void)
{
  /* Set up the call gates in the GDT. */

  u32 counter;
  
  for (counter = 0; counter < SYSTEM_CALLS; counter++)
  {
    gdt_setup_call_gate (system_call[counter].number, SELECTOR_KERNEL_CODE,
			 system_call[counter].handler, 3,
                         system_call[counter].arguments);
  }
}
