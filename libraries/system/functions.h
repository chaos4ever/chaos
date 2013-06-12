// Abstract: Function prototypes.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2007, 2013 chaos development.

#pragma once

#include <storm/defines.h>

extern return_type system_cluster_name_set(const char *name);
extern return_type system_execute(const char *program);
extern void system_exit(void) NORETURN;
extern return_type system_process_name_set(const char *name);
extern void system_shutdown(void) NORETURN;
extern return_type system_sleep(unsigned int time);
extern return_type system_sleep_microseconds(unsigned int time);
extern return_type system_thread_create(void);
extern return_type system_thread_name_set(const char *name);
