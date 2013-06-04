// Abstract: IRQ management.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2013 chaos development.

// One interrupt handler for each interrupt, thank you verymuch.
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

#define IRQ_HANDLER(x) \
   pushal ;\
   pushl $x ;\
   call irq_handler ;\
   addl $4, %esp ;\
   popal ;\
   iret

irq1_handler:
   IRQ_HANDLER(1)

irq3_handler:
   IRQ_HANDLER(3)

irq4_handler:
   IRQ_HANDLER(4)

irq5_handler:
   IRQ_HANDLER(5)

irq6_handler:
   IRQ_HANDLER(6)

irq7_handler:
   IRQ_HANDLER(7)

irq8_handler:
   IRQ_HANDLER(8)

irq9_handler:
   IRQ_HANDLER(9)

irq10_handler:
   IRQ_HANDLER(10)

irq11_handler:
   IRQ_HANDLER(11)

irq12_handler:
   IRQ_HANDLER(12)

irq13_handler:
   IRQ_HANDLER(13)

irq14_handler:
   IRQ_HANDLER(14)

irq15_handler:
   IRQ_HANDLER(15)
