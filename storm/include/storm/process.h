/* $Id$ */
/* Abstract: Process control. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#ifndef __STORM_PROCESS_H__
#define __STORM_PROCESS_H__

#include <storm/types.h>

/* Process types. */

enum
{
  PROCESS_TYPE_REGULAR,
  PROCESS_TYPE_SERVER
};

/* Structure used by process_create. */

typedef struct
{
  u32 process_type;
  u32 initial_eip;
  process_id_type *process_id;

  bool block;

  u8 *code_section_address;
  u32 code_section_base;
  u32 code_section_size;

  u8 *data_section_address;
  u32 data_section_base;
  u32 data_section_size;

  u32 bss_section_base;
  u32 bss_section_size;
  char *parameter_string;
} process_create_type;

#endif /* __STORM_PROCESS_H__ */
