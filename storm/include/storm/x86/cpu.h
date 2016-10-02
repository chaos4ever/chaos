// Abstract: CPU detection and some lowlevel routines.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <storm/generic/bit.h>
#include <storm/generic/defines.h>
#include <storm/generic/debug.h>
#include <storm/generic/memory.h>
#include <storm/x86/idt.h>
#include <storm/x86/port.h>

// External variables.
extern void cpu_structaddr;
extern long no_idt[2];

static inline void cpu_push_all(void)
{
    asm ("pusha");
}

static inline void cpu_pop_all(void)
{
    asm ("popa");
}

static inline void cpu_interrupt_return(void)
{
    asm ("iret");
}

static inline void cpu_interrupts_disable(void)
{
    asm ("cli");
}

static inline void cpu_interrupts_enable(void)
{
    asm ("sti");
}

static inline void cpu_halt(void) __attribute__ ((noreturn));
static inline void cpu_halt(void)
{
    cpu_interrupts_disable();
    while (TRUE) ;
}

static inline void cpu_no_operation(void)
{
    asm ("nop");
}

static inline uint32_t cpu_get_esp(void)
{
    uint32_t return_value;

    asm volatile ("movl %%esp, %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline uint32_t cpu_get_eflags(void)
{
    uint32_t return_value;

    asm volatile
    ("pushf \
    popl %0"
     : "=a" (return_value));

    return return_value;
}

static inline uint32_t cpu_get_cs(void)
{
    uint32_t return_value;

    asm volatile ("movl %%cs, %0"
                  : "=a" (return_value));

    return return_value;
}

static inline uint32_t cpu_get_cr0(void)
{
    uint32_t return_value;
    asm volatile ("movl %%cr0, %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline void cpu_set_cr0(uint32_t new_cr0)
{
    asm volatile ("movl %0, %%cr0"
                  :
                  : "r" (new_cr0));
}

static inline uint32_t cpu_get_cr2(void)
{
    uint32_t return_value;
    asm volatile ("movl %%cr2, %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline void cpu_set_cr3(uint32_t new_cr3)
{
    asm volatile ("movl %0, %%cr3"
                  :
                  : "r" (new_cr3));
}

static inline void cpu_set_cr4(uint32_t new_cr4)
{
    asm volatile ("movl %0, %%cr4"
                  :
                  : "r" (new_cr4));
}

static inline uint16_t cpu_get_tr(void)
{
    uint16_t return_value;
    asm volatile ("str %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline void cpu_set_tr(uint16_t new_tr)
{
    asm volatile ("ltr %0"
                  :
                  : "r" (new_tr));
}

static inline void cpu_reset(void) __attribute__ ((noreturn));
static inline void cpu_reset(void)
{
    // Based on an approach seen at http://wiki.osdev.org/Reboot
    const uint16_t keyboard_interface_port = 0x64;
    const uint8_t keyboard_reset = 0xFE;
    const uint8_t user_data_in_buffer = 0x02;

    // Clear the user data first.
    while (port_in_uint8_t(keyboard_interface_port) & user_data_in_buffer);

    port_out_uint8_t(keyboard_interface_port, keyboard_reset);

    // If all else fails, just halt the CPU.
    while (TRUE)
    {
        asm volatile ("hlt");
    }
}

// CR0 bits.
//
// Paging enabled.
#define CPU_CR0_PG (BIT_VALUE(31))

// Protected mode flag.
#define CPU_CR0_PE (BIT_VALUE(0))

// Extension type.
#define CPU_CR0_ET (BIT_VALUE(4))

// Write protect (486+).
#define CPU_CR0_WP (BIT_VALUE(16))

// CR4 bits.
//
// Enable global page directory entries.
#define CPU_CR4_PGE (BIT_VALUE(7))
