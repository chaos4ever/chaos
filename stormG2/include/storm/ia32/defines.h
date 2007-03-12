/* $Id$ */
/* Abstract: IA32 specific defines. */

/* Author: Per Lundberg <plundis@chaosdev.org> 
           Henrik Hallin <hal@chaosdev.org> 
           Anders Öhrt <doa@chaosdev.org> */

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
   USA. */

#ifndef __STORM_IA32_DEFINES_H__
#define __STORM_IA32_DEFINES_H__

#include <storm/generic/types.h>

/* The size of a physical page. */

#define SIZE_PAGE                       4096

/* Size of some reserved regions. */

#define SIZE_GDT                        (2 * KB)
#define SIZE_IDT                        (2 * KB)
#define SIZE_KERNEL_STACK               (4 * KB)
#define SIZE_EXCEPTION_STACK            (4 * KB)
#define SIZE_MODULE_NAME                (2 * KB)
#define SIZE_MEMORY_MAP                 (2 * KB)
#define SIZE_DMA_MEMORY                 (512 * KB)
#define SIZE_KERNEL                     ((u32) &_end - BASE_KERNEL)

/* Physical base addresses (some are linear too). */

#define BASE_GDT                        (4 * KB)
#define BASE_IDT                        (BASE_GDT + SIZE_GDT)
#define BASE_KERNEL_STACK               (BASE_IDT + SIZE_IDT)
#define BASE_MODULE_NAME                (BASE_KERNEL_STACK + SIZE_KERNEL_STACK)
#define BASE_MEMORY_MAP                 (BASE_MODULE_NAME + SIZE_MODULE_NAME)
#define BASE_FIRST_FREE_PHYSICAL_MEMORY (BASE_MODULE_NAME + \
                                         SIZE_MODULE_NAME)

/* FIXME: This is temporary. Use a memory map instead. */

#define BASE_RESERVED_MEMORY            (600 * KB)
#define BASE_KERNEL                     (1 * MB)

/* Virtual addresses. */

#define BASE_PHYSICAL_MEMORY            (2 * GB)

#endif /* !__STORM_IA32_DEFINES_H__ */
