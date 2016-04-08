// Abstract: TSS definition.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>

// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015-2016 chaos development

// FIXME: Rename this file to task.h, and make it more generic (also split it in two of course).

#pragma once

#include <storm/generic/defines.h>
#include <storm/generic/mutex.h>
#include <storm/generic/types.h>
#include <storm/generic/limits.h>
#include <storm/generic/capability.h>

// TSS structure (with some extra cheese, but no pepperoni).
typedef struct
{
    // CPU data.
    u16 previous_task_link;
    u16 u0;

    // Stack pointer for PL0 code (system calls).
    u32 esp0;
    u16 ss0;
    u16 u1;
    u32 esp1;
    u16 ss1;
    u16 u2;
    u32 esp2;
    u16 ss2;
    u16 u3;
    u32 cr3;

    // Instruction pointer and flags.
    u32 eip;
    u32 eflags;

    // General-purpose registers.
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;

    // Stack pointer.
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u16 es;
    u16 u4;
    u16 cs;
    u16 u5;
    u16 ss;
    u16 u6;
    u16 ds;
    u16 u7;
    u16 fs;

    // Unused field number 8...
    u16 u8;
    u16 gs;
    u16 u9;

    // This is not used by storm.
    u16 ldt_selector;
    u16 u10;
    u16 t: 1;
    u16 u11: 15;

    // Base address of I/O map.
    u16 iomap_base;

    // End of CPU data. Start of storm internal data.

    // Server or regular process.
    unsigned int process_type;

    // Process, cluster and thread ID.
    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;

    // Parent.
    struct storm_tss_type *parent_tss;

    // Owner of process.
    user_id_type user_id;

    // Priorities of the process, cluster and thread respectively.
    u32 priority_process;
    u32 priority_cluster;
    u32 priority_thread;

    // Memory usage and other information regarding memory.
    u32 stack_pages;
    u32 allocated_pages;

    // Mutex stuff.
    mutex_kernel_type *mutex_kernel;
    mutex_id_type mutex_user_id;

    time_type mutex_time;

    // Mailbox stuff.
    mailbox_id_type mailbox_id;

    // State of process.
    state_type state;

    // The number of timeslices this process has got.
    u64 timeslices;

    // The name of this thread.
    char thread_name[MAX_THREAD_NAME_LENGTH];

    // The following are used when creating clusters and spawning threads.
    u32 code_base, data_base, code_pages, data_pages;
    u32 virtual_code_base, virtual_data_base;

    // The current size of the I/O map.
    unsigned int iomap_size;

    // Process capabilities.
    capability_type capability;

    // Has the process run system_call_init() yet?
    bool initialised;

    // This is not the same as EIP; this contains the real instruction pointer. EIP always points into the dispatcher
    // because of the way our implementation of task-switching is done.
    u32 instruction_pointer;

    // Pointer to the the process information structure for this process. This must be defined using the "struct"
    // keyword since there is a circular reference between the TSS and process structures.
    struct process_info_type *process_info;

    // Controls which ports this process can access (bit clear means access enabled).
    u8 iomap[0];
} __attribute__((packed)) storm_tss_type;

// The regular Intel TSS type.
typedef struct
{
    // CPU data.
    u16 previous_task_link;
    u16 u0;

    // Stack pointer for PL0 code (system calls).
    u32 esp0;
    u16 ss0;
    u16 u1;
    u32 esp1;
    u16 ss1;
    u16 u2;
    u32 esp2;
    u16 ss2;
    u16 u3;
    u32 cr3;

    // Instruction pointer and flags.
    u32 eip;
    u32 eflags;

    // General-purpose registers.
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;

    // Stack pointer.
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u16 es;
    u16 u4;
    u16 cs;
    u16 u5;
    u16 ss;
    u16 u6;
    u16 ds;
    u16 u7;
    u16 fs;

    // Unused field number 8...
    u16 u8;
    u16 gs;
    u16 u9;

    // This is not used by storm.
    u16 ldt_selector;
    u16 u10;
    u16 t: 1;
    u16 u11: 15;

    // Base address of I/O map.
    u16 iomap_base;
} __attribute__((packed)) tss_type;

typedef struct
{
    struct tss_list_type *previous;
    struct tss_list_type *next;
    storm_tss_type *tss;
    thread_id_type thread_id;
} tss_list_type;
