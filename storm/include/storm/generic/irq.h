// Abstract: Interrupt and IRQ related function prototypes and structure definitions.
// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>

// © Copyright 1998-2000, 2013 chaos development.

#pragma once

#include <storm/current-arch/irq.h>
#include <storm/types.h>

extern void irq_init(void) INIT_CODE;
extern return_type irq_register(unsigned int irq_number, char *description);
extern return_type irq_unregister(unsigned int irq_number);
extern void irq_handler(unsigned int irq_number);
extern void irq_enable(unsigned int irq_number);
extern void irq_disable(unsigned int irq_number);
extern void irq_free_all(thread_id_type thread_id);
extern return_type irq_wait(unsigned int irq_number);
extern return_type irq_acknowledge(unsigned int irq_number);

typedef struct
{
    bool allocated;
    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;

    // The TSS for the handler.
    storm_tss_type *tss;

    // A short description of the IRQ.
    char *description;

    // The number of currently unhandled interrupts.
    unsigned int interrupts_pending;

    // The number of times this IRQ has been raised.
    unsigned long long occurred;
    volatile bool in_handler;
} irq_type;

extern irq_type irq[IRQ_LEVELS];
