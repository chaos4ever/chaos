/* $Id$ */
/* Abstract: Thread functions. */
/* Authors: Henrik Hallin <hal@chaosdev.org>,
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

#ifndef __STORM_GENERIC_THREAD_H__
#define __STORM_GENERIC_THREAD_H__

#include <storm/generic/mutex.h>
#include <storm/generic/types.h>

#include <storm/thread.h>
#include <storm/current-arch/thread.h>

/* External variables. */

extern volatile u32 number_of_tasks;
extern mutex_kernel_type tss_tree_mutex;
extern tss_list_type *tss_list;
extern tss_list_type *idle_tss_node;

/* Prototypes. */

extern void thread_init (void);
extern thread_id_type thread_get_free_id (void);
extern return_type thread_control (thread_id_type thread_id,
                                   unsigned int class,
                                   unsigned int parameter);
extern return_type thread_create (void);
extern storm_tss_type *thread_get_tss (thread_id_type thread_id);
extern tss_list_type *thread_link (storm_tss_type *tss);
extern void thread_unlink (thread_id_type thread_id);
extern tss_list_type *thread_link_list (tss_list_type **list,
                                        storm_tss_type *tss);
extern void thread_unlink_list (tss_list_type **list,
                                thread_id_type thread_id);
extern void thread_block_kernel_mutex (storm_tss_type *tss,
                                       mutex_kernel_type *mutex_kernel);
extern return_type thread_unblock_kernel_mutex (mutex_kernel_type
                                                *mutex_kernel);

extern void thread_block_mailbox_send (storm_tss_type *tss,
                                       mailbox_id_type mailbox_id);
extern void thread_block_mailbox_receive (storm_tss_type *tss,
                                          mailbox_id_type mailbox_id);

extern void thread_unblock_mailbox_send (mailbox_id_type mailbox_id);
extern void thread_unblock_mailbox_receive (mailbox_id_type mailbox_id);

#endif /* !__STORM_GENERIC_THREAD_H__ */
