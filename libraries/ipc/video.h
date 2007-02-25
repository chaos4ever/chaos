/* $Id$ */
/* Abstract: Video protocol. */
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

#ifndef __LIBRARY_IPC_VIDEO_H__
#define __LIBRARY_IPC_VIDEO_H__

enum
{
  /* Set the specified video mode. */

  IPC_VIDEO_MODE_SET = (IPC_PROTOCOL_VIDEO << 16),
  
  /* Place the cursor on the given position. */

  IPC_VIDEO_CURSOR_PLACE,

  /* Set the full 256 color palette. */

  IPC_VIDEO_PALETTE_SET,

  /* Get the full 256 color palette. */

  IPC_VIDEO_PALETTE_GET,

  /* Set the font. */

  IPC_VIDEO_FONT_SET,
};

/* IPC structures for video. */

typedef struct
{
  unsigned int x;
  unsigned int y;
} video_cursor_type;

typedef struct
{
  u8 red;
  u8 green;
  u8 blue;
} video_palette_type;

/* Videomode set flags. */

enum
{
  VIDEO_MODE_TYPE_TEXT,
  VIDEO_MODE_TYPE_GRAPHIC
};

enum
{
  /* The call completed successfully. */

  IPC_VIDEO_RETURN_SUCCESS,

  /* The requested mode could not be set. */

  IPC_VIDEO_RETURN_MODE_UNAVAILABLE,
};

#endif /* !__LIBRARY_IPC_VIDEO_H__ */
