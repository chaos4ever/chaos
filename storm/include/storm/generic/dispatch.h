// Abstract: Functions for the task dispatcher.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <storm/current-arch/tss.h>
#include <storm/generic/types.h>

extern void dispatch_next(void);
extern int dispatch_update(void);
extern void dispatch(void);
extern void dispatch_task_switcher(void) NORETURN;
extern void dispatch_init(void);

extern volatile process_id_type current_process_id;
extern volatile cluster_id_type current_cluster_id;
extern volatile thread_id_type current_thread_id;
extern volatile time_type timeslice;
extern storm_tss_type *current_tss;
extern volatile u8 dispatch_task_flag;
extern tss_list_type *current_tss_node;
