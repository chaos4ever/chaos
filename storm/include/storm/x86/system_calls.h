// Abstract: Function prototypes and structure definitions for system calls.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/x86/defines.h>
#include <storm/x86/types.h>
#include <storm/system_calls.h>

typedef struct
{
    u32 number;
    function_type handler;
    u8 arguments;
} system_call_type;

C_EXTERN_BEGIN

extern const system_call_type system_call[];
extern void system_calls_init(void) INIT_CODE;

C_EXTERN_END