// Abstract: AVL debugging helpers.
// Author: Henrik Hallin <hal@chaosdev.org>

// © Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/generic/avl.h>

#define MAX_LOG_SIZE 10
#define MAX_LOG_MESSAGE_LENGTH 70

void avl_debug_tree_dump (avl_node_type *tree);
void avl_debug_tree_check (avl_header_type *avl_header, avl_node_type *tree);
void avl_debug_log_write (char *);
void avl_debug_log_dump (void);
