// Abstract: Interrupt and IRQ related function prototypes and structure definitions.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1998-2000 chaos development.
// © Copyright 2013 chaos development.
// © Copyright 2015 chaos development.

#pragma once

// The base addresses of 8259-1 and 8259-2.
#define INTERRUPT_CONTROLLER_MASTER     0x20
#define INTERRUPT_CONTROLLER_SLAVE      0xA0

#define IRQ_LEVELS                      16

// Low level interrupt handlers.
extern void irq1_handler(void);
extern void irq3_handler(void);
extern void irq4_handler(void);
extern void irq5_handler(void);
extern void irq6_handler(void);
extern void irq7_handler(void);
extern void irq8_handler(void);
extern void irq9_handler(void);
extern void irq10_handler(void);
extern void irq11_handler(void);
extern void irq12_handler(void);
extern void irq13_handler(void);
extern void irq14_handler(void);
extern void irq15_handler(void);
