/* $Id$ */
/* Abstract: General definitions. */
/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> 
            Anders Öhrt <doa@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */

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

#ifndef __STORM_DEFINES_H__
#define __STORM_DEFINES_H__

#undef NULL
#define NULL ((void *) 0)

#define TRUE (1 == 1)
#define FALSE (!TRUE)

/* Other definitions. */

#define PROCESS_ID_NONE         (MAX_ID)
#define CLUSTER_ID_NONE         (MAX_ID)
#define THREAD_ID_NONE          (MAX_ID)
#define MAILBOX_ID_NONE         (MAX_ID)

/* Some sizes. */

#define KB ((u32) 1024)
#define MB ((u32) 1024 * KB)
#define GB ((u32) 1024 * MB)

/* Convert a number to the closest aligned address above x. */

#define ALIGN(x,align)          ((x) % (align) == 0 ? \
                                (x) : \
                                (x) + ((align) - \
                                ((x) % (align))))

/* We always want to align things to the closest word - int is always
   equal to the machine word size. (Is this a false premise?) */

#define ARCHITECTURE_ALIGN      (sizeof (int))

/* Convert a number to the closest upper page aligned address. */

#define PAGE_ALIGN(x)           ((x) % SIZE_PAGE != 0 ? \
                                 (x) + (SIZE_PAGE - \
                                ((x) % SIZE_PAGE)) : \
                                 (x))

/* Calculate the number of pages needed to hold this number of
   bytes. */

#define SIZE_IN_PAGES(x)        (((x) % SIZE_PAGE) != 0 ? \
                                 ((x) / SIZE_PAGE) + 1 : \
                                 ((x) / SIZE_PAGE))

#endif /* !__STORM_DEFINES_H__ */
