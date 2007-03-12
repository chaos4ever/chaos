/* $Id$ */
/* Abstract: IA32 specific defines. */
/* Authors: Per Lundberg <plundis@chaosdev.org> 
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

#include <storm/generic/limits.h>
#include <storm/generic/types.h>

/* FIXME: More structure, less BS. */

#define MAX_PAGES                       ((u32) ((4ULL * GB) / SIZE_PAGE))

/* The maximum number of AVL nodes in the virtual allocation tree. */

#define MAX_VIRTUAL_AVL_NODES           100000

/* Physical base addresses (some are linear too). */

#define BASE_KERNEL                     (1 * MB)
#define BASE_KERNEL_TSS                 (4 * KB)
#define BASE_KERNEL_STACK               (8 * KB)
#define BASE_MODULE_NAME                (16 * KB)
#define BASE_TRAP_STACK                 (20 * KB)
#define BASE_GDT                        (44 * KB)
#define BASE_IDT                        (BASE_GDT + SIZE_GDT)
#define BASE_DMA                        (128 * KB)
#define BASE_UPPER                      (640 * KB)
#define BASE_SCREEN                     (0xB8000)

/* FIXME!!! The AVL tree should be at the end of physical memory. */

#define BASE_PAGE_AVL                   (3 * MB + 512 * KB)

/* Virtual addresses. */

#define BASE_VIRTUAL_KERNEL_TSS         (40 * KB)

/* Size of some reserved regions. */

#define SIZE_GDT_IDT                    (4 * KB)
#define SIZE_KERNEL_TSS                 (4 * KB)
#define SIZE_KERNEL_STACK               (8 * KB)
#define SIZE_TRAP_STACK                 (4 * KB)
#define SIZE_MODULE_NAME                (4 * KB)
#define SIZE_UPPER                      ((1024 - 640) * KB)
#define SIZE_DMA                        ((64 * 8) * KB)

/* Process virtual addresses. */

#define BASE_PROCESS_PAGE_DIRECTORY     (4 * KB)
#define BASE_PROCESS_TRAP_TSS           (8 * KB)
#define BASE_PROCESS_TEMPORARY          (32 * KB)
#define BASE_PROCESS_CREATE             (3 * MB)
#define BASE_PROCESS_AVL_TREE           (4 * MB)
#define BASE_PROCESS_PAGE_TABLES        (32 * MB)
#define BASE_PROCESS_PARAMETERS         (36 * MB)
#define BASE_PROCESS_STACK              ((4 * GB) - SIZE_PROCESS_STACK)

/* Kernel virtual addresses. Only used during bootup. */

#define BASE_MODULE                     (2 * GB)

/* Some sizes. */

#define SIZE_GDT                        (2 * KB)
#define SIZE_IDT                        (2 * KB)
#define SIZE_PROCESS_STACK              (64 * MB)
#define SIZE_TRAP_TSS                   (4 * KB)
#define SIZE_PROCESS_CREATE             (1 * MB)

/* Global memory sizes. */

#define SIZE_PHYSICAL_MEMORY_TREE       (SIZE_PAGE * (page_avl_pages))
#define GLOBAL_MEMORY_TREE_INTRO_PAGES  (SIZE_IN_PAGES (sizeof (avl_header_type) + limit_global_nodes / 8))
#define GLOBAL_MEMORY_TREE_ARRAY_PAGES  (SIZE_IN_PAGES (sizeof (avl_node_type) * limit_global_nodes))
#define SIZE_GLOBAL_MEMORY_TREE         ((GLOBAL_MEMORY_TREE_INTRO_PAGES + \
                                         GLOBAL_MEMORY_TREE_ARRAY_PAGES) * SIZE_PAGE)
#define SIZE_GLOBAL_HEAP                (limit_global_heap)

#define SIZE_GLOBAL                     (SIZE_PHYSICAL_MEMORY_TREE + \
                                         SIZE_GLOBAL_MEMORY_TREE + \
                                         SIZE_GLOBAL_HEAP)

/* Global memory locations. Linear addresses. */

#define BASE_GLOBAL                     (40 * MB)
#define BASE_PHYSICAL_MEMORY_TREE       (BASE_GLOBAL)
#define BASE_GLOBAL_MEMORY_TREE         (BASE_PHYSICAL_MEMORY_TREE + \
                                         SIZE_PHYSICAL_MEMORY_TREE)
#define BASE_GLOBAL_HEAP                (BASE_GLOBAL_MEMORY_TREE + \
                                         SIZE_GLOBAL_MEMORY_TREE)
#define BASE_PROCESS_SPACE_START        (BASE_GLOBAL + SIZE_GLOBAL)
#define BASE_PROCESS_SPACE_END          (3 * GB)

/* Convert a pointer to a page index. */

#define GET_PAGE_NUMBER(x)              ((u32) (x) / SIZE_PAGE)

/* Get the low or high u16 of an u32. */

#define LOW_U16(x)                      ((x) & 0xFFFF)
#define HIGH_U16(x)                     ((x) >> 16)

/* Get the low or high u8 of an u16. */

#define LOW_U8(x)                       ((x) & 0xFF)
#define HIGH_U8(x)                      ((x) >> 8)

/* Use those defines to put the given function and variables in
   special sections which are later freed. */

#define INIT_CODE                       __attribute__ ((section (".text.init")))
#define INIT_DATA                       __attribute__ ((section (".data.init")))

/* Use this define to attribute that the function will not ever return. */

#define NORETURN                        __attribute__ ((noreturn))

#endif /* !__STORM_IA32_DEFINES_H__ */
