// Abstract: Thread functions.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <storm/generic/mutex.h>
#include <storm/generic/types.h>

#include <storm/thread.h>
#include <storm/current-arch/thread.h>

// External variables.
extern volatile uint32_t number_of_tasks;
extern mutex_kernel_type tss_tree_mutex;
extern tss_list_type *tss_list;
extern tss_list_type *idle_tss_node;

// Function prototypes.
extern void thread_init(void);
extern thread_id_type thread_get_free_id(void);
extern return_type thread_control(thread_id_type thread_id, unsigned int class, unsigned int parameter);
extern return_type thread_create(uint32_t current_thread_esp, void *(*start_routine)(void *), void *argument);
extern storm_tss_type *thread_get_tss(thread_id_type thread_id);
extern tss_list_type *thread_link(storm_tss_type *tss);
extern void thread_unlink(thread_id_type thread_id);
extern tss_list_type *thread_link_list(tss_list_type **list, storm_tss_type *tss);
extern void thread_unlink_list(tss_list_type **list, thread_id_type thread_id);
extern void thread_block_kernel_mutex(storm_tss_type *tss, mutex_kernel_type *mutex_kernel);
extern return_type thread_unblock_kernel_mutex(mutex_kernel_type *mutex_kernel);
extern void thread_block_mailbox_send(storm_tss_type *tss, mailbox_id_type mailbox_id);
extern void thread_block_mailbox_receive(storm_tss_type *tss, mailbox_id_type mailbox_id);
extern void thread_unblock_mailbox_send(mailbox_id_type mailbox_id);
extern void thread_unblock_mailbox_receive(mailbox_id_type mailbox_id);
