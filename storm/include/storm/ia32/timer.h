/* $Id$ */
/* Abstract: Function prototypes and structure definitions of the
   timer support. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
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
   USA */

#ifndef __STORM_IA32_TIMER_H__
#define __STORM_IA32_TIMER_H__

#include <storm/generic/limits.h>
#include <storm/generic/time.h>
#include <storm/ia32/tss.h>

/* Function prototypes. */

extern void timer_init (void) INIT_CODE;
extern void timer_add_event
  (time_type milliseconds, u32 action, storm_tss_type *tss);
extern void timer_check_events (void);

/* Typedefs. */

typedef struct
{
  struct timer_event_type *previous;
  struct timer_event_type *next;
  time_type timeslice;
  u32 action;
  storm_tss_type *tss;
} __attribute__ ((packed)) timer_event_type;


/* Defines/enums. */
/* Events. */

enum
{
  TIMER_EVENT_THREAD_WAKEUP
};

/* Port numbers. */

#define PIT_BASE                0x40
#define PIT_CHANNEL_0           (PIT_BASE + 0)
#define PIT_COUNTER_DIVISOR     (PIT_BASE + 0)
#define PIT_COUNTER_1           (PIT_BASE + 1)
#define PIT_RAM_REFRESH_COUNTER (PIT_BASE + 1)
#define PIT_COUNTER_2           (PIT_BASE + 2)
#define PIT_MODE_PORT           (PIT_BASE + 3)

/* Bit fields in the mode control word. */

#define COUNTER_0_SELECT        (0)
#define COUNTER_1_SELECT        (BIT_VALUE (6))
#define COUNTER_2_SELECT        (BIT_VALUE (7))
#define READ_BACK_COUNTER       (BIT_VALUE (7) | BIT_VALUE (6))

/* The following are used when READ_BACK_COUNTER is clear. */

#define COUNTER_LATCH_COMMAND   (0)
#define ACCESS_LOW_COUNTER_U8   (BIT_VALUE (4))
#define ACCESS_HIGH_COUNTER_U8  (BIT_VALUE (5))

/* Zero detection interrupt. */

#define MODE_0_SELECT           (0)

/* Programmable one shot. */

#define MODE_1_SELECT           (BIT_VALUE (1))

/* Rate generator. */

#define MODE_2_SELECT           (BIT_VALUE (2))

/* Square wave generator. */

#define MODE_3_SELECT           (BIT_VALUE (2) | BIT_VALUE (1))

/* Software triggered strobe. */

#define MODE_4_SELECT           (BIT_VALUE (3))

/* Hardware triggered strobe. */

#define MODE_5_SELECT           (BIT_VALUE (3) | BIT_VALUE (1))

/* 16-bit binary counter. */

#define BINARY_COUNTER          (0)

/* 4 decade BCD counter. */
 
#define BCD_COUNTER             (BIT_VALUE (0))

/* The following are used when READ_BACK_COUNTER is set. */

#define READ_COUNTER_VALUE      (BIT_VALUE (5))
#define READ_COUNTER_STATUS     (BIT_VALUE (4))
#define SELECT_COUNTER_2        (BIT_VALUE (3))
#define SELECT_COUNTER_1        (BIT_VALUE (2))
#define SELECT_COUNTER_0        (BIT_VALUE (1))

/* The clock speed of the timer chip. */

#define CLOCK_TICK_RATE         1193180

/* Used for setting counter divisors. The 8254 chip has a fixed
   frequency of 1193180 Hz, one fourth of the original 8088 speed. So,
   when we want a given speed, we divide this frequency with the
   desired frequency. */

#define COUNTER_DIVISOR(hz)     (CLOCK_TICK_RATE / hz)

#endif /* !__STORM_IA32_TIMER_H__ */
