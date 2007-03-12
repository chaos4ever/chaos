/* $Id$ */
/* Abstract: Interrupt and IRQ related function prototypes and
   structure definitions. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

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
   USA. */

#ifndef __STORM_IA32_IRQ_H__
#define __STORM_IA32_IRQ_H__

/* Defines. */
/* The base addresses of 8259-1 and 8259-2. */

#define INTERRUPT_CONTROLLER_MASTER     0x20
#define INTERRUPT_CONTROLLER_SLAVE      0xA0

#define IRQ_LEVELS                      16

/* Low level interrupt handlers. */

extern void irq1_handler (void);
extern void irq3_handler (void);
extern void irq4_handler (void);
extern void irq5_handler (void);
extern void irq6_handler (void);
extern void irq7_handler (void);
extern void irq8_handler (void);
extern void irq9_handler (void);
extern void irq10_handler (void);
extern void irq11_handler (void);
extern void irq12_handler (void);
extern void irq13_handler (void);
extern void irq14_handler (void);
extern void irq15_handler (void);

#endif /* !__STORM_IA32_IRQ_H__ */
