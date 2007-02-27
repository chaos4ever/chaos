/* $Id$ */
/* Abstract: Architecture dependent time management. */
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

#include <storm/current-arch/port.h>
#include <storm/generic/debug.h>
#include <storm/generic/time.h>

#define CMOS_BASE       0x70

#define RTC_SECONDS     0
#define RTC_MINUTES     2
#define RTC_HOURS       4
#define RTC_DAY         7
#define RTC_MONTH       8
#define RTC_YEAR        9

/* Read from the CMOS. */

static inline unsigned int cmos_read (unsigned int which)
{
  unsigned int data;
  port_out_u8 (CMOS_BASE, which);
  data = port_in_u8 (CMOS_BASE + 1);
  data = ((data & 0x0F) + (data >> 4) * 10);

  return data;
}

/* Convert a date/time in Gregorian time to the standard chaos
   format. */

static time_type gregorian_to_chaos
  (unsigned int year, unsigned int month, unsigned int day,
   unsigned int hours, unsigned int minutes, unsigned int seconds)
{
  unsigned int leap_years = (year / 4) - (year / 100) + (year / 400);
  unsigned int month_start[] =
  {
    /* January. */   0,
    /* February. */  31,  
    /* March. */     31 + 28,
    /* April. */     31 + 28 + 31,
    /* May. */       31 + 28 + 31 + 30,
    /* June. */      31 + 28 + 31 + 30 + 31,
    /* July. */      31 + 28 + 31 + 30 + 31 + 31,
    /* August. */    31 + 28 + 31 + 30 + 31 + 31 + 30,
    /* September. */ 31 + 28 + 31 + 30 + 31 + 31 + 30 + 31,
    /* October. */   31 + 28 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    /* November. */  31 + 28 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,
    /* December. */  31 + 28 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 + 30
  };
  time_type work;

  work = leap_years * 366 + (year - leap_years) * 365;
  work += month_start[month] + day;
  if ((year % 400 == 0 ||
      (year % 100 != 0 && year % 4 == 0)) && month >= 2)
  {
    work++;
  }

  work = ((work * 24 + hours) * 60 + minutes) * 60 + seconds;

  return work;
}

/* Initialise the time (by reading from the CMOS). */

void time_init (void)
{
  unsigned int seconds = cmos_read (RTC_SECONDS);
  unsigned int minutes = cmos_read (RTC_MINUTES);
  unsigned int hours = cmos_read (RTC_HOURS);
  unsigned int day = cmos_read (RTC_DAY);
  unsigned int month = cmos_read (RTC_MONTH);
  unsigned int year = cmos_read (RTC_YEAR) + 1900;

  /* Convert this time to the chaos format. */

  time = gregorian_to_chaos (year, month, day, hours, minutes, seconds);
}
