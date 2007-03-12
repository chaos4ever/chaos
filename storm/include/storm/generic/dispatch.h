/* $Id$ */
/* Copyright 1999-2000 chaos development. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Description: Function prototype definitions. */

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

#ifndef __STORM_GENERIC_DISPATCH_H__
#define __STORM_GENERIC_DISPATCH_H__

#include <storm/current-arch/tss.h>
#include <storm/generic/types.h>

/* Prototypes. */

extern void dispatch_next (void);
extern int dispatch_update (void);
extern void dispatch (void);
extern void dispatch_task_switcher (void) __attribute__ ((noreturn));
extern void dispatch_init (void);

/* External variables. */

extern volatile process_id_type current_process_id;
extern volatile cluster_id_type current_cluster_id;
extern volatile thread_id_type current_thread_id;
extern volatile time_type timeslice;
extern storm_tss_type *current_tss;
extern volatile u8 dispatch_task_flag;
extern tss_list_type *current_tss_node;

#endif /* !__STORM_GENERIC_DISPATCH_H__ */
