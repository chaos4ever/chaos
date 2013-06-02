// Abstract: Function prototypes for the service system.
// Author: Per Lundberg <per@halleluja.nu>

// Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/current-arch/types.h>
#include <storm/tag.h>
#include <storm/service.h>

typedef struct
{
  mailbox_id_type mailbox_id;
  struct service_type *next;
  tag_type *identification;
} service_type;

typedef struct
{
  char *name;
  unsigned int number_of_services;
  struct service_type *service;
  struct protocol_type *less;
  struct protocol_type *more;
} protocol_type;

#define SERVICE_MAILBOX_SIZE    1024

extern return_type service_create
  (const char *protocol_name,  mailbox_id_type *mailbox_id,
   tag_type *identification);

extern return_type service_destroy (mailbox_id_type mailbox_id);

extern return_type service_get 
  (const char *protocol_name, service_parameter_type *service_parameter,
   tag_type *identification_mask);

extern return_type service_protocol_get_amount 
  (unsigned int *number_of_protocols);

extern return_type service_protocol_get
  (unsigned int *maximum_protocols, service_protocol_type *protocol_info);
