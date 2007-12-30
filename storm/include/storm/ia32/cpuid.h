/* $Id$ */
/* Abstract: Function prototypes and structure definitions for CPU
   identification routines. */

/* Copyright 1999 chaos development. */

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
   USA */

#ifndef __STORM_IA32_CPUID_H__
#define __STORM_IA32_CPUID_H__

#include <storm/ia32/defines.h>
#include <storm/ia32/types.h>

typedef struct
{
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
} __attribute__ ((packed)) cpuid_flags_type;

typedef struct
{
  u32 cpuid;
  
  const char *name;
  
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

  union
  {
    u32 real_flags;
    cpuid_flags_type flags;
  } flags;
 
  /* CPU configuration. */
  
  u32 configuration;
} __attribute__ ((packed)) cpu_info_type;

typedef struct
{
  const char *name;
  const char *vendor;
  u32 speed;
} __attribute__ ((packed)) parsed_cpu_type;

typedef struct
{
  u32 vendor;
  u32 x86;
  const char *name[32];
} cpu_model_type;

/* External variables. */

extern cpu_info_type cpu_info;
extern parsed_cpu_type parsed_cpu;
extern unsigned int cpus;
 
/* External functions. */

extern void cpuid_init (void) INIT_CODE;

/* Vendor strings. */

#define VENDOR_AMD_STRING    "AuthenticAMD"
#define VENDOR_INTEL_STRING  "GenuineIntel"
#define VENDOR_CYRIX_STRING  "CyrixInstead"

#define VENDOR_AMD 0
#define VENDOR_INTEL 1
#define VENDOR_CYRIX 2
#define VENDOR_UNKNOWN 0xFF

#endif /* !__STORM_IA32_CPUID_H__ */
