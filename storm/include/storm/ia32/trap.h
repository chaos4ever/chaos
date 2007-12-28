/* $Id$ */
/* Abstract: Exception stuff. */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __STORM_IA32_TRAP_H__
#define __STORM_IA32_TRAP_H__

#include <storm/ia32/tss.h>
#include <storm/ia32/types.h>

/* Prototypes. */

extern void trap_init (void) INIT_CODE;

/* Inlines. */

/* FIXME: make these support popping errorcode from stack. */

static inline void trap_enter (void)
{
  asm 
  ("pushl %ds\n"
   "movw $1 << 3, %ax\n"
   "movw %ax, %ds\n"
   "cli");
}

static inline void trap_leave (void)
{
  asm 
  ("popl %ds\n"
   "iret");
}

/* External variables. */

extern tss_type *trap_tss;

#endif /* !__STORM_IA32_TRAP_H__ */
