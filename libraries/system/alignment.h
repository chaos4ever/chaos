// Abstract: Alignment functions.
// Author: Anders Ohrt <doa@chaosdev.org>
//
// © Copyright 1999 chaos development.

#pragma once

#if defined __i386__

#define system_get_unaligned_u32(pointer) (*(uint32_t *) (pointer))
#define system_get_unaligned_uint16_t(pointer) (*(uint16_t *) (pointer))
#define system_get_unaligned_uint8_t(pointer) (*(uint8_t *) (pointer))

#define system_get_unaligned_s32(pointer) (*(int32_t *) (pointer))
#define system_get_unaligned_s16(pointer) (*(s16 *) (pointer))
#define system_get_unaligned_s8(pointer) (*(s8 *) (pointer))

#endif
