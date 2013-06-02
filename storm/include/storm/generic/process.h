// Abstract: Function prototypes and structures used by the process support.
// Author: Per Lundberg <per@halleluja.nu>

// Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/process.h>
#include <storm/current-arch/tss.h>
#include <storm/generic/types.h>
#include <storm/generic/memory_virtual.h>

// Type definitions
typedef struct process_info_type
{
  struct process_info_type *next;
  process_id_type process_id;

  // Process name.
  char *name;

  // A linked list over the threads in this process.
  tss_list_type *thread_list;
  unsigned int number_of_threads;
} process_info_type;

typedef struct
{
  storm_tss_type *tss;
  struct kernel_child_list_type *next;
} kernel_child_list_type;

// External variables.
extern storm_tss_type *kernel_tss;
extern process_id_type *process_id_array;
extern unsigned int number_of_processes;
extern process_info_type *process_list;

// Function prototypes
extern void process_init (void) INIT_CODE;
extern return_type process_create (process_create_type *process_data);
extern return_type process_name_set (char *name);
extern return_type process_parent_unblock(void);
extern process_info_type *process_find (process_id_type process_id);
extern void process_link (process_info_type *process_info);
