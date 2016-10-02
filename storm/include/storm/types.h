// Abstract: Standard types used in the chaos operating system.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1998-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#include <stdint.h>

#pragma once

#ifndef __cplusplus
typedef int bool;
#endif

// Specific types.

#ifdef __i386__

typedef uint32_t address_type;
typedef int32_t return_type;
typedef uint32_t process_id_type;
typedef uint32_t cluster_id_type;
typedef uint32_t thread_id_type;
typedef uint32_t user_id_type;
typedef uint32_t group_id_type;
typedef uint64_t time_type;
typedef uint32_t state_type;

#else // !__i386__
#   error "Your host is not supported."
#endif

typedef unsigned int mailbox_id_type;
typedef unsigned int mutex_id_type;
typedef volatile int spinlock_type;
typedef uint32_t limit_type;
typedef void (thread_entry_point_type)(void *);
