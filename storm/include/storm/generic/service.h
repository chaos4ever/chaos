/* $Id$ */
/* Abstract: Function prototypes for the service system. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __STORM_GENERIC_SERVICE_H__
#define __STORM_GENERIC_SERVICE_H__

#include <storm/current-arch/types.h>
#include <storm/tag.h>
#include <storm/service.h>

/* A service. */

typedef struct
{
  mailbox_id_type mailbox_id;
  struct service_type *next;
  tag_type *identification;
} service_type;

/* A protocol. */

typedef struct
{
  char *name;
  unsigned int number_of_services;
  struct service_type *service;
  struct protocol_type *less;
  struct protocol_type *more;
} protocol_type;

#define SERVICE_MAILBOX_SIZE    1024

/* Create a new service. */

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

#endif /* !__STORM_GENERIC_SERVICE_H__ */
