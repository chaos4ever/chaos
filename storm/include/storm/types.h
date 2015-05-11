// Abstract: Standard types used in the chaos operating system.
// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1998-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once

// General types. Those should only be used when you actually *need* a fixed size variable (for example in IP headers); for
// performance reasons, use int or unsigned whenever a generic type is desired. But if you do, remember to not take for granted
// that the maximum value is 2^32 - 1 etcetera.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

#ifndef __cplusplus
typedef int bool;
#endif

typedef unsigned long long u64;
typedef signed long long s64;

// Specific types.

#ifdef __i386__

typedef u32 address_type;
typedef s32 return_type;
typedef u32 process_id_type;
typedef u32 cluster_id_type;
typedef u32 thread_id_type;
typedef u32 user_id_type;
typedef u32 group_id_type;
typedef u64 time_type;
typedef u32 state_type;

#else // !__i386__
#   error "Your host is not supported."
#endif

typedef unsigned int mailbox_id_type;
typedef unsigned int mutex_id_type;
typedef volatile int spinlock_type;
typedef u32 limit_type;
