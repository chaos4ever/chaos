/* $Id$ */
/* Abstract: Function prototypes for GDT routines. */

/* Copyright 1998-2000 chaos development. */

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

#ifndef __STORM_IA32_GDT_H__
#define __STORM_IA32_GDT_H__

#include <storm/ia32/defines.h>
#include <storm/ia32/types.h>
#include <storm/ia32/descriptor.h>

/* Defines. */
/* Selectors. */

#define SELECTOR_KERNEL_CODE            GDT(1, 0)
#define SELECTOR_KERNEL_DATA            GDT(2, 0)
#define SELECTOR_PROCESS_CODE           GDT(3, 3)
#define SELECTOR_DATA                   GDT(4, 3)
#define SELECTOR_TSS1                   GDT(5, 3)
#define SELECTOR_TSS2                   GDT(6, 3)

/* The location of the exceptions in the GDT. */

#define GDT_BASE_EXCEPTIONS             (16)

/* External variables. */

extern descriptor_type *gdt;

/* Function prototypes. */

extern void gdt_add_entry (u16 number, descriptor_type *descriptor) INIT_CODE;

extern void gdt_setup_call_gate (u8 number, u16 selector, void *address,
                                 u8 dpl, u8 params) INIT_CODE;

extern void gdt_setup_tss_descriptor (u16 selector, void *address,
                                      int dpl, int limit);

/* Defines. */

#define GDT(n,p) (((n) << 3) + p)

#endif /* !__STORM_IA32_GDT_H__ */
