// Abstract: Limitation variables. All of those should in the future be overridable via kernel parameters or similar.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>

// Copyright 2000, 2013 chaos development.

#include <storm/generic/defines.h>
#include <storm/generic/types.h>

// System limitations. Can be overridden by kernel parameters. This should be a multiple of four megabytes (one page directory
// entry).
limit_type limit_global_heap = 128 * MB;

// This limits the maximum amount of nodes in the tree over globally allocated memory.
limit_type limit_global_nodes = 5000000;

// This overrides the detected memory size. Can be used if the motherboard reports the wrong size.
limit_type limit_memory = 0;

// Number of entries in the mailbox hash table.
limit_type limit_mailbox_hash_entries = 1024;

// Number of entries in the thread hash table.
limit_type limit_thread_hash_entries = 1024;
