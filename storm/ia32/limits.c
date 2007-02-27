/* $Id$ */
/* Abstract: Limitation variables. All of those should be overridable
   via kernel parameters or otherwise. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#include <storm/generic/defines.h>
#include <storm/generic/types.h>

/* System limitations. Can be overridden by kernel parameters. This
   should be a multiple of four megabytes (one page directory
   entry). */

limit_type limit_global_heap = 128 * MB;

/* This limits the maximum amount of nodes in the tree over globally
   allocated memory.*/

limit_type limit_global_nodes = 5000000;

/* This overrides the detected memory size. Can be used if the
   motherboard reports the wrong size. */

limit_type limit_memory = 0;

/* Number of entries in the mailbox hash table. */

limit_type limit_mailbox_hash_entries = 1024;

/* Number of entries in the thread hash table. */

limit_type limit_thread_hash_entries = 1024;

