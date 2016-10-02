// Abstract: Function prototypes for the kernel file system.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>

// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <storm/types.h>
#include <storm/limits.h>

// Command classes. When adding things here, ***ALWAYS*** put it at the end of the list to avoid breaking all existing programs...
enum
{
    KERNELFS_CLASS_THREAD_AMOUNT,
    KERNELFS_CLASS_THREAD_INFO,
    KERNELFS_CLASS_THREAD_INFO_VERBOSE,

    KERNELFS_CLASS_PROCESS_AMOUNT,
    KERNELFS_CLASS_PROCESS_INFO,

    KERNELFS_CLASS_UPTIME_INFO,
    KERNELFS_CLASS_MEMORY_INFO,
    KERNELFS_CLASS_NUMBER_OF_TIMESLICES,

    KERNELFS_CLASS_CPU_AMOUNT,
    KERNELFS_CLASS_CPU_INFO,

    KERNELFS_CLASS_STORM_INFO,
    KERNELFS_CLASS_VIDEO_INFO,
    KERNELFS_CLASS_SELF_INFO,

    KERNELFS_CLASS_PORT_AMOUNT,
    KERNELFS_CLASS_PORT_INFO,

    KERNELFS_CLASS_IRQ_AMOUNT,
    KERNELFS_CLASS_IRQ_INFO,

    KERNELFS_CLASS_LOG_READ,

    KERNELFS_CLASS_TIME_READ,
};

// "Meta"-type which all other kernelfs types are derived from.
typedef struct
{
    unsigned int kernelfs_class;
} kernelfs_generic_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Input parameters.
    //
    // The process index number in the tables.
    unsigned int process_number;

    // Output parameters.
    //
    // The real process ID.
    process_id_type process_id;
    unsigned int number_of_threads;
    char name[MAX_PROCESS_NAME_LENGTH];
} kernelfs_process_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Input parameters.
    unsigned int thread_number;
    process_id_type process_id;

    // Output parameters.
    thread_id_type thread_id;
    char name[MAX_THREAD_NAME_LENGTH];
    char state[MAX_STATE_NAME_LENGTH];

    // Memory usage (in bytes).
    uint32_t memory_usage;
} kernelfs_thread_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Input parameters.
    unsigned int thread_number;
    process_id_type process_id;

    // Output parameters.
    thread_id_type thread_id;

    // The number of timeslices this thread has been given.
    time_type timeslices;

    // The number of memory bytes this process is using.
    uint32_t main_memory;
    uint32_t stack_memory;

    char thread_name[MAX_THREAD_NAME_LENGTH];

    // The current instruction pointer of the thread.
    uint32_t instruction_pointer;
} kernelfs_thread_info_verbose_type;

typedef struct
{
    // Input parameters.
    unsigned int kernelfs_class;

    // Output parameters.
    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;
} kernelfs_self_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Output parameters.
    //
    // The exact speed of this CPU.
    uint32_t hz;

    // 486 DX/4, K6-2, Athlon, etc.
    // FIXME: Get this from CPUID if AMD.
    char name[64];

    // GenuineIntel, AuthenticAMD, CyrixInstead etcetera..
    char vendor[17];
} kernelfs_cpu_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Output parameters.
    uint32_t total_memory;
    uint32_t free_memory;

    uint32_t total_global_memory;
    uint32_t free_global_memory;
} kernelfs_memory_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Output parameters.
    uint32_t mode_type;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    void *frame_buffer_address;
} kernelfs_video_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Output parameters.
    char version[16];
} kernelfs_storm_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Input parameters.
    unsigned int port;

    // Output parameters.
    unsigned int base;
    unsigned int length;
    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;

    // FIXME: Have a define for the max length of this.
    char description[128];
} kernelfs_port_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Input parameter.
    unsigned int which;

    // Output parameters.
    unsigned int level;
    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;
    unsigned long long occurred;
    char description[128];
} kernelfs_irq_info_type;

typedef struct
{
    unsigned int kernelfs_class;

    // Input parameter.
    unsigned int max_string_length;
    bool block;

    // Output and input parameter.
    char *string;
} kernelfs_log_type;

typedef struct
{
    unsigned int kernelfs_class;

    // The current time, in chaos format (milliseconds since the beginning of year 0. Yes, we know there were no such year, but it
    // makes things easier if we pretend it actually existed...).
    time_type time;
} kernelfs_time_type;
