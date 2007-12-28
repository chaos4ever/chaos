/* $Id$ */
/* Abstract: Execution of ELF images. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#define DEBUG FALSE

#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/elf.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

/* Execute an ELF image located at the linear virtual `image'. */
/* FIXME: Figure out a way to modularize this part of the kernel. */
/* FIXME: image should be elf_header_type *elf_header. */

return_type elf_execute
  (u8 *image, char *parameter_string, process_id_type *process_id)
{
  elf_header_type *elf_header = (elf_header_type *) image;
  section_header_type *section_header;
  u32 index;
  u32 code_section_base = 0, data_section_base = 0, bss_section_base = 0;
  u32 code_section_size = 0, data_section_size = 0, bss_section_size = 0;
  u8 *code_section_address = NULL;
  u8 *data_section_address = NULL;
  process_create_type elf_process_create;

  /* First of all, make sure this is an ELF image... */

  if (!(elf_header->identification[0] == 0x7F &&
        elf_header->identification[1] == 'E' && 
        elf_header->identification[2] == 'L' &&
        elf_header->identification[3] == 'F'))
  {
    return RETURN_ELF_INVALID;
  }

  /* ...and a 32-bit one... */

  if (elf_header->class != ELF_CLASS_32BIT)
  {
    return RETURN_ELF_UNSUPPORTED;
  }

  /* ...big endian... */

  if (elf_header->endian != ELF_ENDIAN_BIG)
  {
    return RETURN_ELF_UNSUPPORTED;
  }

  /* ...executable... */

  if (elf_header->type != ELF_TYPE_EXECUTABLE)
  {
    return RETURN_ELF_UNSUPPORTED;
  }

  /* ...of the right ELF version. */

  if (elf_header->version != ELF_VERSION_CURRENT)
  {
    return RETURN_ELF_UNSUPPORTED;
  }

  /* Well, that was the identification block. Now for the real part of
     the ELF header... */

  /* Make sure it is for Intel 80386. */

  if (elf_header->machine != ELF_MACHINE_386)
  {
    return RETURN_ELF_UNSUPPORTED;
  }

  /* Start by finding the size of the eventual BSS section. The first
     section is used to mark undefined section references, so we start
     counting from one. */

  for (index = 1; index < elf_header->section_header_entries; index++)
  {
    section_header = (section_header_type *)
      (image + elf_header->section_header_offset +
       index * elf_header->section_header_entry_size);

    DEBUG_MESSAGE (DEBUG, "Index = %u", index);
    
    if (section_header->flags ==
        (ELF_SECTION_FLAG_WRITE | ELF_SECTION_FLAG_ALLOCATE) &&
        section_header->type == ELF_SECTION_TYPE_NO_SPACE)
    {
      bss_section_size = section_header->size;
      bss_section_base = section_header->address;
    }
  }

  /* Go through the section header table to find out where to put
     stuff. */

  /* FIXME: Make a real ELF implementation some day. This *will* not
     work in some quite common cases. Also try to modularize this part
     of the kernel and have it removed, once kernel init is done. */

  for (index = 1; index < elf_header->section_header_entries; index++)
  {
    section_header = (section_header_type *)
      (image + elf_header->section_header_offset +
       index * elf_header->section_header_entry_size);
    
    /* Check if the section is within allowed limits. */

    if ((section_header->address < BASE_PROCESS_SPACE_START ||
         (section_header->address + 
          section_header->size) >= BASE_PROCESS_SPACE_END) &&
        (section_header->flags == 6 || section_header->flags == 3))
    {
      return RETURN_ELF_SECTION_MISPLACED;
    }

    /* Check which type of section this is. */

    switch (section_header->flags)
    {
      /* .data or .code */
      
      case ELF_SECTION_FLAG_WRITE | ELF_SECTION_FLAG_ALLOCATE: 
      case ELF_SECTION_FLAG_ALLOCATE | ELF_SECTION_FLAG_EXECUTE:
      {
        if (section_header->type == ELF_SECTION_TYPE_NO_SPACE)
        {
          break;
        }

        switch (section_header->flags)
        {
          /* .data */

          case ELF_SECTION_FLAG_WRITE | ELF_SECTION_FLAG_ALLOCATE: 
          {

            data_section_size = section_header->size;
            data_section_base = section_header->address;
            data_section_address = (u8 *) image + section_header->offset;
            
            break; 
          }

          /* .code */

          case ELF_SECTION_FLAG_ALLOCATE | ELF_SECTION_FLAG_EXECUTE:
          {
            code_section_size = section_header->size;
            code_section_base = section_header->address;
            code_section_address = (u8 *) image + section_header->offset;

            break;
          }
        }
        break;
      }
    }
  }

  /* Let the kernel create us a process. */

  elf_process_create.initial_eip = elf_header->entry_point;
  elf_process_create.process_type = PROCESS_TYPE_SERVER;
  elf_process_create.parameter_string = parameter_string;
  elf_process_create.process_id = process_id;
  elf_process_create.code_section_address = code_section_address;
  elf_process_create.code_section_size = code_section_size;
  elf_process_create.code_section_base = code_section_base;
  elf_process_create.data_section_address = data_section_address;
  elf_process_create.data_section_size = data_section_size;
  elf_process_create.data_section_base = data_section_base;
  elf_process_create.bss_section_size = bss_section_size;
  elf_process_create.bss_section_base = bss_section_base;

  DEBUG_MESSAGE (DEBUG, "Creating new process.");

  return process_create (&elf_process_create);
}
