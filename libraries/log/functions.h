// Abstract: Function prototypes for the log library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development.

#pragma once

#include <ipc/ipc.h>
#include <system/system.h>

extern return_type log_print_formatted
    (log_structure_type *log_structure, unsigned int urgency,
     const char *format_string, ...) __attribute__ ((format (printf, 3, 4)));

extern return_type log_print
    (log_structure_type *log_structure, unsigned int urgency,
     const char *message);

extern return_type log_init
    (log_structure_type *log_structure, char *class, tag_type *tag);
