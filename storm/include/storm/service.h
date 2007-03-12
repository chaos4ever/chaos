/* $Id$ */
/* Abstract: External service header. */
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

#ifndef __STORM_SERVICE_H__
#define __STORM_SERVICE_H__

#include <storm/mailbox.h>

/* This type is used to resolve the service providers of the given
   protocol. */

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

/* This type is used to read out the names of the protocols currently
   installed. */

typedef struct
{
  char max_protocols;
} service_protocol_parameter_type;

#endif /* !__STORM_SERVICE_H__ */
