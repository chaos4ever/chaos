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

#include <storm/generic/arguments.h>
#include <storm/generic/idle.h>
#include <storm/generic/dataarea.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/irq.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/port.h>
#include <storm/generic/types.h>

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

static u16 idtr[] __attribute__ ((unused)) INIT_DATA = 
{
  /* IDT limit, 256 IDT entries. */

  0x7FF,

  /* IDT base. */
  
  LOW_U16 (BASE_IDT),
  HIGH_U16 (BASE_IDT)
};

static u16 gdtr[] __attribute__ ((unused)) INIT_DATA =
{ 
  /* GDT limit, 256 GDT entries. */

  0x7FF,

  /* GDT base. */

  LOW_U16 (BASE_GDT),
  HIGH_U16 (BASE_GDT)
};

/* Multiboot header. */

static u32 multiboot_header[] __attribute__ ((section (".init.pre"), unused)) = 
{
  MULTIBOOT_MAGIC,
  MULTIBOOT_FLAGS,
  - (MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
};
                                     
/* FIXME: Put those in another file. We should have a better system for local
   include files... */

void _start (void) INIT_CODE;
extern int main (int arguments, char *argument[]);

/* This is the kernel. :) */
static void INIT_CODE kernel_entry (void) __attribute__ ((noreturn));
static void INIT_CODE kernel_entry (void) 
{
  multiboot_init ();
  
  /* FIXME: Remove the pointer arithmetic. */

  main (((u32 *) arguments_kernel)[0], (char **) arguments_kernel + 1);

  /* Get in line and float downstream. */

  idle ();
}

/* This is the kernel entry point. */
/* FIXME: Use memory_copy and memory_set instead of inline
   assembler. This is a little hard to combine with MMX optimized
   operations... */

void _start (void)
{
  u8 *bios_data_area = (u8 *) 0x400;

  /* EBX contains the address to the multiboot table. Save this in the
     kernel data table. */

  asm ("movl    %0, %%edi
        movl    %%ebx, %%esi
        movl    %1, %%ecx
        rep     movsl"
       :
       : "g" ((u32) &multiboot_info), "g" (sizeof (multiboot_info_type) / 4));

  /* Store the screen size in the kernel data area. 
     FIXME: Update to new Multiboot video-standard. */

  dataarea.x_size = bios_data_area[0x4A];
  dataarea.y_size = bios_data_area[0x84] + 1;

  /* Also read the screen position. */

  dataarea.x_position = bios_data_area[0x50];
  dataarea.y_position = bios_data_area[0x51];
                
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

  /* Wipe them out. All of them. */

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
