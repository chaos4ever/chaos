// Abstract: Limitations on the kernel level.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/types.h>
#include <storm/limits.h>

#define MAX_EVENTS       10

extern limit_type limit_global_nodes;
extern limit_type limit_global_heap;
extern limit_type limit_memory;
extern limit_type limit_mailbox_hash_entries;
extern limit_type limit_thread_hash_entries;
extern limit_type limit_process_hash_entries;
