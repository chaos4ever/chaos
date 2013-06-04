// Abstract: Function prototypes and structure definitions of port I/O.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1998-2000, 2013 chaos development.

#pragma once

#include <storm/generic/types.h>

static inline void port_out_u8(u16 port, u8 data)
{
    asm ("outb %1, %0"
         :
         : "Nd" (port),
         "a" (data));
}

static inline void port_out_u16(u16 port, u16 data)
{
    asm ("outw %1, %0"
         :
         : "Nd" (port),
         "a" (data));
}

static inline void port_out_u32(u16 port, u32 data)
{
    asm ("outl %1, %0"
         :
         : "Nd" (port),
         "a" (data));
}

static inline u8 port_in_u8(u16 port)
{
    u8 return_value;

    asm volatile ("inb %1, %0"
                  : "=a" (return_value)
                  : "Nd" (port));

    return return_value;
}

static inline u16 port_in_u16(u16 port)
{
    u16 return_value;

    asm volatile ("inw %1, %0"
                  : "=a" (return_value)
                  : "Nd" (port));

    return return_value;
}

static inline u32 port_in_u32(u16 port)
{
    u32 return_value;

    asm volatile ("inl %1, %0"
                  : "=a" (return_value)
                  : "Nd" (port));

    return return_value;
}

static inline void port_out_u8_string(u16 port, u8 *data, u32 length)
{
    asm volatile
    ("cld\n"
     "rep\n"
     "outsb"
     :
     : "c" (length),
     "S" (data),
     "d" (port));
}

static inline void port_out_u32_string(u16 port, u32 *data, u32 length)
{
    asm volatile
    ("cld\n"
     "rep\n"
     "outsl"
     :
     : "c" (length),
     "S" (data),
     "d" (port));
}

static inline void port_in_u32_string(u16 port, u32 *data, u32 length)
{
    asm volatile
    ("cld\n"
     "rep\n"
     "insl"
     :
     : "c" (length),
     "D" (data),
     "d" (port));
}
