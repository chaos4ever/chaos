// Abstract: General definitions.

// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>
//          Anders Öhrt <doa@chaosdev.org>

// © Copyright 1998-2000, 2013 chaos development.

#pragma once

#if (defined __i386__) || (defined __i486__) || (defined __i586__) || \
    (defined __i686__)
#  define SIZE_PAGE 4096
#else
#  error Your processor seems not to be supported by chaos.
#endif

#ifndef NULL
#  define NULL (void *) 0
#endif

// Dedicated to DoA, the one and only. ;)
#define TRUE (1 == 1)
#define FALSE (!TRUE)

// Other definitions.
#define PROCESS_ID_NONE         (MAX_U32)
#define PROCESS_ID_KERNEL       (0)
#define PROCESS_ID_USED         (1)
#define CLUSTER_ID_KERNEL       (0)
#define CLUSTER_ID_NONE         (MAX_U32)
#define THREAD_ID_KERNEL        (0)
#define THREAD_ID_NONE          (MAX_U32)
#define TASK_ID_KERNEL          (0)
#define TASK_ID_MAX             (MAX_U32)
#define MAILBOX_ID_NONE         (MAX_U32)

// Some sizes.
#define KB ((u32) 1024)
#define MB ((u32) 1024 * KB)
#define GB ((u32) 1024 * MB)

// Convert a number to the closest aligned address above x.
#define ALIGN(x, align)          ((x) % (align) == 0 ? \
                                  (x) : \
                                  (x) + ((align) - \
                                         ((x) % (align))))

// We always want to align things to the closest word - int is always equal to the machine word size. (Is this a false premise?)
#define ARCHITECTURE_ALIGN      (sizeof (int))

// Convert a number to the closest upper page aligned address.
#define PAGE_ALIGN(x)           ((x) % SIZE_PAGE != 0 ? \
                                 (x) + (SIZE_PAGE - \
                                        ((x) % SIZE_PAGE)) : \
                                 (x))

// Calculate the number of pages needed to hold this number of bytes.
#define SIZE_IN_PAGES(x)        (((x) % SIZE_PAGE) != 0 ? \
                                 ((x) / SIZE_PAGE) + 1 : \
                                 ((x) / SIZE_PAGE))

#define NORETURN __attribute__ ((noreturn))

#ifdef __cplusplus
#  define C_EXTERN extern "C"
#else
#  error "You should not use this define from non-C++ code"
#endif

#ifdef __cplusplus
#  define C_EXTERN_BEGIN        \
extern "C"                      \
{
#else
#  define C_EXTERN_BEGIN
#endif

#ifdef __cplusplus
#  define C_EXTERN_END          \
}
#else
#  define C_EXTERN_END
#endif
