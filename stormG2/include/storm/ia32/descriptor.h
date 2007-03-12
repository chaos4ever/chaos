/* $Id$ */
/* Abstract: Descriptor definitions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#ifndef __STORM_IA32_DESCRIPTOR_H__
#define __STORM_IA32_DESCRIPTOR_H__

#include <storm/types.h>

enum
{
  DESCRIPTOR_TYPE_TASK_GATE_32 = 5,
  DESCRIPTOR_TYPE_TSS = 9,
  DESCRIPTOR_TYPE_CALL_GATE = 12,
  DESCRIPTOR_TYPE_INTERRUPT_GATE_32 = 14,
  DESCRIPTOR_TYPE_TRAP_GATE_32 = 15,
};

typedef struct
{
  u16 limit_lo;                 /* Low 16 bits of segment limit. */
  u16 base_lo;                  /* Low 16 bits of base address. */
  u8 base_hi;                   /* Bits 16-23 of base address. */
  u8 type : 4;                  /* Segment type. */
  u8 descriptor_type : 1;       /* 0 = system, 1 = code or data. */
  u8 dpl : 2;                   /* Descriptor privilege level. */
  u8 segment_present : 1;       /* Zero if segment isn't used. */
  u8 limit_hi : 4;              /* High four bits of segment limit. */
  u8 unused : 1;                /* Intel is being 'smart' as
                                   usual.. */
  u8 zero : 1;                  /* Always zero! */
  u8 operation_size : 1;        /* 0 = 16-bit, 1 = 32-bit. */
  u8 granularity : 1;           /* If 1, segment limit is multiplied
                                   by 4096. */
  u8 base_hi2;                  /* Bits 24-31 of base address. */
} __attribute__ ((packed)) descriptor_type;

typedef struct
{
  u16 offset_lo;                /* Low 16 bits of offset. */
  u16 segment_selector;
  u8 params : 5;                /* Zero if not type == 12. */
  u8 zero : 3;                  /* Zero */
  u8 type : 5;                 
  u8 dpl : 2;                   /* Descriptor privilege level. */
  u8 present : 1;               /* If zero, this descriptor isn't
                                   used. */
  u16 offset_hi;                /* High 16 bits of offset. */
} __attribute__ ((packed)) gate_descriptor_type;

#endif /* !__STORM_IA32_DESCRIPTOR_H__ */
