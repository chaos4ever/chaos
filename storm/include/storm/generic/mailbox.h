/* $Id$ */
/* Abstract: Function prototypes for the mailbox IPC system. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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
   USA */

#ifndef __STORM_GENERIC_MAILBOX_H__
#define __STORM_GENERIC_MAILBOX_H__

#include <storm/mailbox.h>
#include <storm/generic/defines.h>
#include <storm/generic/mutex.h>
#include <storm/generic/types.h>

/* Type definitions. */

/* This defines the structure for messages inside mailboxes. */

typedef struct
{
  process_id_type sender_process_id;
  cluster_id_type sender_cluster_id;
  thread_id_type sender_thread_id;

  unsigned int protocol;
  unsigned int class;

  struct message_type *next;

  unsigned int length;

  u8 data[0];
} message_type;

/* The format of a mailbox. */

typedef struct
{
  /* Mailbox id. */

  mailbox_id_type id;

  /* Who owns the mailbox. */

  process_id_type owner_process_id;
  cluster_id_type owner_cluster_id;
  thread_id_type owner_thread_id;

  /* Who is allowed to send to the mailbox. */

  process_id_type user_process_id;
  cluster_id_type user_cluster_id;
  thread_id_type user_thread_id;

  /* Size of the mailbox. */

  unsigned int total_size;

  /* Size of the free part of the mailbox. */

  unsigned int free_size;

  /* Size of the message the blocked sender is awaiting to deliver. */

  unsigned int blocked_size;

  /* Number of messages in the mailbox. */

  unsigned int number_of_messages;

  /* Is the owner of this mailbox blocked on reading? */

  bool reader_blocked;

  /* Start of the first message in the mailbox. */

  message_type *first_message;
  message_type *last_message;

  /* Tree nodes. */

  struct mailbox_type *next;
} __attribute__ ((packed)) mailbox_type;

/* Function prototypes. */

extern return_type mailbox_create
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id);

extern return_type mailbox_destroy (mailbox_id_type mailbox_id);

extern return_type mailbox_flush (mailbox_id_type mailbox_id);

extern return_type mailbox_send
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter);

extern return_type mailbox_receive
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter);

extern void mailbox_init (void) INIT_CODE;

extern return_type mailbox_create_kernel
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id);

#endif /* !__STORM_GENERIC_MAILBOX_H__ */
