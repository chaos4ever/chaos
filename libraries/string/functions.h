// Abstract: Function prototypes.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2017 chaos development

#pragma once

#include <stdarg.h>
#include <system/system.h>

extern return_type string_to_number(const char *string, int *number,
                                    unsigned int *characters);

extern return_type string_print(char *output, const char *format_string, ...)
__attribute__((format(printf, 2, 3)));
extern return_type string_print_va(char *output, const char *format_string,
                                   va_list arguments);
