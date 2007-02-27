/* $Id$ */
/* Abstract: Mailbox functions for the LPC (local process
   communication). */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Anders Öhrt <doa@chaosdev.org> 
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

/* Define this as TRUE if you are debugging this module. */

#define DEBUG           TRUE

/* Extra verbose debugging. This will make the system pretty unusable,
   so be warned. */

#define VERBOSE_DEBUG   FALSE

#include <storm/state.h>
#include <storm/generic/mailbox.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/return_values.h>
#include <storm/generic/thread.h>
#include <storm/current-arch/tss.h>

//static mutex_kernel_type mailbox_mutex = MUTEX_UNLOCKED;
//static mutex_kernel_type hash_mutex = MUTEX_UNLOCKED;
//static mutex_kernel_type free_id_mutex = MUTEX_UNLOCKED;

/* The mailbox hash is an array of pointers. */

static mailbox_type **mailbox_hash_table;
static mailbox_id_type free_mailbox_id = 1;

/* Gets the hash value for the given mailbox ID. */

static int hash (mailbox_id_type mailbox_id)
{
  return (mailbox_id % limit_mailbox_hash_entries);
}

/* The init function for the mailbox system. */

void mailbox_init (void)
{
  /* Allocate memory for the hash table. */

  mailbox_hash_table = (mailbox_type **) memory_global_allocate
    (sizeof (mailbox_type *) * limit_mailbox_hash_entries);

  DEBUG_MESSAGE (VERBOSE_DEBUG, "mailbox_hash_table = %x, size = %u",
                 mailbox_hash_table, sizeof (mailbox_type *) *
                 limit_mailbox_hash_entries);

  /* Initially set all pointers to NULL. This isn't entirely nice, but
     doing this as a for loop wouldn't even be close to cool. */

  memory_set_u8 ((u8 *) mailbox_hash_table, 0,
                 limit_mailbox_hash_entries * sizeof (mailbox_type *));
}

/* Looks the given mailbox ID up in the hash table. */ 

static mailbox_type *mailbox_find (mailbox_id_type mailbox_id)
{
  mailbox_type *node;

  node = mailbox_hash_table[hash (mailbox_id)];

  DEBUG_MESSAGE (VERBOSE_DEBUG, "node = %x", node);

  while (node != NULL && node->id != mailbox_id)
  {
    node = (mailbox_type *) node->next;
    DEBUG_MESSAGE (VERBOSE_DEBUG, "node = %x", node);
  }

  return node;
}

/* Get the first available mailbox id. */

static mailbox_id_type mailbox_get_free_id (void)
{
  mailbox_id_type id;

  id = free_mailbox_id;
  free_mailbox_id++;

  return id;
}

/* Link in a new mailbox in the mailbox hash table. Returns TRUE on
   successful linking of the mailbox, or FALSE otherwise. */

static bool mailbox_link (mailbox_type *new_mailbox)
{
  int index;

  index = hash (new_mailbox->id);

  new_mailbox->next = (struct mailbox_type *) mailbox_hash_table[index];
  mailbox_hash_table[index] = new_mailbox;

  return TRUE;
}

/* Unlink a mailbox with a certain id. Returns TRUE on success, and
   FALSE otherwise. */

static bool mailbox_unlink (mailbox_id_type mailbox_id)
{
  int index;
  mailbox_type *mailbox, *previous_mailbox;

  index = hash (mailbox_id);
  
  mailbox = mailbox_hash_table[index];

  if (mailbox->id == mailbox_id)
  {
    mailbox_hash_table[index] = (mailbox_type *) mailbox->next;
    memory_global_deallocate (mailbox);
    return TRUE;
  }
  else
  {
    while (mailbox != NULL && mailbox->id != mailbox_id)
    {
      previous_mailbox = mailbox;
      mailbox = (mailbox_type *) mailbox->next;
    }
    if (mailbox == NULL)
    {
      return FALSE;
    }
    else
    {
      previous_mailbox->next = mailbox->next;
      memory_global_deallocate (mailbox);
      return TRUE;
    }
  }
}

/* Create a new mailbox without locking the dispatcher (presuming it
   has already been locked). This function can only be called from
   within the kernel. */

return_type mailbox_create_kernel
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id)
{
  mailbox_type *new_mailbox;

  DEBUG_MESSAGE (VERBOSE_DEBUG, "Called");

  new_mailbox = (mailbox_type *)
    memory_global_allocate (sizeof (mailbox_type));

  new_mailbox->owner_process_id = current_process_id;
  new_mailbox->owner_cluster_id = current_cluster_id;
  new_mailbox->owner_thread_id = current_thread_id;
  
  new_mailbox->user_process_id = user_process_id;
  new_mailbox->user_cluster_id = user_cluster_id;
  new_mailbox->user_thread_id = user_thread_id;
  
  /* FIXME: Check for allowed limits. */
  
  new_mailbox->total_size = size;
  new_mailbox->free_size = size;
  new_mailbox->number_of_messages = 0;
  new_mailbox->blocked_size = 0;

  new_mailbox->first_message = NULL;
  new_mailbox->last_message = NULL;

  new_mailbox->reader_blocked = FALSE;
  
  new_mailbox->next = NULL;
  
  new_mailbox->id = *mailbox_id = mailbox_get_free_id ();
  
  mailbox_link (new_mailbox);
  
  return STORM_RETURN_SUCCESS;
}

/* Create a new mailbox. */

return_type mailbox_create
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id)
{
  return_type return_value;

  mutex_kernel_wait (&tss_tree_mutex);
  return_value = mailbox_create_kernel (mailbox_id, size,
                                        user_process_id, user_cluster_id,
                                        user_thread_id);
  mutex_kernel_signal (&tss_tree_mutex);

  return return_value;
}

/* Destroy a mailbox, using some common explosive. */

return_type mailbox_destroy (mailbox_id_type mailbox_id)
{
  mailbox_type *mailbox;

  mutex_kernel_wait (&tss_tree_mutex);

  mailbox = mailbox_find (mailbox_id);

  //  debug_print ("%s: destroying %u\n", __FUNCTION__, mailbox_id);

  if (mailbox == NULL)
  {
    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_MAILBOX_UNAVAILABLE;
  }

  if (current_process_id == mailbox->owner_process_id &&
      current_cluster_id == mailbox->owner_cluster_id &&
      current_thread_id == mailbox->owner_thread_id)
  {
    /* We are allowed to destroy this mailbox. */

    mailbox_unlink (mailbox_id);
    
    //    tree_dump (root);
    
    //    debug_print ("root: %x\n", root);
    
    /* No need to close the mailbox, since it is destroyed. */

    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_SUCCESS;
  }
  else
  {
    debug_print ("Segmentation fault.\n");
    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_ACCESS_DENIED;
  }
}

/* Delete all queued messages in the mailbox. */
/* FIXME: Write this code. */

return_type mailbox_flush (mailbox_id_type mailbox_id)
{
  mailbox_id = mailbox_id;

  return STORM_RETURN_SUCCESS;
}

/* Post a message in a mailbox. */
/* FIXME: Is the dude allowed to send to OUR mailbox?! */

return_type mailbox_send (mailbox_id_type mailbox_id,
                          message_parameter_type *message_parameter)
{
  message_type *message;
  mailbox_type *mailbox;

  /* Perform some sanity checking on the input parameters. */

  if (message_parameter == NULL || (message_parameter->data == NULL &&
                                    message_parameter->length > 0))
  {
    DEBUG_SDB (DEBUG, "Invalid argument.");
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  mutex_kernel_wait (&tss_tree_mutex);

  DEBUG_MESSAGE (VERBOSE_DEBUG,
                 "Called (message_parameter = %p, data = %p, protocol = %u, class = %u, id = %u)",
                 message_parameter, message_parameter->data,
                 message_parameter->protocol, message_parameter->message_class,
                 mailbox_id);

  mailbox = mailbox_find (mailbox_id);

  if (mailbox == NULL)
  {
    mutex_kernel_signal (&tss_tree_mutex);
    DEBUG_SDB (DEBUG, "mailbox == NULL.");

    return STORM_RETURN_MAILBOX_UNAVAILABLE;
  }

  /* If the message won't ever fit into this mailbox, fail. */

  if (message_parameter->length + sizeof (message_parameter_type) >
      mailbox->total_size)
  {
    mutex_kernel_signal (&tss_tree_mutex);
    DEBUG_MESSAGE (DEBUG,
                   "Message was larger than the mailbox! (%u > %u)",
                   message_parameter->length + sizeof (message_parameter_type),
                   mailbox->total_size);

    return STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE;
  }

  /* Block or return, depending on how we was called. */

  if (message_parameter->length + sizeof (message_parameter_type) >
      mailbox->free_size)
  {
    if (message_parameter->block)
    {
      /* Block until receiver reads messages so that there is room for
         us. */

      current_tss->mailbox_id = mailbox_id;
      current_tss->mutex_time = timeslice;
      mailbox->blocked_size = (message_parameter->length + 
                               sizeof (message_parameter_type));
      current_tss->state = STATE_MAILBOX_SEND;
      mutex_kernel_signal (&tss_tree_mutex);
      dispatch_next ();
    }
    else
    {
      mutex_kernel_signal (&tss_tree_mutex);
      DEBUG_SDB (DEBUG, "Mailbox was full");

      return STORM_RETURN_MAILBOX_FULL;
    }
  }

  /* When we come here, we are guaranteed to send to the mailbox. */

  DEBUG_MESSAGE (VERBOSE_DEBUG, "Delivering...");

  message = memory_global_allocate (sizeof (message_type) +
                                    message_parameter->length);

  DEBUG_MESSAGE (VERBOSE_DEBUG, "Got %p (%u bytes)", message,
                 sizeof (message_type) + message_parameter->length);
  message->sender_process_id = current_process_id;
  message->sender_cluster_id = current_cluster_id;
  message->sender_thread_id = current_thread_id;

  message->protocol = message_parameter->protocol;
  message->class = message_parameter->message_class;

  message->next = NULL;

  message->length = message_parameter->length;
  memory_copy (message->data, message_parameter->data,
               message_parameter->length);

  /* Add the message to the linked list. */

  if (mailbox->last_message != NULL)
  {
    mailbox->last_message->next = (struct message_type *) message;
  }

  mailbox->last_message = message;

  if (mailbox->number_of_messages == 0)
  {
    mailbox->first_message = message;
  }

  mailbox->number_of_messages++;
  mailbox->free_size -= (message_parameter->length + 
                         sizeof (message_parameter_type));

  /* If someone is blocked on this mailbox, unblock her. */

  if (mailbox->reader_blocked)
  {
    DEBUG_MESSAGE (VERBOSE_DEBUG,
                   "mailbox_id = %u, mailbox->messages = %u, mailbox->first_message = %x", 
                   mailbox_id, mailbox->number_of_messages,
                   mailbox->first_message);
    DEBUG_MESSAGE (VERBOSE_DEBUG, "Unblocking...");
    thread_unblock_mailbox_receive (mailbox_id);
    DEBUG_MESSAGE (VERBOSE_DEBUG, "Done.");
  }

  mutex_kernel_signal (&tss_tree_mutex);

  return STORM_RETURN_SUCCESS;
}

/* Receive a message from the mailbox. */

return_type mailbox_receive (mailbox_id_type mailbox_id,
                             message_parameter_type *message_parameter)
{
  mailbox_type *mailbox;
  message_type *temporary;

  mutex_kernel_wait (&tss_tree_mutex);
  DEBUG_MESSAGE (VERBOSE_DEBUG, "Called (id %u).", mailbox_id);
  mailbox = mailbox_find (mailbox_id);

  if (message_parameter == NULL)
  {
    DEBUG_SDB (DEBUG, "message_parameter == NULL");
    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_INVALID_ARGUMENT;
  }

  /* When we get here, we are allowed to access the mailbox. */

  if (mailbox == NULL)
  {
    mutex_kernel_signal (&tss_tree_mutex);
    DEBUG_HALT ("Mailbox was NULL");
    return STORM_RETURN_MAILBOX_UNAVAILABLE;
  }

  if (!(current_process_id == mailbox->owner_process_id &&
        current_cluster_id == mailbox->owner_cluster_id/* &&
        current_thread_id == mailbox->owner_thread_id*/))
  {
    /* We don't have read-access to this mailbox, since we are not the
       owner. */

    DEBUG_MESSAGE (DEBUG, "Access denied for process/thread %u/%u (should have been %u/%u) mailbox ID %u",
                   current_process_id, current_thread_id, 
                   mailbox->owner_process_id, mailbox->owner_thread_id,
                   mailbox_id);
    mutex_kernel_signal (&tss_tree_mutex);

    while (TRUE);
    return STORM_RETURN_ACCESS_DENIED;
  }

  /* If the mailbox is empty, block or return. */

  if (mailbox->number_of_messages == 0)
  {
    if (message_parameter->block)
    {
      /* Block ourselves until the mailbox gets populated. */

      mailbox->reader_blocked = TRUE;

      DEBUG_MESSAGE (VERBOSE_DEBUG, "Blocking ourselves.");

      mutex_kernel_signal (&tss_tree_mutex);

      /* Modify this task's entry in the TSS structure. */
    
      mutex_kernel_wait (&tss_tree_mutex);
      current_tss->state = STATE_MAILBOX_RECEIVE;
      current_tss->mailbox_id = mailbox_id;
      current_tss->mutex_time = timeslice;
      mutex_kernel_signal (&tss_tree_mutex);

      dispatch_next ();

      mutex_kernel_wait (&tss_tree_mutex);
      mailbox->reader_blocked = FALSE;

      /* A message has arrived. We open the mailbox again, so that we
         can read out the message. */
      
      DEBUG_MESSAGE (VERBOSE_DEBUG,
                     "mailbox_id = %u, mailbox->messages = %u, mailbox->first_message = %x", 
                     mailbox_id, mailbox->number_of_messages,
                     mailbox->first_message);
    }
    else
    {
      /* There was no mail, return. */

      mutex_kernel_signal (&tss_tree_mutex);

      return STORM_RETURN_MAILBOX_EMPTY;
    }
  }

  /* Receive the message. */
  /* FIXME: Allow conditional reception. */
  
  if (message_parameter->length >= mailbox->first_message->length)
  {
    /* This one fits into our bag. */
    
    message_parameter->protocol = mailbox->first_message->protocol;
    message_parameter->message_class = mailbox->first_message->class;
    message_parameter->length = mailbox->first_message->length;
    
    mailbox->free_size += (mailbox->first_message->length + 
                           sizeof (message_parameter_type));

    memory_copy (message_parameter->data, mailbox->first_message->data,
                 mailbox->first_message->length);
    temporary = mailbox->first_message;

    mailbox->first_message = (message_type *) mailbox->first_message->next;

    if (mailbox->first_message == NULL)
    {
      mailbox->last_message = NULL;
    }

    memory_global_deallocate (temporary);

    mailbox->number_of_messages--;

    /* Check if we can unblock our boy. */

    if (mailbox->blocked_size != 0 &&
        mailbox->blocked_size < mailbox->free_size)
    {
      thread_unblock_mailbox_send (mailbox_id);
      mailbox->blocked_size = 0;
    }

    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_SUCCESS;
  }
  else
  {
    /* Need more space. */

    DEBUG_MESSAGE (DEBUG, "%x < %x", 
                   message_parameter->length, mailbox->first_message->length);

    message_parameter->length = mailbox->first_message->length;
    mutex_kernel_signal (&tss_tree_mutex);

    return STORM_RETURN_MAILBOX_MESSAGE_TOO_LARGE;
  }
}
