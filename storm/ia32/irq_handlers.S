/* $Id: irq_handlers.S,v 1.2 2000/10/16 17:06:48 plundis Exp $ */
/* Abstract: IRQ management. */
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

/* One interrupt handler for each interrupt, thank you verymuch. */

.globl irq1_handler
.globl irq3_handler
.globl irq4_handler
.globl irq5_handler
.globl irq6_handler
.globl irq7_handler
.globl irq8_handler
.globl irq9_handler
.globl irq10_handler
.globl irq11_handler
.globl irq12_handler
.globl irq13_handler
.globl irq14_handler
.globl irq15_handler

.extern irq_handler

#define IRQ_HANDLER_HIGH(x) \
   pushal ;\
   pushl $x ;\
   call irq_handler ;\
   addl $4, %esp ;\
   popal ;\
   iret

#define IRQ_HANDLER_LOW(x) \
   pushal ;\
   pushl $x ;\
   call irq_handler ;\
   addl $4, %esp ;\
   popal ;\
   iret

irq1_handler:
   IRQ_HANDLER_LOW(1)

irq3_handler:
   IRQ_HANDLER_LOW(3)

irq4_handler:
   IRQ_HANDLER_LOW(4)

irq5_handler:
   IRQ_HANDLER_LOW(5)

irq6_handler:
   IRQ_HANDLER_LOW(6)

irq7_handler:
   IRQ_HANDLER_LOW(7)

irq8_handler:
   IRQ_HANDLER_HIGH(8)

irq9_handler:
   IRQ_HANDLER_HIGH(9)

irq10_handler:
   IRQ_HANDLER_HIGH(10)

irq11_handler:
   IRQ_HANDLER_HIGH(11)

irq12_handler:
   IRQ_HANDLER_HIGH(12)

irq13_handler:
   IRQ_HANDLER_HIGH(13)

irq14_handler:
   IRQ_HANDLER_HIGH(14)

irq15_handler:
   IRQ_HANDLER_HIGH(15)
