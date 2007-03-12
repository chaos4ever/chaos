/* $Id$ */
/* Abstract: CPU detection and some lowlevel routines. */

/* Author: Per Lundberg <plundis@chaosdev.org>
           Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#ifndef __STORM_IA32_CPU_H__
#define __STORM_IA32_CPU_H__

#include <storm/generic/bit.h>
#include <storm/generic/defines.h>
#include <storm/ia32/memory.h>

/* Type definitions. */
/* A 'task state segment' structure type. */

typedef struct
{
  /* CPU data. */

  u16 previous_task_link;
  u16 u0;

  /* Stack pointer for PL0 code (system calls). */

  u32 esp0;              
  u16 ss0;
  u16 u1;
  u32 esp1;
  u16 ss1;
  u16 u2;
  u32 esp2;
  u16 ss2;
  u16 u3;
  u32 cr3;

  /* Instruction pointer and flags. */

  u32 eip;
  u32 eflags;

  /* General-purpose registers. */

  u32 eax;
  u32 ecx;
  u32 edx;
  u32 ebx;

  /* Stack pointer. */

  u32 esp;
  u32 ebp;
  u32 esi;
  u32 edi;
  u16 es;
  u16 u4;
  u16 cs;
  u16 u5;
  u16 ss;
  u16 u6;
  u16 ds;
  u16 u7;
  u16 fs;

  /* Unused field number 8... */

  u16 u8;
  u16 gs;
  u16 u9;

  /* This is not used by storm. */

  u16 ldt_selector;
  u16 u10;
  u16 t: 1;
  u16 u11: 15;

  /* Base address of I/O map. */

  u16 iomap_base;
} __attribute__ ((packed)) cpu_task_type;

/* CPU families. */

enum
{
  CPU_FAMILY_386 = 3,
  CPU_FAMILY_486,
  CPU_FAMILY_PENTIUM,
  CPU_FAMILY_P6
};

/* CPU information. */

typedef struct
{
  /* Phony variable. Since C is so stupid it won't let me have union
     elements accessed the same way as the rest of the structure, we
     have to hack it a little.. */

  u32 signature[0];

  /* CPU signature. */
  
  u32 stepping: 4;
  u32 model:    4;
  u32 family:   4;
  u32 type:     2;
  u32 s_res0:   18;

  u32 flags[0];

  /* Feature flags. */
  
  u32 fpu:       1;
  u32 vme:       1;
  u32 de:        1;
  u32 pse:       1;
  u32 tsc:       1;
  u32 msr:       1;
  u32 pae:       1;
  u32 mce:       1;
  u32 cx8:       1;
  u32 apic:      1;
  u32 ff_res0:   1;
  u32 sep:       1;
  u32 mtrr:      1;
  u32 pge:       1;
  u32 mca:       1;
  u32 cmov:      1;
  u32 pat:       1;
  u32 pse_36:    1;
  u32 ff_res1:   5;
  u32 mmx:       1;
  u32 fxsr:      1;
  u32 sse:       1;
  u32 ff_res2:   5;
  u32 amd_3dnow: 1;
} cpu_type;

/* External variables. */

extern u32 no_idt[2];
extern cpu_type cpu;

/* Function prototypes. */

extern void cpu_init (void);

/* Inlines. */

static inline void cpu_push_all (void)
{
  asm ("pusha");
}

static inline void cpu_pop_all (void)
{
 asm ("popa");
}

static inline void cpu_interrupt_return (void)
{
  asm ("iret");
}

static inline void cpu_interrupts_disable (void)
{
  asm ("cli");
}

static inline void cpu_interrupts_enable (void)
{
  asm ("sti");
}

static inline void cpu_halt (void)
{
  cpu_interrupts_disable ();
  while (TRUE);
}

static inline void cpu_no_operation (void)
{
  asm ("nop");
}

static inline u32 cpu_get_esp (void)
{
  u32 return_value;

  asm volatile ("movl %%esp, %0"
		: "=a" (return_value)
		:);

  return return_value;
}

static inline void cpu_set_esp (u32 new_esp)
{
  asm volatile ("movl %0, %%esp"
		:
		: "r" (new_esp));
}

static inline u32 cpu_get_ebx (void)
{
  u32 return_value;

  asm volatile ("movl %%ebx, %0"
		: "=a" (return_value)
		:);

  return return_value;
}

static inline u32 cpu_get_eflags (void)
{
  u32 return_value;

  asm volatile 
  ("\
    pushf
    popl %0"
   : "=a" (return_value));

  return return_value;
}

static inline u32 cpu_get_cs (void)
{
  u32 return_value;

  asm volatile ("movl %%cs, %0"
                : "=a" (return_value));
  return return_value;
}

static inline u32 cpu_get_cr0 (void)
{
  u32 return_value;
  asm volatile ("movl %%cr0, %0"
		: "=a" (return_value) 
		:);
  return return_value;
}

static inline u32 cpu_get_cr4 (void)
{
  u32 return_value;
  asm volatile ("movl %%cr4, %0"
		: "=a" (return_value) 
		:);
  return return_value;
}

static inline void cpu_set_cr0 (u32 new_cr0)
{
  asm volatile ("movl %0, %%cr0"
		:
		: "r" (new_cr0));
}

static inline u32 cpu_get_cr2 (void)
{			   
  u32 return_value;
  asm volatile ("movl %%cr2, %0"
		: "=a" (return_value) 
		:);
  return return_value;
}

static inline void cpu_set_cr3 (u32 new_cr3)
{
  asm volatile ("movl %0, %%cr3"
		:
		: "r" (new_cr3));
}

static inline void cpu_set_cr4 (u32 new_cr4)
{
  asm volatile ("movl %0, %%cr4"
		:
		: "r" (new_cr4));
}

static inline u16 cpu_get_tr (void)
{
  u16 return_value;
  asm volatile ("str %0"
		: "=a" (return_value)
		:);
  return return_value;
}

static inline void cpu_set_tr (u16 new_tr)
{
  asm volatile ("ltr %0"
		:
		: "r" (new_tr));
}

static inline void cpu_reset (void)
{
  /* Force a triple fault. */

  asm ("cli");
  memory_set_u8 ((u8 *) BASE_IDT, 0, SIZE_IDT);

  asm ("int $0x3");
}

/* CPU flags. */
/* Flags in the EFLAGS register. See the Intel documentation for more
   information about what those does. */

enum
{
  CPU_FLAG_CARRY = (BIT_VALUE (0)),
  CPU_FLAG_SET = (BIT_VALUE (1)),
  CPU_FLAG_PARITY = (BIT_VALUE (2)),
  CPU_FLAG_ADJUST = (BIT_VALUE (4)),
  CPU_FLAG_ZERO = (BIT_VALUE (6)),
  CPU_FLAG_SIGN = (BIT_VALUE (7)),
  CPU_FLAG_TRAP = (BIT_VALUE (8)),
  CPU_FLAG_INTERRUPT_ENABLE = (BIT_VALUE (9)),
  CPU_FLAG_DIRECTION = (BIT_VALUE (10)),
  CPU_FLAG_OVERFLOW = (BIT_VALUE (11)),
  CPU_FLAG_IOPL_LOW = (BIT_VALUE (12)),
  CPU_FLAG_IOPL_HIGH = (BIT_VALUE (13)),
  CPU_FLAG_NESTED_TASK = (BIT_VALUE (14)),
  CPU_FLAG_RESUME_TASK = (BIT_VALUE (16)),
  CPU_FLAG_V8086_MODE = (BIT_VALUE (17)),
  CPU_FLAG_ALIGNMENT_CHECK = (BIT_VALUE (18)),
  CPU_FLAG_VIRTUAL_INTERRUPT = (BIT_VALUE (19)),
  CPU_FLAG_VIRTUAL_INTERRUPT_PENDING = (BIT_VALUE (20)),
  CPU_FLAG_ID = (BIT_VALUE (21))
};

/* CR0 bits. */
/* Paging enabled. */

#define CPU_CR0_PG (BIT_VALUE (31))

/* Protected mode flag. */

#define CPU_CR0_PE (BIT_VALUE (0))

/* Extension type. */

#define CPU_CR0_ET (BIT_VALUE (4))

/* Write protect (486+). */

#define CPU_CR0_WP (BIT_VALUE (16))

/* CR4 bits. */
/* Enable global page directory entries. */

#define CPU_CR4_PGE (BIT_VALUE (7))
#define CPU_CR4_PSE (BIT_VALUE (4))

#endif /* !__STORM_IA32_CPU_H__ */
