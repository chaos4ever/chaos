/* $Id$ */
/* Abstract: Time library header file. */
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

#ifndef __LIBRARY_TIME_TIME_H__
#define __LIBRARY_TIME_TIME_H__

#include <system/system.h>

extern unsigned int time_to_year (time_type time);
extern unsigned int time_to_month (time_type time);
extern unsigned int time_to_day (time_type time);

extern unsigned int time_to_hours (time_type time);
extern unsigned int time_to_minutes (time_type time);
extern unsigned int time_to_seconds (time_type time);

extern time_type time_get (void);

#endif /* !__LIBRARY_TIME_TIME_H__ */
