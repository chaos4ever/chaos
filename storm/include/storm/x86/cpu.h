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

static inline u32 cpu_get_esp(void)
{
    u32 return_value;

    asm volatile ("movl %%esp, %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline u32 cpu_get_eflags(void)
{
    u32 return_value;

    asm volatile
    ("pushf \
    popl %0"
     : "=a" (return_value));

    return return_value;
}

static inline u32 cpu_get_cs(void)
{
    u32 return_value;

    asm volatile ("movl %%cs, %0"
                  : "=a" (return_value));

    return return_value;
}

static inline u32 cpu_get_cr0(void)
{
    u32 return_value;
    asm volatile ("movl %%cr0, %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline void cpu_set_cr0(u32 new_cr0)
{
    asm volatile ("movl %0, %%cr0"
                  :
                  : "r" (new_cr0));
}

static inline u32 cpu_get_cr2(void)
{
    u32 return_value;
    asm volatile ("movl %%cr2, %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline void cpu_set_cr3(u32 new_cr3)
{
    asm volatile ("movl %0, %%cr3"
                  :
                  : "r" (new_cr3));
}

static inline void cpu_set_cr4(u32 new_cr4)
{
    asm volatile ("movl %0, %%cr4"
                  :
                  : "r" (new_cr4));
}

static inline u16 cpu_get_tr(void)
{
    u16 return_value;
    asm volatile ("str %0"
                  : "=a" (return_value)
                  :);

    return return_value;
}

static inline void cpu_set_tr(u16 new_tr)
{
    asm volatile ("ltr %0"
                  :
                  : "r" (new_tr));
}

static inline void cpu_reset(void) __attribute__ ((noreturn));
static inline void cpu_reset(void)
{
    // Based on an approach seen at http://wiki.osdev.org/Reboot
    const u16 keyboard_interface_port = 0x64;
    const u8 keyboard_reset = 0xFE;
    const u8 user_data_in_buffer = 0x02;

    // Clear the user data first.
    while (port_in_u8(keyboard_interface_port) & user_data_in_buffer);

    port_out_u8(keyboard_interface_port, keyboard_reset);

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
