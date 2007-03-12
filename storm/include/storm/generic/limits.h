/* $Id$ */
/* Abstract: Limitations on the kernel level. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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
   USA. */

#ifndef __STORM_GENERIC_LIMITS_H__
#define __STORM_GENERIC_LIMITS_H__

#include <storm/types.h>
#include <storm/limits.h>

/* Some system limits. */

#define MAX_EVENTS       10

/* External variables. */

extern limit_type limit_global_nodes;
extern limit_type limit_global_heap;
extern limit_type limit_memory;
extern limit_type limit_mailbox_hash_entries;
extern limit_type limit_thread_hash_entries;
extern limit_type limit_process_hash_entries;

#endif /* !__STORM_GENERIC_LIMITS_H__ */
