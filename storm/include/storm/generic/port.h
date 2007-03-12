/* $Id$ */
/* Abstract: Function prototypes and structure definitions of port in
   and output. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */

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
   USA */

#ifndef __STORM_GENERIC_PORT_H__
#define __STORM_GENERIC_PORT_H__

#include <storm/generic/defines.h>
#include <storm/generic/types.h>

/* Type definitions. */

typedef struct
{
  unsigned int start;
  unsigned int length;
  process_id_type process_id;
  cluster_id_type cluster_id;
  thread_id_type thread_id;
  struct port_range_type *next;
  struct port_range_type *previous;
  char *description;
} port_range_type;

/* Function prototypes. */

extern void port_init (void) INIT_CODE;
extern return_type port_range_register (unsigned int start, unsigned int ports,
                                        char *description);
extern return_type port_range_unregister (unsigned int start);
extern void port_range_free_all (thread_id_type thread_id);
extern bool port_range_link (unsigned int start, unsigned int ports, 
                             char *description);

/* External variables. */

extern port_range_type *port_list;

/* Now, the architecture specifics. */

#include <storm/current-arch/port.h>

#endif /* !__STORM_GENERIC_PORT_H__ */
