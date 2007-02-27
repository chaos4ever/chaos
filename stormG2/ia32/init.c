/* $Id$ */
/* Abstract: Set up the GDT, stack, selectors and pass control to the
             kernel. */
/* Author: Per Lundberg <plundis@chaosdev.org>
           Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1997-2000 chaos development. */

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

#include <storm/generic/types.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/defines.h>
#include <storm/generic/memory.h>
#include <storm/generic/main.h>
#include <storm/ia32/cpu.h>
#include <storm/ia32/gdt.h>

/* The startup GDT we use. This is copied to its real location on startup,
   which is defined by BASE_GDT. For the descriptor format, see the Intel
   reference documentation. */

static u16 temporary_gdt[] INIT_DATA = 
{
  /* Null descriptor. Generates GPF on access. */
  
  0x0000,
  0x0000,
  0x0000,
  0x0000,

  /* Kernel code segment (exec). */

  0xFFFF,
  0x0000,
  0x9800,
  0x00CF,

  /* Process data segment (read/write). */

  0xFFFF,
  0x0000,
  0x9200,
  0x00CF,

  /* Process code segment (exec). */

  0xFFFF,
  0x0000,
  0xF800,
  0x00CF,

  /* Process data segment (read/write). */

  0xFFFF,
  0x0000,
  0xF200,
  0x00CF
};

/* The GDT ant IDT fits into the first physical page. */

static u16 idtr[] INIT_DATA UNUSED = 
{
  /* IDT limit, 256 IDT entries. */

  0x7FF,

  /* IDT base. */
  
  LOW_U16 (BASE_IDT),
  HIGH_U16 (BASE_IDT)
};

static u16 gdtr[] INIT_DATA UNUSED =
{ 
  /* GDT limit, 256 GDT entries. */

  0x7FF,

  /* GDT base. */

  LOW_U16 (BASE_GDT),
  HIGH_U16 (BASE_GDT)
};

/* This is the first code of the kernel that gets executed. (well,
   almost. _start () sets up some stuff first...) */

static void INIT_CODE kernel_entry (void)
{
  multiboot_init ();

  //  main (((u32 *) arguments_kernel)[0], (char **) arguments_kernel + 1);

  main_bootup (0, NULL);

  /* Get in line and float downstream. */

  //  idle ();

  while (TRUE);
}

/* Multiboot header. */

static u32 multiboot_header[] __attribute__ ((section (".init.pre"), unused)) = 
{
  MULTIBOOT_MAGIC,
  MULTIBOOT_FLAGS,
  - (MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
};
                                     
/* This is the kernel entry point. */

void _start (void);
void _start (void)
{
  /* EBX contains the address to the multiboot table. Save this in the
     kernel data table. We can NOT use memory_copy here, since that
     will need a stack. */

  asm ("movl    %0, %%edi
        movl    %%ebx, %%esi
        movl    %1, %%ecx
        rep     movsl"
       :
       : "g" ((u32) &multiboot_info), "g" (sizeof (multiboot_info_type) / 4));
  
  /* Move the GDT to the right location in memory. */
    
  asm ("cld
        movl    %0, %%edi
        movl    %1, %%esi
        movl    %2, %%ecx
        rep     movsl
        movl    %3, %%ecx
        movl    $0, %%eax
        rep     stosl"
       :
       : "g" ((u32) BASE_GDT),
         "g" ((u32) &temporary_gdt),
         "n" (sizeof (temporary_gdt) / 4),
         "n" ((0x800 - sizeof (temporary_gdt)) / 4));

  /* Clear the IDT. */

  asm ("movl    %0, %%edi
        movl    $0, %%eax
        movl    %1, %%ecx
        rep     stosl"
       :
       : "n" (BASE_IDT), "n" (SIZE_IDT / 4));

  /* Set up the GDTR and IDTR. */
  /* FIXME: Use constraints. */

  asm ("lgdt    gdtr
        lidt    idtr");

  /* Initialise the segment registers so they are loaded with our new
     selectors. */
  
  asm ("movl    %0, %%eax
        movw    %%ax, %%ss
        movl    %1, %%esp
        movw    %%ax, %%es
        movw    %%ax, %%fs
        movw    %%ax, %%gs
        movw    %%ax, %%ds"
       :
       : "n" (SELECTOR_KERNEL_DATA),
         "n" (BASE_KERNEL_STACK + SIZE_KERNEL_STACK));

  /* Pass control to the kernel. */

  asm ("ljmp   %0, %1"
       :
       : "n" (SELECTOR_KERNEL_CODE),
         "p" (&kernel_entry));
}
