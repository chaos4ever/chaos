/* $Id$ */
/* Abstract: Unique ID generation. */
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

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/id.h>
#include <storm/types.h>

static id_type free_id = 0;

/* Allocate a new ID. */
/* FIXME: Implement the ID pool we've been thinking about. */

id_type id_allocate (void)
{
  id_type id = free_id;

  free_id++;
  return id;
}

/* Deallocate an ID no longer in use. */

void id_deallocate (id_type id UNUSED)
{
}
