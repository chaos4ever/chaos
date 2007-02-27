/* $Id$ */
/* Abstract: VGA header file. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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
   USA. */

#ifndef __VGA_H__
#define __VGA_H__

#define VGA_PORT_BASE           0x3C0
#define VGA_PORTS               32
#define VGA_PALETTE_READ        (VGA_PORT_BASE + 7)
#define VGA_PALETTE_WRITE       (VGA_PORT_BASE + 8)
#define VGA_PALETTE_DATA        (VGA_PORT_BASE + 9)
#define VGA_SEQUENCER_REGISTER  0x3C4
#define VGA_SEQUENCER_DATA      0x3C5
#define VGA_GRAPHIC_REGISTER    0x3CE
#define VGA_GRAPHIC_DATA        0x3CF

#define VGA_MEMORY              0xA0000
#define VGA_MEMORY_SIZE         (64 * KB)

#endif /* !__VGA_H__ */
