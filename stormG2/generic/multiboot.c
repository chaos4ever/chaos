/* $Id$ */
/* Abstract: Parse some of the Multiboot data. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#include <storm/limits.h>
#include <storm/generic/debug.h>
#include <storm/generic/memory.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/string.h>

multiboot_info_type multiboot_info;
multiboot_module_info_type multiboot_module_info[MAX_STARTUP_SERVERS];
u8 *memory_map = (u8 *) BASE_MEMORY_MAP;

/* Copy off some of the Multiboot data we need, so it won't be
   overwritten. */

void multiboot_init (void)
{
  u8 *target = (u8 *) BASE_MODULE_NAME;
  unsigned module;
  
  /* Copy the module parameters. */

  memory_copy
    (multiboot_module_info,
     (multiboot_module_info_type *) multiboot_info.module_base,
     multiboot_info.number_of_modules * sizeof (multiboot_module_info_type));

  /* First of all, make sure module names and parameters are stored in
     a safe place. Otherwise, we may overwrite them later on in the
     boot process. */

  for (module = 0; module < multiboot_info.number_of_modules; module++)
  {
    string_copy (target, multiboot_module_info[module].name);
    multiboot_module_info[module].name = target;
    target += string_length (target) + 1;
  }

  /* Now, save the memory map. */

  memory_copy ((u8 *) BASE_MEMORY_MAP, 
               (void *) multiboot_info.memory_map_address, 
               multiboot_info.memory_map_length);

  /* Now, lets parse the kernel command line. */

  //  arguments_parse ((u8 *) multiboot_info.command_line, arguments_kernel, 0);
}
