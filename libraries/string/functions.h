/* $Id$ */
/* Abstract: Function prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_STRING_FUNCTIONS_H__
#define __LIBRARY_STRING_FUNCTIONS_H__

#include <stdarg.h>
#include <system/system.h>

extern return_type string_to_number (const char *string, int *number,
                                     int *characters);

extern return_type string_print (char *output, const char *format_string, ...)
  __attribute__ ((format (printf, 2, 3)));
extern return_type string_print_va (char *output, const char *format_string,
                                    va_list arguments);

#endif /* !__LIBRARY_STRING_FUNCTIONS_H__ */
