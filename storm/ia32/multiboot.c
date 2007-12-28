/* $Id$ */
/* Abstract: Parse some of the Multiboot data. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#include <storm/generic/arguments.h>
#include <storm/generic/defines.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

multiboot_info_type multiboot_info;
multiboot_module_info_type multiboot_module_info[MAX_STARTUP_SERVERS] INIT_DATA;

void multiboot_init (void)
{
  char *target = (char *) BASE_MODULE_NAME;
  unsigned module;

  memory_copy
    (multiboot_module_info,
     (multiboot_module_info_type *) multiboot_info.module_base,
     multiboot_info.number_of_modules * sizeof (multiboot_module_info_type));

  /* First of all, make sure module names and parameters are stored in
     a safe place. Otherwise, we may overwrite them later on in the
     boot process. */

  for (module = 0; module < multiboot_info.number_of_modules; module++)
  {
    string_copy (target, (char *) multiboot_module_info[module].name);
    multiboot_module_info[module].name = (u8 *) target;
    target += string_length (target) + 1;
  }

  /* Now, lets parse the kernel command line. */

  arguments_parse ((char *) multiboot_info.command_line, arguments_kernel, 0);
}
