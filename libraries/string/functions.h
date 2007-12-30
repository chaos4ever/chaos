/* $Id$ */
/* Abstract: Function prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __LIBRARY_STRING_FUNCTIONS_H__
#define __LIBRARY_STRING_FUNCTIONS_H__

#include <stdarg.h>
#include <system/system.h>

extern return_type string_to_number (const char *string, int *number,
                                     unsigned int *characters);

extern return_type string_print (char *output, const char *format_string, ...)
  __attribute__ ((format (printf, 2, 3)));
extern return_type string_print_va (char *output, const char *format_string,
                                    va_list arguments);

#endif /* !defined __LIBRARY_STRING_FUNCTIONS_H__ */
