// Abstract: Kernel logging functions.
// Author: Henrik Hallin <hal@chaosdev.org>

// © Copyright 2000, 2013 chaos development.

#pragma once

typedef struct
{
  char *string;
  struct log_list_type *next;
} log_list_type;

extern log_list_type *log_list;
