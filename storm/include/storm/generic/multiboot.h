/* $Id$ */
/* Abstract: Function prototypes and structure definitions of the
   Multiboot stuff. */

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

#ifndef __STORM_GENERIC_MULTIBOOT_H__
#define __STORM_GENERIC_MULTIBOOT_H__

#include <storm/generic/bit.h>
#include <storm/generic/types.h>

/* Enums. */

enum
{
  /* Indicates that the boot loader should page align modules. */

  MULTIBOOT_PAGE_ALIGN = BIT_VALUE (0),

  /* Indicate that the boot loader should put information about
     available memory in the Multiboot info structure. */

  MULTIBOOT_GET_MEMORY = BIT_VALUE (1),

  /* Indicate that the boot loader should put information about the
     current video mode in the Multiboot info structure. */

  MULTIBOOT_GET_VIDEO_MODE = BIT_VALUE (2),
};

/* Defines. */

#define MULTIBOOT_MAGIC         0x1BADB002
#define MULTIBOOT_FLAGS         (MULTIBOOT_PAGE_ALIGN | \
                                 MULTIBOOT_GET_MEMORY)

/* More information about the multiboot format can be found in the GNU
   GRUB documentation available on
   http://www.gnu.org/software/grub/. */

/* Type definitions. */

typedef struct
{
  /* The following flags determine wheter the rest of the fields are
     available. */

  u32 has_memory_info           : 1;
  u32 has_boot_device_info      : 1;
  u32 has_command_line          : 1;
  u32 has_module_info           : 1;
  u32 has_aout_info             : 1;
  u32 has_elf_info              : 1;
  u32 has_memory_map            : 1;
  u32 has_drives_info           : 1;
  u32 has_ROM_config            : 1;
  u32 has_boot_loader_name      : 1;
  u32 has_APM_info              : 1;
  u32 has_video_mode_info       : 1;
  u32 reserved                  : 20;

  u32 memory_lower;
  u32 memory_upper;
  
  u32 boot_device;

  u32 command_line;

  u32 number_of_modules;
  u32 module_base;

  /* If has_elf_info is 1 (and it is), those fields are valid. */
  
  u32 section_header_num; /* ? */
  u32 section_header_size;
  u32 section_header_address;
  u32 section_header_string_index;

  /* FIXME: Use this information. */

  u32 memory_map_length;
  u32 memory_map_addr;

  /* The following are not implemented by GRUB yet.. */
#if FALSE
  u32 drives_count;
  u32 drives_addr;
  
  u32 config_table;
  
  /* Pointer to the boot loader name. */

  u32 boot_loader_name;
#endif

  /* Likewise... */
#if FALSE
  /* Video mode information. */

  u32 mode_type;
  u32 width;
  u32 height;
  u32 depth;
  u32 frame_buffer_address;
#endif
} __attribute__ ((packed)) multiboot_info_type;

typedef struct
{
  u32 start;
  u32 end;
  u8 *name;
  u32 reserved;
} __attribute__ ((packed)) multiboot_module_info_type;

typedef struct
{
  u32 num;
  u32 size;
  u32 addr;
  u32 section_header_index;
} __attribute__ ((packed)) multiboot_elf_info_type;

/* External variables. */

extern multiboot_info_type multiboot_info;
extern multiboot_module_info_type multiboot_module_info[];

/* Function prototypes. */

extern void multiboot_init (void) INIT_CODE;

#endif /* !__STORM_GENERIC_MULTIBOOT_H__ */
