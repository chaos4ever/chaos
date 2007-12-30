/* $Id$ */
/* Abstract: Function prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __LIBRARY_SYSTEM_FUNCTIONS_H__
#define __LIBRARY_SYSTEM_FUNCTIONS_H__

extern return_type system_cluster_name_set (char *name);
extern return_type system_execute (char *program);
extern void system_exit (void) __attribute__ ((noreturn));
extern return_type system_process_name_set (char *name);
extern void system_shutdown (void) __attribute__ ((noreturn));
extern return_type system_sleep (unsigned int time);
extern return_type system_sleep_microseconds (unsigned int time);
extern return_type system_thread_create (void);
extern return_type system_thread_name_set (char *name);

#endif /* !defined __LIBRARY_SYSTEM_FUNCTIONS_H__ */
