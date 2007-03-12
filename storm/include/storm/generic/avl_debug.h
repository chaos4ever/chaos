/* $Id$ */
/* Abstract: */
/* Author: Henrik Hallin <hal@chaosdev.org> */

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
   USA. */

#ifndef __STORM_GENERIC_AVL_DEBUG_H__
#define __STORM_GENERIC_AVL_DEBUG_H__

#include <storm/generic/avl.h>

#define MAX_LOG_SIZE 10
#define MAX_LOG_MESSAGE_LENGTH 70

void avl_debug_tree_dump (avl_node_type *tree);
void avl_debug_tree_check (avl_header_type *avl_header, avl_node_type *tree);
void avl_debug_log_write (char *);
void avl_debug_log_dump (void);

#endif /* !__STORM_GENERIC_AVL_DEBUG_H__ */
