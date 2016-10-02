// Abstract: Function prototypes and structure definitions for CPU identification routines.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999, 2013 chaos development.

#pragma once

#include <storm/x86/defines.h>
#include <storm/x86/types.h>

typedef struct
{
    // Feature flags.
    uint32_t fpu:        1;
    uint32_t vme:        1;
    uint32_t de:         1;
    uint32_t pse:        1;
    uint32_t tsc:        1;
    uint32_t msr:        1;
    uint32_t pae:        1;
    uint32_t mce:        1;
    uint32_t cx8:        1;
    uint32_t apic:       1;
    uint32_t ff_res0:    1;
    uint32_t sep:        1;
    uint32_t mtrr:       1;
    uint32_t pge:        1;
    uint32_t mca:        1;
    uint32_t cmov:       1;
    uint32_t pat:        1;
    uint32_t pse_36:     1;
    uint32_t ff_res1:    5;
    uint32_t mmx:        1;
    uint32_t fxsr:       1;
    uint32_t sse:        1;
    uint32_t ff_res2:    5;
    uint32_t amd_3dnow:  1;
} PACKED cpuid_flags_type;

typedef struct
{
    uint32_t cpuid;

    const char *name;

    // Phony variable. Since C is so stupid it won't let me have union elements accessed the same way as the rest of the
    // structure, we have to hack it a little..
    uint32_t signature[0];

    // CPU signature.
    uint32_t stepping :  4;
    uint32_t model:      4;
    uint32_t family:     4;
    uint32_t type:       2;
    uint32_t s_res0:     18;

    union
    {
        uint32_t real_flags;
        cpuid_flags_type flags;
    } flags;

    // CPU configuration.
    uint32_t configuration;
} PACKED cpu_info_type;

typedef struct
{
    const char *name;
    const char *vendor;
    uint32_t speed;
} PACKED parsed_cpu_type;

typedef struct
{
    uint32_t vendor;
    uint32_t x86;
    const char *name[32];
} cpu_model_type;

extern cpu_info_type cpu_info;
extern parsed_cpu_type parsed_cpu;
extern unsigned int cpus;

extern void cpuid_init(void) INIT_CODE;

#define VENDOR_AMD_STRING    "AuthenticAMD"
#define VENDOR_INTEL_STRING  "GenuineIntel"
#define VENDOR_CYRIX_STRING  "CyrixInstead"

#define VENDOR_AMD           0
#define VENDOR_INTEL         1
#define VENDOR_CYRIX         2
#define VENDOR_UNKNOWN       0xFF
