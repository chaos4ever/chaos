// Abstract: External service header.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/mailbox.h>

// This type is used to resolve the service providers of the given protocol.
typedef struct
{
    unsigned int max_services;
    mailbox_id_type *mailbox_id;
} service_parameter_type;

typedef struct
{
    char name[MAX_PROTOCOL_NAME_LENGTH];
    unsigned int number_of_services;
} service_protocol_type;

// This type is used to read out the names of the protocols currently installed.
typedef struct
{
    char max_protocols;
} service_protocol_parameter_type;
