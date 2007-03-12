/* $Id$ */
/* Abstract: Debug functions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#ifndef __STORM_GENERIC_DEBUG_H__
#define __STORM_GENERIC_DEBUG_H__

#include <storm/generic/thread.h>
#include <storm/current-arch/debug.h>

#if !OPTION_RELEASE

#include <storm/generic/types.h>
#include <storm/generic/defines.h>

/* Function prototypes. */

extern void debug_init (void) INIT_CODE;
extern void debug_print (const char *string, ...);
extern void debug_crash_screen (const char *message, thread_type *thread);

/* Debug macros. */

#define DEBUG_MESSAGE(debug, message...) \
  if (debug) \
  { \
    debug_print ("%s: ", __FUNCTION__); \
    debug_print (## message); \
    debug_print ("\n"); \
  }

#define DEBUG_HALT(message...) \
  cpu_interrupts_disable (); \
  debug_print ("[KERNEL BUG] %s: ", __FUNCTION__); \
  debug_print (## message); \
  while (TRUE);

#else /* OPTION_RELEASE */

#define DEBUG_MESSAGE(debug, message...)
#define DEBUG_HALT(message...) cpu_halt ();

#define debug_print(format_string...)
#define debug_init()
#define debug_crash_screen(message,thread)

#endif /* !OPTION_RELEASE */

#endif /* !__STORM_GENERIC_DEBUG_H__ */
