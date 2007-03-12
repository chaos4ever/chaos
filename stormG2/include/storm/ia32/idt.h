/* $Id$ */
/* Abstract: Function prototypes for IDT routines. */

/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development */

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

#ifndef __STORM_IA32_IDT_H__
#define __STORM_IA32_IDT_H__

#include <storm/generic/defines.h>
#include <storm/generic/types.h>
#include <storm/ia32/descriptor.h>

/* Function prototypes. */

extern void idt_setup_task_gate (unsigned int number, unsigned int selector, 
                                 unsigned int privilege_level);

extern void idt_setup_interrupt_gate 
  (unsigned int number, unsigned int selector, void *address, 
   unsigned int privilege_level);

/* Global variables. */

extern descriptor_type *idt;

#endif /* !__STORM_IA32_IDT_H__ */
