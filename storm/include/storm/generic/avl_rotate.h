// Abstract: AVL rotation functions. ONLY used by avl.c
// Author: Henrik Hallin <hal@chaosdev.org>

// © Copyright 2000, 2013 chaos development.

#pragma once

extern avl_node_type *avl_rotate_right_single (avl_header_type *avl_header, avl_node_type *tree);
extern avl_node_type *avl_rotate_left_single (avl_header_type *avl_header, avl_node_type *tree);
extern avl_node_type *avl_rotate_right_double (avl_header_type *avl_header, avl_node_type *tree);
extern avl_node_type *avl_rotate_left_double (avl_header_type *avl_header, avl_node_type *tree);
