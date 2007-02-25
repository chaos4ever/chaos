/* $Id$ */
/* Abstract: Log urgencies. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#ifndef __LIBRARY_LOG_DEFINES_H__
#define __LIBRARY_LOG_DEFINES_H__

enum
{
  /* Emergency messages are to be printed when the program has
     encountered a serious error which forces termination. */

  LOG_URGENCY_EMERGENCY,

  /* Something is wrong, but execution may continue. */

  LOG_URGENCY_ERROR,

  /* Something might be wrong, but we are not entirely sure. */

  LOG_URGENCY_WARNING,

  /* Typical informative messages are showing which hardware we
     detected, etc. */

  LOG_URGENCY_INFORMATIVE,

  /* Debug messages are for debugging purposes only -- normally, they
     can be safely ignored. Eventually, they will be removed. */

  LOG_URGENCY_DEBUG
};

#endif /* !__LIBRARY_LOG_DEFINES_H__ */
