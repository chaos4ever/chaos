/* $Id$ */
/* Abstract: Debug functions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __STORM_GENERIC_DEBUG_H__
#define __STORM_GENERIC_DEBUG_H__

#include <storm/generic/dispatch.h>
#include <storm/generic/irq.h>
#include <storm/generic/defines.h>
#include <storm/generic/process.h>
#include <storm/generic/types.h>
#include <storm/current-arch/debug.h>
#include <config.h>

#if !OPTION_RELEASE

/* Defines. */

#define DEBUG_ATTRIBUTE_TEXT            0x07
#define DEBUG_ATTRIBUTE_BACKGROUND      0x07
#define DEBUG_ATTRIBUTE_CRASH           0x4F
#define DEBUG_MAX_INPUT                 100
#define DEBUG_KEYBOARD_DATA             0x60
#define DEBUG_KEYBOARD_STATUS           0x64

#define DEBUG_KEYBOARD_INPUT_FULL       0x01

#define DEBUG_SCAN_CODE_LEFT_SHIFT      42
#define DEBUG_SCAN_CODE_RIGHT_SHIFT     54
#define DEBUG_SCAN_CODE_ALT             56
#define DEBUG_SCAN_CODE_ENTER           28
#define DEBUG_SCAN_CODE_BACK_SPACE      14

#define DEBUG_SHIFT                     (1 << 0)
#define DEBUG_ALT                       (1 << 1)

/* Typedefs. */

typedef struct
{ 
  u8 character;
  u8 attribute;
} __attribute__ ((packed)) debug_screen_type;

typedef struct
{
  int number_of_arguments;
  char **argument_start;
  char *arguments;
} debug_arguments_type;

/* Type definitions. */

typedef struct
{
  char name[64];
  char arguments[256];
  char description[70];
  void *pointer;
} debug_command_type;

/* External variables. */

extern u32 debug_text_attribute;
extern u32 debug_background_attribute;
extern bool debug_log_enable;

/* Function prototypes. */

extern void debug_init (void) INIT_CODE;
extern void debug_memdump (u32 *memory, int length);
extern void debug_print (const char *string, ...);
extern void debug_print_simple (const char *string);
extern void debug_crash_screen (const char *message,
                                volatile storm_tss_type *dump_tss);
extern void debug_run (void);

/* Debug macros. */
/* FIXME: Don't do irq_disable here!!! */

#define DEBUG_SDB(debug, message...) \
  if (debug) \
  { \
    cpu_interrupts_disable (); \
    debug_print ("%s: ", __FUNCTION__); \
    debug_print (#message); \
    debug_print (" (%s:%u, process = %s (%u), thread = %s (%u)\n\n" \
                 "Going into kernel debugger.\n\n", __FILE__, __LINE__, \
    ((process_info_type *) current_tss->process_info)->name, \
    current_tss->process_id, current_tss->thread_name, \
    current_tss->thread_id); \
    debug_run (); \
  }

#define DEBUG_MESSAGE(debug, message...) \
  if (debug) \
  { \
    irq_disable (0); \
    debug_print ("%s: ", __FUNCTION__); \
    debug_print (#message); \
    debug_print (" (%s:%u, process = %s (%u), thread = %s (%u)\n", \
    __FILE__, __LINE__, \
    ((process_info_type *) current_tss->process_info)->name, \
    current_tss->process_id, current_tss->thread_name, \
    current_tss->thread_id); \
    irq_enable (0); \
  }

#define DEBUG_HALT(message...) \
  cpu_interrupts_disable (); \
  debug_print ("[KERNEL BUG] %s: ", __FUNCTION__); \
  debug_print (#message); \
  debug_print (" (%s:%u, process = %s (%u), thread = %s (%u)\n\n" \
               "Going into kernel debugger.\n\n", __FILE__, __LINE__, \
  ((process_info_type *) current_tss->process_info)->name, \
  current_tss->process_id, current_tss->thread_name, \
  current_tss->thread_id); \
  debug_run (); \
  while (TRUE);

#else /* OPTION_RELEASE */

#define DEBUG_MESSAGE(debug, message...)
#define DEBUG_SDB(debug, message...)
#define DEBUG_HALT(message...) cpu_halt ();

#define debug_print(format_string...)
#define debug_run()

#define debug_crash_screen(message, dump_tss) \
  cpu_halt ();

#define debug_init()

extern void debug_print_simple (char *string);

#endif /* !OPTION_RELEASE */

#endif /* !__STORM_GENERIC_DEBUG_H__ */
