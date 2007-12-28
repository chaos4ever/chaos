/* $Id$ */
/* Abstract: Interrupt and IRQ management. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */
/* Copyright 2007 chaos development. */

/* Define this as TRUE to get debug information. */

#define DEBUG FALSE

#include <storm/state.h>
#include <storm/generic/cpu.h>
#include <storm/generic/dispatch.h>
#include <storm/ia32/gdt.h>
#include <storm/ia32/idt.h>
#include <storm/generic/irq.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/port.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>

#define BASE_IRQ                        0x20
#define IDT_ENTRY(a)                    (BASE_IRQ + a)

irq_type irq[IRQ_LEVELS];
//static mutex_kernel_type irq_mutex = MUTEX_UNLOCKED;

/* Just a short delay, to wait for the IRQ controller. */

static void delay (void)
{
  asm
  ("nop\n"
   "nop\n"
   "nop\n"
   "jmp 1f\n"
   "1:");
}

/* Remap all IRQ:s to 0x20-0x30. */

static void INIT_CODE irq_remap (void)
{
  /* Send initialisation sequence to 8259A-1 and 8259A-2. */

  port_out_u8 (INTERRUPT_CONTROLLER_MASTER, 0x11);
  delay ();
  port_out_u8 (INTERRUPT_CONTROLLER_SLAVE, 0x11);
  delay ();

  /* Start of hardware int's (0x20). */

  port_out_u8 (INTERRUPT_CONTROLLER_MASTER + 1, BASE_IRQ);
  delay ();

  /* Start of hardware int's 2 (0x28). */

  port_out_u8 (INTERRUPT_CONTROLLER_SLAVE + 1, BASE_IRQ + 8);
  delay ();

  /* 8259-1 is master. */

  port_out_u8 (INTERRUPT_CONTROLLER_MASTER + 1, 0x04);
  delay ();

  /* 8259-2 is slave. */

  port_out_u8 (INTERRUPT_CONTROLLER_SLAVE + 1, 0x02);
  delay ();

  /* 8086 mode for both. */

  port_out_u8 (INTERRUPT_CONTROLLER_MASTER + 1, 0x01);
  delay ();
  port_out_u8 (INTERRUPT_CONTROLLER_SLAVE + 1, 0x01);
  delay ();
}

/* Enable the given interrupt. */

void irq_enable (unsigned int irq_number)
{
  u8 mask;

  /* Check which interrupt controller to use. */

  if (irq_number < 8)
  {
    mask = port_in_u8 (INTERRUPT_CONTROLLER_MASTER + 1) &
      ~BIT_VALUE (irq_number);
    port_out_u8 (INTERRUPT_CONTROLLER_MASTER + 1, mask);
  }
  else
  {
    mask = port_in_u8 (INTERRUPT_CONTROLLER_SLAVE + 1) &
      ~BIT_VALUE (irq_number - 8);
    port_out_u8 (INTERRUPT_CONTROLLER_SLAVE + 1, mask);
  }
}

/* Disable the given interrupt. */

void irq_disable (unsigned int irq_number)
{
  u8 mask;

  /* Check which interrupt controller to use. */
  
  if (irq_number < 8)
  {
    mask = port_in_u8 (INTERRUPT_CONTROLLER_MASTER + 1) | 
      BIT_VALUE (irq_number);
    port_out_u8 (INTERRUPT_CONTROLLER_MASTER + 1, mask);
  }
  else
  {
    mask = port_in_u8 (INTERRUPT_CONTROLLER_SLAVE + 1) |
      BIT_VALUE (irq_number - 8);
    port_out_u8 (INTERRUPT_CONTROLLER_SLAVE + 1, mask);
  }
}

/* Initialise interrupts. */

void irq_init (void)
{
  memory_set_u8 ((u8 *) irq, 0, sizeof (irq_type) * IRQ_LEVELS);

  /* Remap the IRQs to 20-2F. The defaults are Very Bad, since they
     overlap the exceptions... */

  irq_remap ();

  /* Set up a handler for the task switcher. */

  idt_setup_interrupt_gate (IDT_ENTRY(0), SELECTOR_KERNEL_CODE,
                            dispatch_task_switcher, 0);

  idt_setup_interrupt_gate (IDT_ENTRY(1), SELECTOR_KERNEL_CODE,
                            irq1_handler, 0);

  /* IRQ 2 is used to connect the two interrupt controllers and can
     not be used as an ordinary IRQ. */

  idt_setup_interrupt_gate (IDT_ENTRY(3), SELECTOR_KERNEL_CODE,
                            irq3_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(4), SELECTOR_KERNEL_CODE,
                            irq4_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(5), SELECTOR_KERNEL_CODE,
                            irq5_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(6), SELECTOR_KERNEL_CODE,
                            irq6_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(7), SELECTOR_KERNEL_CODE,
                            irq7_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(8), SELECTOR_KERNEL_CODE,
                            irq8_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(9), SELECTOR_KERNEL_CODE,
                            irq9_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(10), SELECTOR_KERNEL_CODE,
                            irq10_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(11), SELECTOR_KERNEL_CODE,
                            irq11_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(12), SELECTOR_KERNEL_CODE,
                            irq12_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(13), SELECTOR_KERNEL_CODE,
                            irq13_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(14), SELECTOR_KERNEL_CODE,
                            irq14_handler, 0);
  idt_setup_interrupt_gate (IDT_ENTRY(15), SELECTOR_KERNEL_CODE,
                            irq15_handler, 0);

  /* Disable all IRQs. */

  port_out_u8 (INTERRUPT_CONTROLLER_MASTER + 1, 0xFF);
  port_out_u8 (INTERRUPT_CONTROLLER_SLAVE + 1, 0xFF);

  /* Allocate IRQ 0 and 2 for the system. */

  irq[0].allocated = TRUE;
  irq[0].process_id = PROCESS_ID_KERNEL;
  irq[0].cluster_id = CLUSTER_ID_KERNEL;
  irq[0].thread_id = THREAD_ID_KERNEL;
  irq[0].description = "System timer";
  irq_enable (0);
  
  irq[2].allocated = TRUE;
  irq[2].process_id = PROCESS_ID_KERNEL;
  irq[2].cluster_id = CLUSTER_ID_KERNEL;
  irq[2].thread_id = THREAD_ID_KERNEL;
  irq[2].description = "Cascade IRQ";
  irq_enable (2);
  
  /* Enable interrupts. */

  cpu_interrupts_enable ();
}

/* Register an IRQ for use by a server. */

return_type irq_register (unsigned int irq_number, char *description)
{
  DEBUG_MESSAGE (DEBUG, "Registering interrupt");

  /* Make sure the input data is pure. */

  if (irq_number >= IRQ_LEVELS)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  mutex_kernel_wait (&tss_tree_mutex);

  /* Make sure the IRQ level is free. */
  
  if (irq[irq_number].allocated)
  {
    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_BUSY;
  }

  /* Update the information in the IRQ structure. */

  irq[irq_number].description =
    memory_global_allocate (string_length (description) + 1);
  string_copy (irq[irq_number].description, description);

  irq[irq_number].allocated = TRUE;
  irq[irq_number].process_id = current_tss->process_id;
  irq[irq_number].cluster_id = current_tss->cluster_id;
  irq[irq_number].thread_id = current_tss->thread_id;
  irq[irq_number].interrupts_pending = 0;
  irq[irq_number].tss = current_tss;
  irq_enable (irq_number);

  mutex_kernel_signal (&tss_tree_mutex);

  return STORM_RETURN_SUCCESS;
}

/* Unregister a previously registered IRQ. */

return_type irq_unregister (unsigned int irq_number)
{
  /* Make sure we're not being tricked to unregister someone else's
     IRQ. */

  if (!irq[irq_number].allocated ||
      irq[irq_number].process_id != current_process_id)
  {
    return STORM_RETURN_ACCESS_DENIED;
  }

  mutex_kernel_wait (&tss_tree_mutex);

  memory_global_deallocate (irq[irq_number].description);
  irq[irq_number].allocated = FALSE;
  irq_disable (irq_number);

  mutex_kernel_signal (&tss_tree_mutex);

  return STORM_RETURN_SUCCESS;
}

/* This function handles all interrupts except for the timer
   interrupt. */

void irq_handler (unsigned int irq_number)
{
  DEBUG_MESSAGE (DEBUG, "irq %u occured.", irq_number);

  irq[irq_number].in_handler = TRUE;
  irq[irq_number].occurred++;

  if (!irq[irq_number].allocated)
  {
    DEBUG_MESSAGE (DEBUG, "Unexpected interrupt %u occured!", irq_number);
  }
  else
  {
    storm_tss_type *tss = irq[irq_number].tss;

    if (tss->state == STATE_INTERRUPT_WAIT)
    {
      DEBUG_MESSAGE (DEBUG, "%p %s", tss, tss->thread_name);
      tss->state = STATE_DISPATCH;
    }
    else
    {
      DEBUG_MESSAGE (DEBUG, "Queuing interrupt.");
      irq[irq_number].interrupts_pending++;
    }
  }
  DEBUG_MESSAGE (DEBUG, "Retoorning");

  irq[irq_number].in_handler = FALSE;
}

/* Free all IRQs for a certain thread. */
/* FIXME: This should take a thread_id as argument. */
/* FIXME: Find a better name for this. */

void irq_free_all (thread_id_type thread_id)
{
  unsigned int index;

  for (index = 0; index < IRQ_LEVELS; index++)
  {
    if (irq[index].allocated &&
        irq[index].thread_id == thread_id)
    {
      irq[index].allocated = FALSE;
    }
  }
}

/* Wait for the next interrupt, or if we have pending interrupts,
   return immediately. */

return_type irq_wait (unsigned int irq_number)
{
  /* If we currently are in the interrupt handler, wait until we are
     not. */
  /* FIXME: We must use a mutex here. Or something. Right now, we
     just hope and pray we won't get another interrupt during the
     execution of the following... */

  //  DEBUG_MESSAGE (TRUE, "irq: %u", irq_number);

  while (irq[irq_number].in_handler);

  if (!irq[irq_number].allocated ||
      irq[irq_number].thread_id != current_tss->thread_id)

  {
    DEBUG_MESSAGE (TRUE, "Access denied");
    return STORM_RETURN_ACCESS_DENIED;
  }

  if (irq[irq_number].interrupts_pending > 0)
  {
    DEBUG_MESSAGE (DEBUG, "Interrupts pending %u",
                   irq[irq_number].interrupts_pending);
    irq[irq_number].interrupts_pending--;
    return STORM_RETURN_SUCCESS;
  }

  current_tss->state = STATE_INTERRUPT_WAIT;
  dispatch_next ();

  DEBUG_MESSAGE (DEBUG, "Retoorning");
  return STORM_RETURN_SUCCESS;
}

/* Acknowledge the given IRQ level. */

return_type irq_acknowledge (unsigned int irq_number)
{
  //  DEBUG_MESSAGE (TRUE, "irq: %u", irq_number);

  if (!irq[irq_number].allocated ||
      irq[irq_number].thread_id != current_tss->thread_id)

  {
    DEBUG_MESSAGE (DEBUG, "Access denied");
    return STORM_RETURN_ACCESS_DENIED;

  }
  
  /* If this is a low interrupt, ACK:ing the low PIC is enough;
     otherwise, we'll have to do the other one too. */

  if (irq_number < 8)
  {
    port_out_u8 (0x20, 0x20);
  }
  else
  {
    port_out_u8 (0xA0, 0x20);
    port_out_u8 (0x20, 0x20);
  }

  return STORM_RETURN_SUCCESS;
}
