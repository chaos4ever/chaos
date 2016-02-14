// Abstract: Limitations on the kernel level.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/types.h>

// Limits of types.
#define MAX_U8                          ((u8) - 1)
#define MAX_U16                         ((u16) - 1)
#define MAX_U32                         ((u32) - 1)
#define MAX_TIME                        ((time_type) - 1)

// Other limitations.
#define MAX_PROCESS_NAME_LENGTH         128
#define MAX_THREAD_NAME_LENGTH          128

// The maximum length of the kernel parameters.
#define MAX_KERNEL_PARAMETER_LENGTH     256

// The maximum number of servers to start at boot time.
#define MAX_STARTUP_SERVERS             256

// The maximum length of a thread state name.
#define MAX_STATE_NAME_LENGTH           64

#define MAX_PROTOCOL_NAME_LENGTH        32
