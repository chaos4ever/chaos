// Abstract: CPU identification. Parts borrowed from the Linux kernel.
// Author: Per Lundberg <per@chaosdev.io> with some parts borrowed from Linux kernel.

// © Copyright 1999 chaos development

// TODO: Get rid of the Linux kernel legacy here so we can relicense the file under the BSD license instead.

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.

#include <storm/x86/timer.h>
#include <storm/x86/types.h>
#include <storm/x86/defines.h>
#include <storm/x86/cpuid.h>
#include <storm/x86/cpu.h>
#include <storm/x86/flags.h>
#include <storm/x86/memory.h>
#include <storm/x86/port.h>
#include <storm/x86/string.h>

#define DEBUG FALSE

/* CPUID functions. */

enum
{
  GET_CPU_VENDOR,
  GET_CPU_INFO,
};

static inline void rdtsc (uint32_t *low, uint32_t *high)
{
  asm volatile
  ("rdtsc"
   : "=a" (*low), "=d" (*high));
}

cpu_info_type cpu_info;
parsed_cpu_type parsed_cpu;
static const char unknown[] = "Unknown";

/* The number of CPUs in the system. When we write SMP support, this
   variable might be something else. */

unsigned int cpus = 1;

/* FIXME: Once the CPU type is determined, we can safely copy the name
   and deallocate this structure. */
/* FIXME: This list is totally irrelevant for most modern CPUs that
   fully implement CPUID detection... We can get the name of the CPU
   using CPUID instead and only store the family, model and flags (for
   use by code that e.g. relies on MMX being present etc). */

static cpu_model_type cpu_model[] =
{
  {
    VENDOR_INTEL, 4,
    {
      "486 DX-25/33", "486 DX-50", "486 SX", "486 DX/2", "486 SL", "486 SX/2",
      unknown, "486 DX/2-WB", "486 DX/4", "486 DX/4-WB", unknown, unknown,
      unknown, unknown, unknown, unknown
    }
  },
  {
    VENDOR_INTEL, 5,
    {
      "Pentium A-step", "Pentium", "Pentium", "OverDrive PODP5V83",
      "Pentium MMX", unknown, unknown, "Mobile Pentium", "Mobile Pentium MMX",
      unknown, unknown, unknown, unknown, unknown, unknown, unknown
    }
  },
  {
    VENDOR_INTEL, 6,
    {
      "Pentium Pro A-step", "Pentium Pro", unknown, "Pentium II (Klamath)",
      unknown, "Pentium II (Deschutes)", "Celeron (Mendocino)",
      "Pentium III (McKinley)", "Pentium III (Coppermine)", unknown,
      unknown, unknown, unknown, unknown, unknown, unknown
    }
  },
  {
    VENDOR_INTEL, 15,
    {
      "Core Duo", "Core 2", unknown, unknown, unknown, unknown, unknown,
      unknown, unknown, unknown, unknown, unknown, unknown, unknown,
      unknown, unknown, unknown, unknown, unknown, unknown, unknown,
      unknown, unknown, unknown, unknown, unknown, unknown, unknown,
      unknown, unknown, unknown, unknown
    }
  },
  {
    VENDOR_AMD, 4,
    {
      unknown, unknown, unknown, "486 DX/2", unknown, unknown, unknown,
      "486 DX/2-WB", "486 DX/4", "486 DX/4-WB", unknown, unknown, unknown,
      unknown, "Am5x86-WT", "Am5x86-WB"
    }
  },
  {
    VENDOR_AMD, 5,
    {
      "K5/SSA5", "K5", "K5", "K5", unknown, unknown, "K6", "K6", "K6-2",
      "K6-3", unknown, unknown, unknown, unknown, unknown, unknown
    }
  },
  {
    VENDOR_AMD, 6,
    {
      "Athlon", "Athlon", unknown, unknown, unknown, unknown, unknown,
      unknown, unknown, unknown, unknown, unknown, unknown, unknown,
      unknown, unknown
    }
  }
};

static const char *cpu_name_nocpuid = "noname" ;
static char cpuid_string[13];

/* This function is called if there seems to be an overflow in the
   matrix. */
static void INIT_CODE corrupted_struct (void) __attribute__ ((noreturn));
static void INIT_CODE corrupted_struct (void)
{
  DEBUG_HALT ("CPUID struct corrupted.");
}

/* Generic CPUID function */

static inline void cpuid (uint32_t command, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
  asm volatile ("cpuid"
       : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
       : "a" (command)
       : "cc");
}

/* Check if the CPUID instruction is available on this system.  This
   is distinguished by checking if FLAG_ID in eflags can be toggled. If
   that's true, the CPU has the CPUID instruction. See Intel's
   documentation for more information about how the CPUID instruction
   works. */

static int INIT_CODE has_cpuid (void)
{
  int return_value;

  asm volatile ("pushl       %%ebx\n"

                /* Invert id bit. */
                "pushfl\n"
                "xorl        %1, (%%esp)\n"
                "popfl\n"

                /* Read eflags register and mask all bits but id bit. */

                "pushfl\n"
                "movl        (%%esp), %%eax\n"
                "andl        %1, %%eax\n"
                "popfl\n"

                /* Invert id bit. */

                "pushfl\n"
                "xorl        %1, (%%esp)\n"
                "popfl\n"

                /* Mask. */

                "pushfl\n"
                "movl        (%%esp), %%ebx\n"
                "andl        %1, %%ebx\n"
                "popfl\n"

                /* Is id bit the same? */

                "cmpl        %%ebx, %%eax\n"
                "jne         1f\n"

                /* CPUID not supported. */

                "movl        $0, %%eax\n"
                "jmp         2f\n"
                "1: movl        $1, %%eax\n"
                "2: popl        %%ebx"
                : "=&a" (return_value)
                : "g" (FLAG_ID));

  return return_value;
}

/* Check if the current CPU is a 486. Returns 1 if 486 and zero if
   386. */

static int INIT_CODE is_486 (void)
{
  int return_value;

  asm
  ("pushl   %%ecx\n"

   "pushfl\n"
   "popl    %%eax\n"
   "movl    %%eax, %%ecx\n"
   "xorl    $0x40000, %%eax\n"
   "pushl   %%eax\n"
   "popf\n"

   "pushf\n"
   "popl    %%eax\n"
   "xorl    %%ecx, %%eax\n"
   "and     $0x40000, %%eax\n"
   "je      1f\n"

   /* 486 was detected. */

   "movl   $1, %%eax\n"
   "jmp    2f\n"

   "1:  movl   $0, %%eax\n"
   "2:  popl   %%ecx"
   : "=&a" (return_value));

  return return_value;
}

/* Examine the CPU type. */

static void INIT_CODE cpu_examine (void)
{
  /* If CPUID is available, use it. */

  if (has_cpuid ())
  {
    uint32_t dummy;

    DEBUG_MESSAGE (DEBUG, "Passed");

    cpu_info.cpuid = 1;

    /* Get CPU vendor name. */

    cpuid (GET_CPU_VENDOR, &dummy, (uint32_t *) &cpuid_string[0],
           (uint32_t *) &cpuid_string[8], (uint32_t *) &cpuid_string[4]);

    cpu_info.name = cpuid_string;

    /* Get model type and flags. */

    cpuid (GET_CPU_INFO, (uint32_t *) &cpu_info.signature, &dummy, &dummy,
           (uint32_t *) &cpu_info.flags.real_flags);
  }
  else
  {
    cpu_info.name = cpu_name_nocpuid;
    cpu_info.cpuid = 0;

    /* Now detect if the CPU is a 386 or 486. */

    if (is_486 ())
    {
      cpu_info.family = 4;
      DEBUG_MESSAGE (DEBUG, "Passed");
    }
    else
    {
      cpu_info.family = 3;
    }
  }
}

/* FIXME: This code should be much cleaner... */

/* Calibrate the TSC. */
/* Return 2^32 * (1 / (TSC clocks per usec)) for CPU HZ detection.
   Too much 64-bit arithmetic here to do this cleanly in C, and for
   accuracy's sake we want to keep the overhead on the CTC speaker
   (channel 2) output busy loop as low as possible. We avoid reading
   the CTC registers directly because of the awkward 8-bit access
   mechanism of the 82C54 device. */

/* For divider */

#define LATCH           ((CLOCK_TICK_RATE + hz / 2) / hz)
#define CALIBRATE_LATCH (5 * LATCH)
#define CALIBRATE_TIME  (5 * 1000020 / hz)

static uint32_t INIT_CODE calibrate_tsc (void)
{
  uint32_t startlow, starthigh;
  uint32_t endlow, endhigh;
  uint32_t count;

  /* Set the gate high, disable speaker. */

  port_out_uint8_t (0x61, (port_in_uint8_t (0x61) & ~0x02) | 0x01);

  /* Now let's take care of CTC channel 2. */

  /* Set the gate high, program CTC channel 2 for mode 0, (interrupt
     on terminal count mode), binary count, load 5 * LATCH count, (LSB
     and MSB) to begin countdown. */

  /* Binary, mode 0, LSB/MSB, Ch 2 */

  port_out_uint8_t (0x43, 0xB0);

  /* LSB of count */

  port_out_uint8_t (0x42, CALIBRATE_LATCH & 0xFF);

  /* MSB of count */

  port_out_uint8_t (0x42, CALIBRATE_LATCH >> 8);

  rdtsc (&startlow, &starthigh);
  count = 0;

  do
  {
    count++;
  } while ((port_in_uint8_t (0x61) & 0x20) == 0);

  rdtsc (&endlow, &endhigh);

  /* Error */

  if (count <= 1)
  {
    return 0;
  }

  /* 64-bit subtract - gcc just messes up with long longs. FIXME: test
     if this is really so. The code was borrowed from Linux which was
     designed for gcc 2.7, and we're targeting gcc 2.95... */

  asm ("subl %2, %0\n"
       "sbbl %3, %1"
       : "=a" (endlow), "=d" (endhigh)
       : "g" (startlow), "g" (starthigh), "0" (endlow), "1" (endhigh));

  /* CPU is too fast. */

  if (endhigh != 0)
  {
    return 0;
  }

  /* CPU is too slow. */

  if (endlow <= CALIBRATE_TIME)
  {
    return 0;
  }

  asm ("divl %2"
       : "=a" (endlow), "=d" (endhigh)
       : "r" (endlow), "0" (0), "1" (CALIBRATE_TIME));

  return endlow;
}

/* Get the CPU speed in MHz. This requires TSC support. */

static uint32_t INIT_CODE cpuid_get_cpu_speed (void)
{
  uint32_t tsc_quotient = calibrate_tsc ();
  uint32_t cpu_hz;

  if (tsc_quotient != 0)
  {
    /* Report CPU clock rate in Hz. The formula is (10^6 * 2^32) /
       (2^32 * 1 / (clocks/us)) = clock/second. Our precision is about
       100 ppm. */

    {
      uint32_t eax = 0, edx = 1000000;
      asm ("divl %2"
           : "=a" (cpu_hz), "=d" (edx)
           : "r" (tsc_quotient), "0" (eax), "1" (edx));
    }
  }
  else
  {
    return 0;
  }
  return cpu_hz;
}

/* Initialise the CPU detection functionality. */

void cpuid_init (void)
{
  uint8_t vendor;
  uint8_t c;

  /* Make sure we clear this structure. */

  memory_set_uint8_t ((uint8_t *) &cpu_info, 0, sizeof (cpu_info_type));

  DEBUG_MESSAGE (DEBUG, "Passed");

  cpu_examine ();

  DEBUG_MESSAGE (DEBUG, "Passed");

  /* If we don't have MMX and are compiled for using it, halt. */

#ifdef MMX
  if (!cpu_info.cpuid || !cpu_info.flags.flags.mmx)
  {
    DEBUG_HALT ("No MMX detected. This kernel requires it!");
  }
#endif

  /* If we have TSC support, we can do an accurate CPU Hz
     detection. */

  if ((cpu_info.cpuid & cpu_info.flags.flags.tsc) != 0)
  {
    DEBUG_MESSAGE (DEBUG, "Passed");
    parsed_cpu.speed = cpuid_get_cpu_speed ();
  }

  switch (cpu_info.cpuid)
  {
    /* Non-CPUID capable machine (386, early 486). */

    case 0:
    {
      DEBUG_MESSAGE (DEBUG, "Passed");

      switch (cpu_info.family)
      {
        /* 386 */

        case 3:
        {
          parsed_cpu.name = "Unidentified 386";
          parsed_cpu.vendor = "Unknown";
          break;
        }

        /* 486 */

        case 4:
        {
          parsed_cpu.name = "Unidentified 486";
          parsed_cpu.vendor = "Unknown";
          break;
        }

        /* Error */

        default:
        {
          corrupted_struct ();
          break;
        };
        break;
      }

      break;
    }

    /* CPUID capable machine. (late 486, Pentium, K6, Athlon and so
       on...) */

    case 1:
    {
      DEBUG_MESSAGE (DEBUG, "Passed");

      if (!string_compare (cpu_info.name, VENDOR_AMD_STRING))
      {
        vendor = VENDOR_AMD;
        parsed_cpu.vendor = "AMD";
      }
      else if (!string_compare (cpu_info.name, VENDOR_INTEL_STRING))
      {
        vendor = VENDOR_INTEL;
        parsed_cpu.vendor = "Intel";
      }
      else if (!string_compare (cpu_info.name, VENDOR_CYRIX_STRING))
      {
        vendor = VENDOR_CYRIX;
        parsed_cpu.vendor = "Cyrix";
      }
      else
      {
        vendor = VENDOR_UNKNOWN;
        parsed_cpu.name = "Unknown clone manufacturer";
        parsed_cpu.vendor = "Unknown";
      }

      DEBUG_MESSAGE (DEBUG, "Passed");

      if (vendor != VENDOR_UNKNOWN) {

        /* Find the model name for this CPU. */

        for (c = 0; c < sizeof(cpu_model) / sizeof(cpu_model_type); c++)
        {
          if (cpu_model[c].vendor == vendor &&
              cpu_model[c].x86 == cpu_info.family)
          {
            parsed_cpu.name = cpu_model[c].name[cpu_info.model];
            break;
          }
        }
      }

      DEBUG_MESSAGE (DEBUG, "Passed");

      break;
    }

    default:
    {
      corrupted_struct ();
      break;
    }
  }
}
