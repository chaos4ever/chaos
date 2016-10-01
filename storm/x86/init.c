// Abstract: Set up the GDT, stack, selectors and pass control to the kernel.
// Author: Per Lundberg <per@chaosdev.io>
//         Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1997-2000, 2007, 2013 chaos development.

#include <storm/generic/arguments.h>
#include <storm/generic/idle.h>
#include <storm/generic/dataarea.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/irq.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/port.h>
#include <storm/generic/types.h>

#include <storm/x86/gdt.h>

// The startup GDT we use. This is copied to its real location on startup, which is defined by BASE_GDT. For the descriptor format,
// see the Intel reference documentation or the OSDev wiki: http://wiki.osdev.org/Global_Descriptor_Table
static u16 temporary_gdt[] INIT_DATA =
{
    // Null descriptor. Generates GPF on access.
    0x0000,
    0x0000,
    0x0000,
    0x0000,

    // Kernel code segment (exec).
    0xFFFF,
    0x0000,
    0x9800,
    0x00CF,

    // Process data segment (read/write).
    0xFFFF,
    0x0000,
    0x9200,
    0x00CF,

    // Process code segment (exec).
    0xFFFF,
    0x0000,
    0xF800,
    0x00CF,

    // Process data segment (read/write).
    0xFFFF,
    0x0000,
    0xF200,
    0x00CF
};

// The GDT ant IDT fits into the first physical page.
u16 idtr[] UNUSED INIT_DATA =
{
    // IDT limit, 256 IDT entries.
    0x7FF,

    // IDT base.
    LOW_U16(BASE_IDT),
    HIGH_U16(BASE_IDT)
};

u16 gdtr[] UNUSED INIT_DATA =
{
    // GDT limit, 256 GDT entries.
    0x7FF,

    // GDT base.
    LOW_U16(BASE_GDT),
    HIGH_U16(BASE_GDT)
};

// Multiboot header.
u32 multiboot_header[] __attribute__ ((section(".init.pre"), unused)) =
{
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
};

// FIXME: Put those in another file. We should have a better system for local include files...
void _start (void) INIT_CODE;
static void INIT_CODE kernel_entry (void) NORETURN;
extern int kernel_main (int arguments, char *argument[]);

// This is the kernel entry point.
// FIXME: Use memory_copy and memory_set instead of inline assembler. The current way is very hard to optomize for e.g.
// MMX-based CPUs.
void _start(void)
{
    u8 *bios_data_area = (u8 *) 0x400;

    // EBX contains the address to the multiboot table. Save this in the kernel data table.
    asm ("movl    %0, %%edi\n"
         "movl    %%ebx, %%esi\n"
         "movl    %1, %%ecx\n"
         "rep     movsl"
         :
         : "g" ((u32) & multiboot_info), "g" (sizeof (multiboot_info_type) / 4));

    // FIXME: Update to new Multiboot video-standard.
    dataarea.x_size = bios_data_area[0x4A];
    dataarea.y_size = bios_data_area[0x84] + 1;

    dataarea.x_position = bios_data_area[0x50];
    dataarea.y_position = bios_data_area[0x51];

    // Move the GDT to the right location in memory.
    asm ("cld\n"
         "movl    %0, %%edi\n"
         "movl    %1, %%esi\n"
         "movl    %2, %%ecx\n"
         "rep     movsl\n"
         "movl    %3, %%ecx\n"
         "movl    $0, %%eax\n"
         "rep     stosl"
         :
         : "g" ((u32) BASE_GDT),
         "g" ((u32) & temporary_gdt),
         "n" (sizeof (temporary_gdt) / 4),
         "n" ((0x800 - sizeof (temporary_gdt)) / 4));

    // "Wipe them out. All of them." (Darth Sidious, to Nute Gunray)
    asm ("movl    %0, %%edi\n"
         "movl    $0, %%eax\n"
         "movl    %1, %%ecx\n"
         "rep     stosl"
         :
         : "n" (BASE_IDT), "n" (SIZE_IDT / 4));

    // Set up the GDTR and IDTR.
    // FIXME: Use constraints.
    asm ("lgdt    gdtr\n"
         "lidt    idtr");

    // Initialise the segment registers so they are loaded with our new selectors.
    asm ("movl    %0, %%eax\n"
         "movw    %%ax, %%ss\n"
         "movl    %1, %%esp\n"
         "movw    %%ax, %%es\n"
         "movw    %%ax, %%fs\n"
         "movw    %%ax, %%gs\n"
         "movw    %%ax, %%ds"
         :
         : "n" (SELECTOR_KERNEL_DATA),
         "n" (BASE_KERNEL_STACK + SIZE_KERNEL_STACK));

    // Pass control to the kernel.
    asm ("ljmp   %0, %1"
         :
         : "n" (SELECTOR_KERNEL_CODE),
         "p" (&kernel_entry));
}

// When this gets called, the new (temporary) GDT has been set up and the segment selectors have been initialized with
// their new values.
static void INIT_CODE kernel_entry(void)
{
    multiboot_init();

    kernel_main(((u8 *) arguments_kernel)[0], (char **) &arguments_kernel[4]);

    // This code should never really get called.
    idle();
}
