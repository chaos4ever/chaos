/* $Id$ */
/* Abstract: AVL rotation functions. ONLY used by avl.c */
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

#ifndef __STORM_IA32_AVL_ROTATE_H__
#define __STORM_IA32_AVL_ROTATE_H__

/* Function declarations. */

extern avl_node_type *avl_rotate_right_single (avl_header_type *avl_header,
                                               avl_node_type *tree);

extern avl_node_type *avl_rotate_left_single (avl_header_type *avl_header,
                                              avl_node_type *tree);

extern avl_node_type *avl_rotate_right_double (avl_header_type *avl_header,
                                               avl_node_type *tree);

extern avl_node_type *avl_rotate_left_double (avl_header_type *avl_header,
                                              avl_node_type *tree);

#endif /* !__STORM_IA32_AVL_ROTATE_H__ */
