/* $Id$ */
/* Abstract: Function prototypes and structure definitions of port in-
   and output. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */

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

#ifndef __LIBRARY_SYSTEM_PORT_H__
#define __LIBRARY_SYSTEM_PORT_H__

#include <storm/storm.h>

/* Inlines. */

/* I/O functions. */

static inline void system_port_out_u8 (u16 port, u8 data)
{
  asm ("outb %1, %0"
       :
       : "Nd" (port),
         "a" (data));
}

static inline void system_port_out_u16 (u16 port, u16 data)
{
  asm ("outw %1, %0"
       :
       : "Nd" (port),
         "a" (data));
}

static inline void system_port_out_u32 (u16 port, u32 data)
{
  asm ("outl %1, %0"
       :
       : "Nd" (port),
         "a" (data));
}

/* 'Pausing' version of the above. */

static inline void system_port_out_u8_pause (u16 port, u8 data)
{
  asm ("outb %1, %0
        jmp 1f
1:      jmp 2f
2:     "
       :
       : "Nd" (port),
         "a" (data));
  system_sleep (1);
}

static inline void system_port_out_u16_pause (u16 port, u16 data)
{
  asm ("outw %1, %0
        jmp 1f
1:      jmp 2f
2:
       "
       :
       : "Nd" (port),
         "a" (data));
}

static inline void system_port_out_u32_pause (u16 port, u32 data)
{
  asm ("outl %1, %0
        jmp 1f
1:     jmp 2f
2:     "
       :
       : "Nd" (port),
         "a" (data));
}

/* Input operations. */

static inline u8 system_port_in_u8 (u16 port)
{
  u8 return_value;

  asm volatile ("inb %1, %0"
                : "=a" (return_value)
                : "Nd" (port));

  return return_value;
}

static inline u16 system_port_in_u16 (u16 port)
{
  u16 return_value;

  asm volatile ("inw %1, %0"
                : "=a" (return_value)
                : "Nd" (port));

  return return_value;
}

static inline u32 system_port_in_u32 (u16 port)
{
  u32 return_value;

  asm volatile ("inl %1, %0"
                : "=a" (return_value)
                : "Nd" (port));

  return return_value;
}

/* String operations. */

static inline void system_port_out_u8_string (u16 port, u8 *data, u32 length)
{
  asm volatile ("cld\n"
                "rep\n"
                "outsb"
                :
                : "c" (length),
                  "S" (data),
                  "d" (port));
}

static inline void system_port_out_u32_string (u16 port, u32 *data, u32 length)
{
  asm volatile ("cld\n"
                "rep\n"
                "outsl"
                :
                : "c" (length),
                  "S" (data),
                  "d" (port));
}

static inline void system_port_in_u8_string (u16 port, u8 *data, u32 length)
{
  asm volatile ("cld\n"
                "rep\n"
                "insb"
                :
                : "c" (length),
                  "D" (data),
                  "d" (port));
}

static inline void system_port_in_u16_string (u16 port, u16 *data, u32 length)
{
  asm volatile ("cld\n"
                "rep\n"
                "insw"
                :
                : "c" (length),
                  "D" (data),
                  "d" (port));
}

static inline void system_port_in_u32_string (u16 port, u32 *data, u32 length)
{
  asm volatile ("cld\n"
                "rep\n"
                "insl"
                :
                : "c" (length),
                  "D" (data),
                  "d" (port));
}

#endif /* !__LIBRARY_SYSTEM_PORT_H__ */
