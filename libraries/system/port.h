// Abstract: Function prototypes and structure definitions of port in-and output.
// Author: Per Lundberg <per@chaosdev.io>
//
// Probably a bit based on Linux or similar (to know the gcc syntax for inline assembly).
//
// © Copyright 1998-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <storm/storm.h>

// Inlines.
// I/O functions.
static inline void system_port_pause(void)
{
    asm ("outb %al, $0x80");
}

static inline void system_port_out_uint8_t(uint16_t port, uint8_t data)
{
    asm("outb %1, %0"
        :
        : "Nd" (port),
          "a" (data));
}

static inline void system_port_out_uint16_t(uint16_t port, uint16_t data)
{
    asm("outw %1, %0"
        :
        : "Nd" (port),
          "a" (data));
}

static inline void system_port_out_u32(uint16_t port, uint32_t data)
{
    asm("outl %1, %0"
        :
        : "Nd" (port),
          "a" (data));
}

// 'Pausing' version of the above.
static inline void system_port_out_uint8_t_pause(uint16_t port, uint8_t data)
{
    asm("outb %1, %0\n"
         :
         : "Nd" (port),
           "a" (data));
    system_port_pause();
}

static inline void system_port_out_uint16_t_pause(uint16_t port, uint16_t data)
{
    asm ("outw %1, %0\n"
         :
         : "Nd" (port),
           "a" (data));
    system_port_pause();
}

static inline void system_port_out_u32_pause(uint16_t port, uint32_t data)
{
    asm("outl %1, %0\n"
        :
        : "Nd" (port),
          "a" (data));
    system_port_pause();
}

// Input operations.
static inline uint8_t system_port_in_uint8_t(uint16_t port)
{
    uint8_t return_value;

    asm volatile("inb %1, %0"
                 : "=a" (return_value)
                 : "Nd" (port));

    return return_value;
}

static inline uint16_t system_port_in_uint16_t(uint16_t port)
{
    uint16_t return_value;

    asm volatile("inw %1, %0"
                 : "=a" (return_value)
                 : "Nd" (port));

    return return_value;
}

static inline uint32_t system_port_in_u32(uint16_t port)
{
    uint32_t return_value;

    asm volatile("inl %1, %0"
                 : "=a" (return_value)
                 : "Nd" (port));

    return return_value;
}

static inline void system_port_out_uint8_t_string(uint16_t port, uint8_t *data, uint32_t length)
{
    asm volatile("cld\n"
                 "rep\n"
                 "outsb"
                 :
                 : "c" (length),
                   "S" (data),
                   "d" (port));
}

static inline void system_port_out_uint16_t_string(uint16_t port, uint16_t *data, uint32_t length)
{
    asm volatile("cld\n"
                 "rep\n"
                 "outsw"
                 :
                 : "c" (length),
                   "S" (data),
                   "d" (port));
}

static inline void system_port_out_u32_string(uint16_t port, uint32_t *data, uint32_t length)
{
    asm volatile("cld\n"
                 "rep\n"
                 "outsl"
                 :
                 : "c" (length),
                   "S" (data),
                   "d" (port));
}

static inline void system_port_in_uint8_t_string(uint16_t port, uint8_t *data, uint32_t length)
{
    asm volatile("cld\n"
                 "rep\n"
                 "insb"
                 :
                 : "c" (length),
                   "D" (data),
                   "d" (port));
}

static inline void system_port_in_uint16_t_string(uint16_t port, uint16_t *data, uint32_t length)
{
    asm volatile("cld\n"
                 "rep\n"
                 "insw"
                 :
                 : "c" (length),
                   "D" (data),
                   "d" (port));
}

static inline void system_port_in_u32_string(uint16_t port, uint32_t *data, uint32_t length)
{
    asm volatile("cld\n"
                 "rep\n"
                 "insl"
                 :
                 : "c" (length),
                   "D" (data),
                   "d" (port));
}
