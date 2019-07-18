// Abstract: TSS definition.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development

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
    uint16_t previous_task_link;
    uint16_t u0;

    // Stack pointer for PL0 code (system calls).
    uint32_t esp0;
    uint16_t ss0;
    uint16_t u1;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t u2;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t u3;
    uint32_t cr3;

    // Instruction pointer and flags.
    uint32_t eip;
    uint32_t eflags;

    // General-purpose registers.
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;

    // Stack pointer.
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t u4;
    uint16_t cs;
    uint16_t u5;
    uint16_t ss;
    uint16_t u6;
    uint16_t ds;
    uint16_t u7;
    uint16_t fs;

    // Unused field number 8...
    uint16_t uint8_t;
    uint16_t gs;
    uint16_t u9;

    // This is not used by storm.
    uint16_t ldt_selector;
    uint16_t u10;
    uint16_t t: 1;
    uint16_t u11: 15;

    // Base address of I/O map.
    uint16_t iomap_base;

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
    uint32_t priority_process;
    uint32_t priority_cluster;
    uint32_t priority_thread;

    // Memory usage and other information regarding memory.
    uint32_t stack_pages;
    uint32_t allocated_pages;

    // Mutex stuff.
    mutex_kernel_type *mutex_kernel;
    mutex_id_type mutex_user_id;

    time_type mutex_time;

    // Mailbox stuff.
    mailbox_id_type mailbox_id;

    // State of process.
    state_type state;

    // The number of timeslices this process has got.
    uint64_t timeslices;

    // The name of this thread.
    char thread_name[MAX_THREAD_NAME_LENGTH];

    // The following are used when creating clusters and spawning threads.
    uint32_t code_base, data_base, code_pages, data_pages;
    uint32_t virtual_code_base, virtual_data_base;

    // The current size of the I/O map.
    unsigned int iomap_size;

    // Process capabilities.
    capability_type capability;

    // Has the process run system_call_init() yet?
    bool initialised;

    // This is not the same as EIP; this contains the real instruction pointer. EIP always points into the dispatcher
    // because of the way our implementation of task-switching is done.
    uint32_t instruction_pointer;

    // Pointer to the the process information structure for this process. This must be defined using the "struct"
    // keyword since there is a circular reference between the TSS and process structures.
    struct process_info_type *process_info;

    // Controls which ports this process can access (bit clear means access enabled).
    uint8_t iomap[0];
} __attribute__((packed)) storm_tss_type;

// The regular Intel TSS type.
typedef struct
{
    // CPU data.
    uint16_t previous_task_link;
    uint16_t u0;

    // Stack pointer for PL0 code (system calls).
    uint32_t esp0;
    uint16_t ss0;
    uint16_t u1;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t u2;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t u3;
    uint32_t cr3;

    // Instruction pointer and flags.
    uint32_t eip;
    uint32_t eflags;

    // General-purpose registers.
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;

    // Stack pointer.
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t u4;
    uint16_t cs;
    uint16_t u5;
    uint16_t ss;
    uint16_t u6;
    uint16_t ds;
    uint16_t u7;
    uint16_t fs;

    // Unused field number 8...
    uint16_t uint8_t;
    uint16_t gs;
    uint16_t u9;

    // This is not used by storm.
    uint16_t ldt_selector;
    uint16_t u10;
    uint16_t t: 1;
    uint16_t u11: 15;

    // Base address of I/O map.
    uint16_t iomap_base;
} __attribute__((packed)) tss_type;

typedef struct
{
    struct tss_list_type *previous;
    struct tss_list_type *next;
    storm_tss_type *tss;
    thread_id_type thread_id;
} tss_list_type;
