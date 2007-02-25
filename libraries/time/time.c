/* $Id$ */
/* Abstract: Time/date library. */
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

#include "time.h"

/* Get the year number for the given chaos time. */

unsigned int time_to_year (time_type time) 
{
  return time;
}

/* Get the month number for the given chaos time. */

unsigned int time_to_month (time_type time) 
{
  return time;
}

/* Get the day of month number for the given chaos time. */

unsigned int time_to_day (time_type time) 
{
  return time;
}

/* Get the hours count for the given chaos time. */

unsigned int time_to_hours (time_type time)
{
  return ((time / 3600) % 24);
}

/* Get the minutes count for the given chaos time. */

unsigned int time_to_minutes (time_type time)
{
  return ((time / 60) % 60);
}

/* Get the seconds count for the given chaos time. */

unsigned int time_to_seconds (time_type time)
{
  return (time % 60);
}

/* Get the time. */

time_type time_get (void)
{
  kernelfs_time_type kernelfs_time;

  kernelfs_time.kernelfs_class = KERNELFS_CLASS_TIME_READ;
  system_call_kernelfs_entry_read (&kernelfs_time);

  return kernelfs_time.time;
}
