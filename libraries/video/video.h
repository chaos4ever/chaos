// Abstract: Graphics library main header file.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <ipc/ipc.h>
#include <system/system.h>
#include <video/types.h>
#include <video/return_values.h>

extern return_type video_set_palette(ipc_structure_type *video_structure, video_palette_type *palette);
extern return_type video_get_palette(ipc_structure_type *video_structure, video_palette_type *palette);
extern return_type video_mode_set(ipc_structure_type *video_structure, video_mode_type *video_mode);
extern return_type video_init(ipc_structure_type *video_structure, tag_type *tag);
