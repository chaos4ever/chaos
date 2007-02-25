/* $Id$ */
/* Abstract: Function prototypes, defines and structures. */
/* Author: Anders Ohrt <doa@chaosdev.org>
           Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA */

#ifndef __TETRIS_H__
#define __TETRIS_H__

#define PLAYFIELD_WIDTH    12
#define PLAYFIELD_HEIGHT   22

/* Type definitions. */

typedef struct
{
  int block[4][4][4];
} piece_type;

/* External variables. */

extern console_structure_type console_structure;

/* Function prototypes. */

void tetris (void);

#endif /* !__TETRIS_H__ */
