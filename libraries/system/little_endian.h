// Abstract: Little endian functions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

#define SYSTEM_LITTLE_ENDIAN

#define system_little_endian_to_native_uint64_t(x) (x)
#define system_little_endian_to_native_u32(x) (x)
#define system_little_endian_to_native_uint16_t(x) (x)

#define system_big_endian_to_native_uint64_t(x) (system_byte_swap_uint64_t (x))
#define system_big_endian_to_native_u32(x) (system_byte_swap_uint32_t (x))
#define system_big_endian_to_native_uint16_t(x) (system_byte_swap_uint16_t (x))

#define system_native_to_big_endian_uint64_t(x) (system_byte_swap_uint64_t (x))
#define system_native_to_big_endian_u32(x) (system_byte_swap_uint32_t (x))
#define system_native_to_big_endian_uint16_t(x) (system_byte_swap_uint16_t (x))

#define system_native_to_little_endian_uint64_t(x) (x)
#define system_native_to_little_endian_u32(x) (x)
#define system_native_to_little_endian_uint16_t(x) (x)
