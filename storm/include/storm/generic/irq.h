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

#ifndef __STORM_GENERIC_IRQ_H__
#define __STORM_GENERIC_IRQ_H__

#include <storm/current-arch/irq.h>
#include <storm/types.h>

/* Function prototypes. */

extern void irq_init (void) INIT_CODE;
extern return_type irq_register (unsigned int irq_number, char *description);
extern return_type irq_unregister (unsigned int irq_number);
extern void irq_handler (unsigned int irq_number);
extern void irq_enable (unsigned int irq_number);
extern void irq_disable (unsigned int irq_number);
extern void irq_free_all (thread_id_type thread_id);
extern return_type irq_wait (unsigned int irq_number);
extern return_type irq_acknowledge (unsigned int irq_number);

/* Type definitions. */

typedef struct
{
  bool allocated;
  process_id_type process_id;
  cluster_id_type cluster_id;
  thread_id_type thread_id;

  /* The TSS for the handler. */

  storm_tss_type *tss;
  
  /* A short description of the IRQ. */

  char *description;

  /* The number of currently unhandled interrupts. */

  unsigned int interrupts_pending;
  
  /* The number of times this IRQ has been raised. */

  unsigned long long occurred;
  volatile bool in_handler;
} irq_type;

/* External variables. */

extern irq_type irq[IRQ_LEVELS];

#endif /* !__STORM_GENERIC_IRQ_H__ */
