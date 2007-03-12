/* $Id: kernel.ld,v 1.1.1.1 2000/09/26 19:08:16 plundis Exp $ */
/* Abstract: storm linker script. Heavily based on the elf_i386.xbn
   from GNU binutils. */
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

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
  /* Read-only sections, merged into text segment: */

  . = 0x100000;
  .init.pre  :
  {
    *(.init.pre)
  }

  _init_start = .;

  .init      :
  {
    *(.text.init)
    *(.data.init)
  }

  . = ALIGN (0x1000);
  _init_end = .;

  .text      :
  {
    *(.text)
    *(.text.*)
    *(.stub)

    /* .gnu.warning sections are handled specially by elf32.em.  */

    *(.gnu.warning)
    *(.gnu.linkonce.t*)
  } =0x9090
  _etext = .;
  PROVIDE (etext = .);
  .fini      :
  {
    KEEP (*(.fini))
  } =0x9090
  .rodata   : { *(.rodata) *(.rodata.*) *(.gnu.linkonce.r*) }
  .rodata1   : { *(.rodata1) }

  /* Adjust the address for the data segment.  We want to adjust up to
     the same address within the page on the next page up.  */

  . = .;
  .data    :
  {
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d*)
    SORT(CONSTRUCTORS)
  }
  .data1   : { *(.data1) }
  .eh_frame : { *(.eh_frame) }
  .gcc_except_table : { *(.gcc_except_table) }
  .ctors   : 
  {
    /* gcc uses crtbegin.o to find the start of the constructors, so
       we make sure it is first.  Because this is a wildcard, it
       doesn't matter if the user does not actually link against
       crtbegin.o; the linker won't look for a file to match a
       wildcard.  The wildcard also means that it doesn't matter which
       directory crtbegin.o is in.  */

    KEEP (*crtbegin.o(.ctors))

    /* We don't want to include the .ctor section from from the
       crtend.o file until after the sorted ctors.  The .ctor section
       from the crtend file contains the end of ctors marker and it
       must be last */

    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  }
   .dtors         :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  }
  .got           : { *(.got.plt) *(.got) }
  .dynamic       : { *(.dynamic) }

  /* We want the small data sections together, so single-instruction
     offsets can access them all, and initialised data all before
     uninitialised, so we can shorten the on-disk segment size.  */

  .sdata     : 
  {
    *(.sdata) 
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
  }
  _edata = .;
  PROVIDE (edata = .);
  __bss_start = .;
  .sbss      :
  {
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.scommon)
  }
  .bss       :
  {
   *(.dynbss)
   *(.bss)
   *(.bss.*)
   *(COMMON)

   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.  */

   . = ALIGN(32 / 8);
  }
  . = ALIGN(32 / 8);
  _end = .;
  PROVIDE (end = .);

  /* Stabs debugging sections.  */

  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }

  /* DWARF debug sections. Symbols in the DWARF debugging sections
     are relative to the beginning of the section so we begin them at
     0. */

  /* DWARF 1 */

  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */

  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */

  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */

  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  /* SGI/MIPS DWARF 2 extensions */

  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  /* These must appear regardless of  .  */
}
