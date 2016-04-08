// Abstract: Function prototypes.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <storm/defines.h>

C_EXTERN_BEGIN

extern return_type system_cluster_name_set(const char *name);
extern return_type system_execute(const char *program);
extern void system_exit(void) NORETURN;
extern return_type system_process_name_set(const char *name);
extern void system_shutdown(void) NORETURN;
extern return_type system_sleep(unsigned int time);
extern return_type system_sleep_microseconds(unsigned int time);
extern return_type system_thread_create(thread_entry_point_type *thread_entry_point, void *argument);
extern return_type system_thread_name_set(const char *name);

C_EXTERN_END
