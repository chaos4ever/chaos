/* $Id$ */
/* Abstract: ELF execution library header files. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_EXECUTE_ELF_EXECUTE_ELF_H__
#define __LIBRARY_EXECUTE_ELF_EXECUTE_ELF_H__

#include <system/system.h>
#include <execute_elf/return_values.h>

/* More information about the ELF format can be found in the ELF
   specification, as published by the Tool Interface Standards (TIS). */

/* Enumerations. */

/* File classes. */

enum
{
  /* No class. Bad ELF. */

  ELF_CLASS_NONE,

  /* 32-bit ELF. */

  ELF_CLASS_32BIT,

  /* 64-bit ELF. */

  ELF_CLASS_64BIT,
};

/* File types. */

enum
{
  /* No file type. */

  ELF_TYPE_NONE, 
  
  /* Relocatable file (i.e. position independant code, or PIC) */

  ELF_TYPE_RELOCATABLE,
  
  /* Executable file. */

  ELF_TYPE_EXECUTABLE,

  /* Shared object file. */

  ELF_TYPE_DYNAMIC,
  
  /* Core dump. */

  ELF_TYPE_CORE,
};

/* Machine types. */

enum
{
  /* No machne. */

  ELF_MACHINE_NONE,

  /* AT&T WE 32100 */

  ELF_MACHINE_WE32100,

  /* Sparc. */

  ELF_MACHINE_SPARC,

  /* Intel 80386. */

  ELF_MACHINE_386,

  /* Motorola 68000. */

  ELF_MACHINE_68000,
  
  /* Motorola 88000. */

  ELF_MACHINE_88000,

  /* Intel 80860. */

  ELF_MACHINE_860,

  /* MIPS RS3000. */

  ELF_MACHINE_RS3000,
};

/* ELF versions. */

enum
{
  /* Invalid version. */

  ELF_VERSION_NONE,

  /* Current version. */

  ELF_VERSION_CURRENT,
};

/* Endianess. */

enum
{
  /* No endian (invalid). */

  ELF_ENDIAN_NONE,

  /* Big endian. */

  ELF_ENDIAN_BIG,

  /* Little endian. */

  ELF_ENDIAN_LITTLE,
};

/* Section types. */

enum
{
  /* This section is inactive. */

  ELF_SECTION_TYPE_NULL,

  /* This section is used for something defined by the program
     itself. */

  ELF_SECTION_TYPE_UNDEFINED,

  /* This section holds the symbol table. May only exist once in the
     file. */

  ELF_SECTION_TYPE_SYMBOL_TABLE,

  /* This section holds the string table. May only exist once in the
     file. */

  ELF_SECTION_TYPE_STRING_TABLE,

  /* This section holds relocation entries with explicit addends
     (?). */

  ELF_SECTION_TYPE_RELOCATION_ADDENDS,

  /* This section holds a symbol hash table. */

  ELF_SECTION_TYPE_SYMBOL_HASH_TABLE,
  
  /* This section holds information for dynamic linking. */
  
  ELF_SECTION_TYPE_DYNAMIC,

  /* This section holds information that marks the file in some
     way. */

  ELF_SECTION_TYPE_NOTE,

  /* This section does not take up any space in the ELF file. Most
     often used for BSS sections. */

  ELF_SECTION_TYPE_NO_SPACE,

  /* This section holds relocation entries without explicit
     addends. */

  ELF_SECTION_TYPE_RELOCATION,
  
  /* This section type is reserved by has unspecified
     semantics. Programs that contain a section of this type does not
     conform to the ABI. */

  ELF_SECTION_TYPE_SHARED_LIBRARY,
};

/* Section flags. */

enum
{
  /* This section contains data that should be writable during process
     execution. */
  
  ELF_SECTION_FLAG_WRITE = SYSTEM_BIT_VALUE (0),

  /* This section occupies memory during process execution. */

  ELF_SECTION_FLAG_ALLOCATE = SYSTEM_BIT_VALUE (1),

  /* This section contains code that should be executable. */

  ELF_SECTION_FLAG_EXECUTE = SYSTEM_BIT_VALUE (2),
};

/* Type definitions. */

/* An ELF (Executable and linkable format) header. */

typedef struct 
{
  /* Must be 0x7F, 'E, 'L', 'F' in this order. */
  
  u8 identification[4]; 

  /* The class of this executable. ELF_CLASS_WHATEVER. */

  u8 class;

  /* The endianess of the data in this file. ELF_ENDIAN_* */

  u8 endian;

  /* Current version is ELF_VERSION_CURRENT. */

  u8 version;

  /* Should be zero. FreeBSD uses those to put 'FreeBSD' in the ELF
     header. We could do the same... */

  u8 pad[9];

  /* The type of ELF. ELF_TYPE_WHATEVER. */
  
  u16 type;   

  /* The machine type this ELF is designed to run
     on. ELF_MACHINE_WHATEVER. */
  
  u16 machine;

  /* Current version is still ELF_VERSION_CURRENT. (Don't ask me why
     they put the version ID in two places...) */

  u32 version2;

  /* The location of the program entry point. */

  u32 entry_point;

  /* Offset of program header table. */

  u32 program_header_offset;

  /* Offset of section header table. */

  u32 section_header_offset;
  u32 flags;   

  /* The size of the ELF header. */
  
  u16 elf_header_size;

  /* The size of a program header table entry. */

  u16 program_header_entry_size;

  /* The number of program header entries. */

  u16 program_header_entries;

  /* The size of a section header table entry. */

  u16 section_header_entry_size;

  /* The number of section header entries. */

  u16 section_header_entries;

  /* The section header table index of the section name string
     table. */

  u16 section_string_index;
} __attribute__ ((packed)) elf_header_type;

/* A section header entry. */

typedef struct
{
  /* The name of the section (index of the string table). */

  u32 name;
  u32 type;
  u32 flags;     

  /* The start of the section in memory. */
  
  u32 address;
  
  /* The start of the section in the file. */

  u32 offset;

  /* The size of the section. */

  u32 size;
  u32 link;     
  u32 info;
  u32 address_align;

  /* The size of each section entry. */

  u32 entry_size;
} __attribute__ ((packed)) section_header_type;

/* Function prototypes. */

return_type execute_elf (elf_header_type *elf_header, char *parameter_string,
                         process_id_type *child_process_id);

#endif /* !__LIBRARY_EXECUTE_ELF_EXECUTE_ELF_H__ */
