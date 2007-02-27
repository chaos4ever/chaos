/* $Id$ */
/* Abstract: Handle CPU exceptions. */

/* Authors: Henrik Hallin <hal@chaosdev.org>
            Johan Thim <nospam@inter.net>
            Per Lundberg <plundis@chaosdev.org> */

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

/* Define this when debugging this module. */

#define DEBUG FALSE

/* Define this if you want to halt the system whenever an unhandled
   exception occurs. (Sometimes, the system will triple fault otherwise) */

#undef HALT_ON_ERROR

#include <storm/state.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/ia32/cpu.h>
#include <storm/ia32/descriptor.h>
#include <storm/ia32/exception.h>
#include <storm/ia32/gdt.h>
#include <storm/ia32/idt.h>

static void *exception_stack;

/* Now, it's time for some exception handlers. */

static void exception_divide_error_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Divide error fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_debug_trap (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Debug trap/fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_nmi (void)
{
  debug_crash_screen ("NMI received", current_thread);
  DEBUG_HALT ("System halted.");
}

static void exception_breakpoint_trap (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Breakpoint trap", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_overflow_trap (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Overflow trap", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_bound_range_exceeded_fault (void)
{
  /* FIXME: What do we do here? */

  while (TRUE)
  {
    debug_crash_screen ("Bound range exceeded fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_invalid_opcode_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Invalid opcode fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_device_not_available_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Device not available fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_double_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Double fault (probably due to a kernel bug)",
                        current_thread);
    cpu_halt ();
  }
}

static void exception_coprocessor_segment_overrun_abort (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Coprocessor segment overrun abort", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_invalid_tss_fault (void)
{
  debug_crash_screen ("Invalid TSS fault", current_thread);
  DEBUG_HALT ("System halted.");
}

static void exception_segment_not_present_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Segment not present fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_stack_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Stack fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_general_protection_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("General protection fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_page_fault (void)
{
  u32 address;

  while (TRUE)
  {
    /* We need to check if this is a fault caused by the stack. */
    
    address = cpu_get_cr2 ();
    
    /* If this pagefault is caused by a growing stack, just map more
       memory. */
    
#if FALSE
    if (address >= BASE_PROCESS_STACK)
    {
      /* FIXME: This code should not really be here, but right now, it's good
         for debugging... */

      debug_crash_screen ("Stack overflow", current_thread);
      current_thread->state = STATE_ZOMBIE;
      //      if (current_task == TASK_ID_KERNEL)
      //      {
        //        cpu_halt ();
      //      }
      dispatch_next ();

      /* FIXME: End of temporary code. */

      DEBUG_MESSAGE (DEBUG, "Stack is growing (was %u pages)",
                     current_thread->stack_pages);
      if (memory_mutex == MUTEX_LOCKED)
      {
        DEBUG_MESSAGE (DEBUG, "Memory mutex was locked.");
      }
      else
      {
        u32 physical_page;

        mutex_kernel_wait (&memory_mutex);

        /* FIXME: Check return value. */

        memory_physical_allocate (&physical_page, 1, "Growing stack.");

        memory_virtual_map_other (current_thread, GET_PAGE_NUMBER (address),
                                  physical_page, 1,
                                  PAGE_WRITABLE | PAGE_NON_PRIVILEGED);
        mutex_kernel_signal (&memory_mutex);
        
        current_thread->stack_pages++;
        DEBUG_MESSAGE (DEBUG, "Done growing stack.");
      }
    }

    /* Ugly hack to detect (some) kernel stack overruns. */

    else if (address >= BASE_PROCESS_STACK - SIZE_PAGE)
    {
      debug_crash_screen ("Kernel stack overrun", current_thread);
      current_thread->state = STATE_ZOMBIE;

#ifdef HALT_ON_ERROR
      debug_run ();
#endif

      dispatch_next ();
    }
    
    /* ...or else, kill the thread. */
    
    else
#endif
    {
      debug_crash_screen ("Illegal page fault", current_thread);
      current_thread->state = STATE_ZOMBIE;

#ifdef HALT_ON_ERROR
      debug_run ();
#endif

      dispatch_next ();
    }

    asm volatile ("iret");
  }
}  

static void exception_klotis_fault (void)
{
  debug_crash_screen ("Buggy CPU non-fault", current_thread);
  DEBUG_HALT ("System halted.");
}

static void exception_floating_point_error_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Floating point error fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_alignment_check_fault (void)
{
  while (TRUE)
  {
    debug_crash_screen ("Alignment check fault", current_thread);
    current_thread->state = STATE_ZOMBIE;
    dispatch_next ();
  }
}

static void exception_machine_check_abort (void)
{
  debug_crash_screen ("Machine check abort", current_thread);
  DEBUG_HALT ("System halted.");
}

/* Initialise exceptions. */

static void setup_handler (u32 number, cpu_task_type *exception_tss,
                           void *exception_handler)
{
  /* Use the kernel's page directory. */

  exception_tss->cr3 = (u32) kernel_page_directory;

  /* Set the flags to AF only. */

  exception_tss->eflags = CPU_FLAG_ADJUST;

  exception_tss->eip = (u32) exception_handler;
  exception_tss->esp = (u32) exception_stack;

  /* Set up segment selectors. */

  exception_tss->cs = SELECTOR_KERNEL_CODE;
  exception_tss->ss = SELECTOR_KERNEL_DATA;
  exception_tss->ds = SELECTOR_KERNEL_DATA;
  exception_tss->es = SELECTOR_KERNEL_DATA;
  exception_tss->fs = SELECTOR_KERNEL_DATA;
  exception_tss->gs = SELECTOR_KERNEL_DATA;

  /* Add it to the GDT and IDT. */

  gdt_setup_tss_descriptor (GDT_BASE_EXCEPTIONS + number, exception_tss,
                            0, sizeof (cpu_task_type));
  idt_setup_task_gate (number, GDT (GDT_BASE_EXCEPTIONS + number, 0), 0);
}

/* Initialise exceptions. */

void exception_init (void)
{
  cpu_task_type *exception_tss;

  debug_print ("Setting up exception handlers...\n");

  /* Allocate one page for the exception TSS:es. */

  exception_tss = memory_physical_allocate ();

  if (exception_tss == NULL)
  {
    DEBUG_HALT ("Out of memory!");
  }

  /* Allocate a page for the exception stack. */

  exception_stack = memory_physical_allocate ();

  if (exception_stack == NULL)
  {
    DEBUG_HALT ("Out of memory!");
  }

  /* Wipe the TSS:es. */

  memory_set_u8 ((u8 *) exception_tss, 0, SIZE_PAGE); 

  /* Setup exception handlers for all exceptions. */

  setup_handler (0, &exception_tss[0], exception_divide_error_fault);
  setup_handler (1, &exception_tss[1], exception_debug_trap);
  setup_handler (2, &exception_tss[2], exception_nmi);
  setup_handler (3, &exception_tss[3], exception_breakpoint_trap);
  setup_handler (4, &exception_tss[4], exception_overflow_trap);
  setup_handler (5, &exception_tss[5], exception_bound_range_exceeded_fault);
  setup_handler (6, &exception_tss[6], exception_invalid_opcode_fault);
  setup_handler (7, &exception_tss[7], exception_device_not_available_fault);
  setup_handler (8, &exception_tss[8], exception_double_fault);
  setup_handler (9, &exception_tss[9], exception_coprocessor_segment_overrun_abort);
  setup_handler (10, &exception_tss[10], exception_invalid_tss_fault);
  setup_handler (11, &exception_tss[11], exception_segment_not_present_fault);
  setup_handler (12, &exception_tss[12], exception_stack_fault);
  setup_handler (13, &exception_tss[13], exception_general_protection_fault);
  setup_handler (14, &exception_tss[14], exception_page_fault);
  setup_handler (15, &exception_tss[15], exception_klotis_fault);
  setup_handler (16, &exception_tss[16], exception_floating_point_error_fault);
  setup_handler (17, &exception_tss[17], exception_alignment_check_fault);
  setup_handler (18, &exception_tss[18], exception_machine_check_abort);
}
