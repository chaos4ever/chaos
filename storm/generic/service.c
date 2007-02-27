/* $Id$ */
/* Abstract: Support for services. */
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

/* The services are used for getting information about how to
   communicate with a server of a specific type. The types are not
   regulated by the kernel. Instead, when a server registers its
   service, the kernel creates the given class. This gives a great
   responsibility to the servers to not pollute the namespace, but we
   trust people not to misuse this. If you're uncertain about were
   your service should be placed, please contact staff@chaosdev.org */

/* The services are stored as a linked list in a tree. The tree is
   sorted on protocol name. The linked list is sorted on inverse
   chronology. */

#define DEBUG FALSE

#include <storm/generic/debug.h>
#include <storm/generic/irq.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/mailbox.h>
#include <storm/generic/service.h>
#include <storm/generic/tag.h>

#include <storm/return_values.h>

//mutex_kernel_type service_mutex = MUTEX_UNLOCKED;

static protocol_type *root = NULL;

/* Inserts the given protocol into its place in the tree. */

static void protocol_insert (protocol_type *protocol)
{
  int return_value;
  
  protocol_type *node = root;
  protocol_type *parent;

  /* If the tree is empty, put this protocol at the root. */

  if (node == NULL)
  {
    root = protocol;
    return;
  }
  
  while (node != NULL)
  {
    return_value = string_compare (protocol->name, node->name);

    parent = node;

    if (return_value < 0)
    {
      node = (protocol_type *) node->less;
    }
    else if (return_value > 0)
    {
      node = (protocol_type *) node->more;
    }
    else
    {
      DEBUG_HALT ("protocol_name == node->name");
    }
  }

  if (return_value < 0)
  {
    parent->less = (struct protocol_type *) protocol;
  }
  else if (return_value > 0)
  {
    parent->more = (struct protocol_type *) protocol;
  }
}

/* Get a pointer to the given protocol, if it exists, or NULL
   otherwise. */

static protocol_type *protocol_get (const char *protocol_name)
{
  protocol_type *protocol = root;
  
  while (protocol != NULL)
  {
    int return_value = string_compare (protocol_name, protocol->name);
    
    if (return_value < 0)
    {
      protocol = (protocol_type *) protocol->less;
    }
    else if (return_value > 0)
    {
      protocol = (protocol_type *) protocol->more;
    }
    else
    {
      break;
    }
  }

  return protocol;
}

/* Create a new service. */

return_type service_create
  (const char *protocol_name, mailbox_id_type *mailbox_id,
   tag_type *identification)
{
  protocol_type *protocol = NULL;
  service_type *service;
  unsigned int tag_length;

  mutex_kernel_wait (&tss_tree_mutex);

  protocol = protocol_get (protocol_name);

  /* Check if this protocol exists. */

  if (protocol == NULL)
  {
    /* No. We have to allocate memory for a new node in the tree. */

    protocol = (protocol_type *)
      memory_global_allocate (sizeof (protocol_type));
    protocol->name = (char *)
      memory_global_allocate (sizeof (char) *
                              string_length (protocol_name) + 1);

    string_copy (protocol->name, protocol_name);
    protocol->less = protocol->more = NULL;
    protocol->service = NULL;
    protocol->number_of_services = 0;

    DEBUG_MESSAGE (DEBUG, "Inserting %s into tree.", protocol->name);

    /* Insert it into the tree. */

    protocol_insert (protocol);
  }

  /* Create a service structure. */

  service = memory_global_allocate (sizeof (service_type));

  /* Make sure we could get the requested memory. */

  if (service == NULL)
  {
    mutex_kernel_signal (&tss_tree_mutex);
    return STORM_RETURN_OUT_OF_MEMORY;
  }

  tag_length = tag_get_length (identification);

  protocol->number_of_services++;
  service->next = protocol->service;
  service->identification = memory_global_allocate (tag_length);
  memory_copy (service->identification, identification, tag_length);

  if (mailbox_create_kernel (&service->mailbox_id, SERVICE_MAILBOX_SIZE,
                             PROCESS_ID_NONE, CLUSTER_ID_NONE,
                             THREAD_ID_NONE) != STORM_RETURN_SUCCESS)
  {
    DEBUG_HALT ("FIXME");
  }

  *mailbox_id = service->mailbox_id;
  
  DEBUG_MESSAGE (DEBUG, "Handing out mailbox ID %u (%x) %u.",
                 service->mailbox_id, mailbox_id, *mailbox_id);

  /* Insert the node at the beginning of the list. Since everything is
     dynamic, this is not a problem. */

  protocol->service = (struct service_type *) service;
  mutex_kernel_signal (&tss_tree_mutex);
  return STORM_RETURN_SUCCESS;
}

//return_type service_get_protocol

/* Get all the services for the given protocol. */

return_type service_get (const char *protocol_name,
                         service_parameter_type *service_parameter,
                         tag_type *identification_mask)
{
  protocol_type *protocol;
  unsigned int index = 0;
  service_type *service;

  mutex_kernel_wait (&tss_tree_mutex);
  protocol = root;

  /* Make some sanity checks. */

  if (protocol_name == NULL ||
      service_parameter == NULL ||
      identification_mask == NULL)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  /* Make sure the protocol exists. */

  protocol = protocol_get (protocol_name);

  if (protocol == NULL)
  {
    DEBUG_MESSAGE (DEBUG, "STORM_RETURN_PROTOCOL_UNAVAILABLE (%s)", 
                   protocol_name);
    mutex_kernel_signal (&tss_tree_mutex);
    return STORM_RETURN_PROTOCOL_UNAVAILABLE;
  }

  /* Okay, everything seems just fine. Let's get down to business! */

  service = (service_type *) protocol->service;

  while (service != NULL)
  {
    if (tag_masked_compare (service->identification, identification_mask))
    {
      service_parameter->mailbox_id[index] = service->mailbox_id;
      index++;

      /* Make sure there is space for all those services. */

      if (index > service_parameter->max_services)
      {
        /* FIXME: This is not right. */

        service_parameter->max_services = index;
        
        DEBUG_MESSAGE (DEBUG, "STORM_RETURN_TOO_MANY_PROTOCOLS");
        mutex_kernel_signal (&tss_tree_mutex);
        return STORM_RETURN_TOO_MANY_SERVICES;
      }
    }
    service = (service_type *) service->next;
    DEBUG_MESSAGE (DEBUG, "Got mailbox_id %u for protocol %s",
                   service_parameter->mailbox_id[index], protocol_name);
  }

  service_parameter->max_services = index;
  mutex_kernel_signal (&tss_tree_mutex);
  return STORM_RETURN_SUCCESS;
}

/* Destroy a service which we no longer provide. */

return_type service_destroy (mailbox_id_type mailbox_id __attribute__ ((unused)))
{
  return STORM_RETURN_SUCCESS;
}

/* Get the number of protocols currently supported. */

return_type service_protocol_get_amount (unsigned int *number_of_protocols)
{
  *number_of_protocols = 0;
  return STORM_RETURN_SUCCESS;
}

/* This is a recursive function that is called from
   service_protocol_get. */
/* FIXME: Find a better name for this. */

static void protocol_get_all
  (service_protocol_type *protocol_info, unsigned int *index,
   protocol_type *node, unsigned int max)
{
  if (node == NULL)
  {
    return;
  }

  if (*index == max)
  {
    return;
  }

  protocol_get_all (protocol_info, index, (protocol_type *) node->less, max);
  protocol_get_all (protocol_info, index, (protocol_type *) node->more, max);
  
  string_copy_max (protocol_info[*index].name, node->name, 
                   MAX_PROTOCOL_NAME_LENGTH);
  protocol_info[*index].number_of_services = node->number_of_services;
  (*index)++;
}

/* Get a list of all the protocols currently supported. */

return_type service_protocol_get
  (unsigned int *maximum_protocols, service_protocol_type *protocol_info)
{
  unsigned int index = 0;

  protocol_get_all (protocol_info, &index, root, *maximum_protocols);
  *maximum_protocols = index;

  return STORM_RETURN_SUCCESS;
}
