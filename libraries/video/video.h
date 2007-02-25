/* $Id$ */
/* Abstract: Graphics library function prototypes and type
   definitions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_VIDEO_VIDEO_H__
#define __LIBRARY_VIDEO_VIDEO_H__

#include <system/system.h>
#include <video/return_values.h>

/* FIXME: Move to video/types.h */
/* Type definitions. */

typedef struct
{
  /* Video mode resolution. */

  unsigned int width;
  unsigned int height;
  unsigned int depth;
  
  /* Do we want a double buffered mode? We may get it anyway, but we
     might as well state what we'd prefer. For example, most games
     would likely always want a double buffered mode for flicker-free
     animation. */

  bool buffered;

  /* Which type of mode do we want? (Graphic or text) */

  unsigned int mode_type;
} video_mode_type;

/* Function prototypes. */

extern return_type video_set_palette 
  (ipc_structure_type *video_structure,
   video_palette_type *palette);
extern return_type video_get_palette 
  (ipc_structure_type *video_structure,
   video_palette_type *palette);
extern return_type video_mode_set 
  (ipc_structure_type *video_structure,
   video_mode_type *video_mode);
extern return_type video_init
  (ipc_structure_type *video_structure, tag_type *tag);

#endif /* !__LIBRARY_VIDEO_VIDEO_H__ */
