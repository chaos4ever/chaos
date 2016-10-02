/* $Id$ */
/* Abstract: Partitioning stuff. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

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

#ifndef __PARTITION_H__
#define __PARTITION_H__

#include "config.h"

/* A partition table entry. */

typedef struct
{
  /* Active flag. */

  uint8_t reserved : 7;
  uint8_t active : 1;

  uint8_t starting_head;
  uint16_t starting_sector : 6;
  uint16_t starting_cylinder : 10;

  /* Type of the partition. */

  uint8_t type;

  uint8_t ending_head;
  uint16_t ending_sector : 6;
  uint16_t ending_cylinder : 10;

  /* The partition descibed in a 'correct' way. */

  uint32_t lba_starting_sector_number;
  uint32_t lba_number_of_sectors;
} __attribute__ ((packed)) partition_entry_type;

#define PARTITION_TYPE_EXTENDED_DOS             0x05
#define PARTITION_TYPE_EXTENDED_LINUX           0x85
#define PARTITION_TYPE_EXTENDED_WINDOWS         0x0F

#endif /* !__PARTITION_H__ */
