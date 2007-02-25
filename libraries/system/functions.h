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

#ifndef __LIBRARY_SYSTEM_FUNCTIONS_H__
#define __LIBRARY_SYSTEM_FUNCTIONS_H__

extern return_type system_cluster_name_set (char *name);
extern return_type system_execute (char *program);
extern return_type system_exit (void) __attribute__ ((noreturn));
extern return_type system_process_name_set (char *name);
extern return_type system_shutdown (void) __attribute__ ((noreturn));
extern return_type system_sleep (unsigned int time);
extern return_type system_sleep_microseconds (unsigned int time);
extern return_type system_thread_create (void);
extern return_type system_thread_name_set (char *name);

#endif /* !__LIBRARY_SYSTEM_FUNCTIONS_H__ */
