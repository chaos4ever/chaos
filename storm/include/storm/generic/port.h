// Abstract: Function prototypes and structure definitions of ISA port I/O.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1998-2000, 2013 chaos development.

#pragma once

#include <storm/generic/defines.h>
#include <storm/generic/types.h>

typedef struct
{
    unsigned int start;
    unsigned int length;
    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;
    struct port_range_type *next;
    struct port_range_type *previous;
    char *description;
} port_range_type;

extern void port_init(void) INIT_CODE;
extern return_type port_range_register(unsigned int start, unsigned int ports, char *description);
extern return_type port_range_unregister(unsigned int start);
extern void port_range_free_all(thread_id_type thread_id);
extern bool port_range_link(unsigned int start, unsigned int ports, char *description);

extern port_range_type *port_list;

#include <storm/current-arch/port.h>
