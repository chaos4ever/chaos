// Abstract: Function prototypes and structure definitions of port I/O.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1998-2000, 2013 chaos development.

#pragma once

#include <storm/generic/types.h>

static inline void port_out_uint8_t(uint16_t port, uint8_t data)
{
    asm ("outb %1, %0"
         :
         : "Nd" (port),
         "a" (data));
}

static inline void port_out_uint16_t(uint16_t port, uint16_t data)
{
    asm ("outw %1, %0"
         :
         : "Nd" (port),
         "a" (data));
}

static inline void port_out_u32(uint16_t port, uint32_t data)
{
    asm ("outl %1, %0"
         :
         : "Nd" (port),
         "a" (data));
}

static inline uint8_t port_in_uint8_t(uint16_t port)
{
    uint8_t return_value;

    asm volatile ("inb %1, %0"
                  : "=a" (return_value)
                  : "Nd" (port));

    return return_value;
}

static inline uint16_t port_in_uint16_t(uint16_t port)
{
    uint16_t return_value;

    asm volatile ("inw %1, %0"
                  : "=a" (return_value)
                  : "Nd" (port));

    return return_value;
}

static inline uint32_t port_in_u32(uint16_t port)
{
    uint32_t return_value;

    asm volatile ("inl %1, %0"
                  : "=a" (return_value)
                  : "Nd" (port));

    return return_value;
}

static inline void port_out_uint8_t_string(uint16_t port, uint8_t *data, uint32_t length)
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

static inline void port_out_u32_string(uint16_t port, uint32_t *data, uint32_t length)
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

static inline void port_in_u32_string(uint16_t port, uint32_t *data, uint32_t length)
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
