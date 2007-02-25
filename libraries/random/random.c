/* $Id$ */
/* Abstract: Library for calculating random numbers. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#include <random/random.h>
#include <system/system.h>

/* The algorithm is pretty pathetic right now, but it's better than
   nothing... */

static volatile int random_seed = 0;

/* Initialise the random seed. */

return_type random_init (int seed)
{
  random_seed = seed;

  return RANDOM_RETURN_SUCCESS;
}

/* Get a random number with max size max in *output. */

int random (int max)
{
  random_seed = (random_seed * 12345 + 67892);
  //  random_seed &= 0xFFFF;

  max++;

  return random_seed;
}
