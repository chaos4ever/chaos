/* $Id$ */
/* Abstract: The kernel virtual file system. In this filesystem,
   information about the kernel and the rest of the system is made
   easily available to regular user processes. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#define DEBUG FALSE

#include <storm/generic/dispatch.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <storm/generic/debug.h>
#include <storm/generic/kernelfs.h>
#include <storm/generic/log.h>
#include <storm/generic/time.h>
#include <storm/current-arch/cpuid.h>
#include <storm/current-arch/timer.h>
#include <storm/current-arch/tss.h>
#include <storm/state.h>
#include <config.h>

/* Read an entry from the kernelfs. */

return_type kernelfs_entry_read (kernelfs_generic_type *kernelfs_generic)
{
  /* Make sure we're not completely brain-damaged. */
  
  if (kernelfs_generic == NULL)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  switch (kernelfs_generic->kernelfs_class)
  {
    /* Get information on a thread. */

    case KERNELFS_CLASS_THREAD_INFO:
    {
      kernelfs_thread_info_type *thread_info =
        (kernelfs_thread_info_type *) kernelfs_generic;
      unsigned int thread = 0;
      tss_list_type *tss_node;

      mutex_kernel_wait (&tss_tree_mutex);
      
      tss_node = tss_list;

      while (tss_node != NULL)
      {
        if (tss_node->tss->process_id == thread_info->process_id)
        {
          if (thread == thread_info->thread_number)
          {            
            thread_info->thread_id = tss_node->tss->thread_id;
            string_copy (thread_info->name, tss_node->tss->thread_name);

            switch (tss_node->tss->state)
            {
              case STATE_DISPATCH:
              {
                /* FIXME: We should have similar enum names and
                   strings. */

                string_copy_max (thread_info->state, "Running",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_MAILBOX_SEND:
              {
                string_copy_max (thread_info->state,
                                 "Blocked on a mailbox (sending)",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_MAILBOX_RECEIVE:
              {
                string_copy_max (thread_info->state,
                                 "Blocked on a mailbox (receiving)",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_MUTEX_KERNEL:
              {
                string_copy_max (thread_info->state,
                                 "Blocked on a kernel mutex",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_MUTEX_USER:
              {
                string_copy_max (thread_info->state,
                                 "Blocked on a user mutex",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_IDLE:
              {
                string_copy_max (thread_info->state,
                                 "Waiting for idle timeslice",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_ZOMBIE:
              {
                string_copy_max (thread_info->state,
                                 "Zombie (countdown to extinction)",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    

              case STATE_INTERRUPT_WAIT:
              {
                string_copy_max (thread_info->state,
                                 "Waiting for hardware interrupt",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }    
              
              case STATE_SLEEP:
              {
                string_copy_max (thread_info->state,
                                 "Sleeping", MAX_STATE_NAME_LENGTH - 1);
                break;
              }

              case STATE_LOG_READ:
              {
                string_copy_max (thread_info->state,
                                 "Reading kernel log",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }

              case STATE_BLOCKED_PARENT:
              {
                string_copy_max (thread_info->state, "Blocked on parent",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }

              case STATE_BLOCKED_BOOT:
              {
                string_copy_max (thread_info->state, "Blocked during bootup",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }

              default:
              {
                string_copy_max (thread_info->state,
                                 "Unknown (kernel bug)",
                                 MAX_STATE_NAME_LENGTH - 1);
                break;
              }
            }
            break;
          }
          thread++;
        }
        
        tss_node = (tss_list_type *) tss_node->next;
      }

      mutex_kernel_signal (&tss_tree_mutex);

      break;
    }      

    /* Get verbose information on a thread. */

    case KERNELFS_CLASS_THREAD_INFO_VERBOSE:
    {
      kernelfs_thread_info_verbose_type *thread_info =
        (kernelfs_thread_info_verbose_type *) kernelfs_generic;
      unsigned int thread = 0;
      tss_list_type *tss_node;

      mutex_kernel_wait (&tss_tree_mutex);

      tss_node = tss_list;

      while (tss_node != NULL)
      {
        if (tss_node->tss->process_id == thread_info->process_id)
        {
          if (thread == thread_info->thread_number)
          {            
            //            string_copy (thread_info->process_name,
            //                         tss_node->tss->process_name);
            string_copy (thread_info->thread_name, 
                         tss_node->tss->thread_name);

            thread_info->thread_id = tss_node->tss->thread_id;
            thread_info->timeslices = tss_node->tss->timeslices;
            thread_info->main_memory = (tss_node->tss->allocated_pages *
                                        SIZE_PAGE);
            thread_info->stack_memory = (tss_node->tss->stack_pages *
                                         SIZE_PAGE);
            thread_info->instruction_pointer =
              tss_node->tss->instruction_pointer;

            /* Now, reset the timeslices counter in the TSS, so we can
               see which threads are currently getting most of the
               time. */

            tss_node->tss->timeslices = 0;
            break;
          }

          thread++;
        }

        tss_node = (tss_list_type *) tss_node->next;
      }

      mutex_kernel_signal (&tss_tree_mutex);
      
      break;
    }
    
    /* Get the amount of processes. */

    case KERNELFS_CLASS_PROCESS_AMOUNT:
    {
      u32 *processes = (u32 *) kernelfs_generic;

      *processes = number_of_processes;
      break;
    }

    /* Get information on the given process. */

    case KERNELFS_CLASS_PROCESS_INFO:
    {
      kernelfs_process_info_type *process_info = 
        (kernelfs_process_info_type *) kernelfs_generic;
      process_info_type *node = process_list;
      unsigned int counter = process_info->process_number;

      if (process_info->process_number > number_of_processes)
      {
        DEBUG_MESSAGE (DEBUG,
                       "process_info->process_number > number_of_processes");
        break;
      }

      while (counter > 0 && node != NULL)
      {
        counter--;
        node = (process_info_type *) node->next;
      }
      
      if (node == NULL)
      {
        process_info->process_id = PROCESS_ID_NONE;
      }
      else
      {
        process_info->process_id = node->process_id;
        process_info->number_of_threads = node->number_of_threads;
        string_copy_max (process_info->name, node->name, 
                         MAX_PROCESS_NAME_LENGTH);
      }
      break;
    }
    
    /* Return process information for the current thread. */

    case KERNELFS_CLASS_SELF_INFO:
    {
      kernelfs_self_type *kernelfs_self =
        (kernelfs_self_type *) kernelfs_generic;

      kernelfs_self->process_id = current_process_id;
      kernelfs_self->cluster_id = current_cluster_id;
      kernelfs_self->thread_id = current_thread_id;

      break;
    }

    /* Get information about the uptime of the system. */

    case KERNELFS_CLASS_UPTIME_INFO:
    {
      u32 *uptime_info = (u32 *) kernelfs_generic;

      *uptime_info = uptime;
      break;
    }

    /* Get information about available memory. */

    case KERNELFS_CLASS_MEMORY_INFO:
    {
      kernelfs_memory_info_type *kernelfs_memory_info =
        (kernelfs_memory_info_type *) kernelfs_generic;
      
      kernelfs_memory_info->total_memory =
        memory_physical_get_number_of_pages () * SIZE_PAGE;
      kernelfs_memory_info->free_memory = memory_physical_get_free () *
        SIZE_PAGE;
      kernelfs_memory_info->total_global_memory = SIZE_GLOBAL_HEAP;
      kernelfs_memory_info->free_global_memory = memory_global_get_free ();
      break;
    }

    /* Get the number of CPUs installed in the system. */

    case KERNELFS_CLASS_CPU_AMOUNT:
    {
      unsigned int *cpu_amount = (unsigned int *) kernelfs_generic;

      *cpu_amount = cpus;
      break;
    }

    /* Get info about the given CPU. */

    case KERNELFS_CLASS_CPU_INFO:
    {
      /* FIXME: We need to have an array/list of CPU structures. Also,
         we must decide how this should be done in an architecture
         independant manner. This can wait until we start porting
         storm... */

      kernelfs_cpu_info_type *kernelfs_cpu_info =
        (kernelfs_cpu_info_type *) kernelfs_generic;

      kernelfs_cpu_info->hz = parsed_cpu.speed;
      string_copy (kernelfs_cpu_info->name, parsed_cpu.name);
      string_copy (kernelfs_cpu_info->vendor, parsed_cpu.vendor);
      break;
    }

    /* Get info from the kernel. */

    case KERNELFS_CLASS_STORM_INFO:
    {
      kernelfs_storm_info_type *kernelfs_storm_info =
        (kernelfs_storm_info_type *) kernelfs_generic;

      string_copy_max (kernelfs_storm_info->version, PACKAGE_VERSION, 16);
      break;
    }

#if FALSE
    /* Get information about the current video mode. */

    case KERNELFS_CLASS_VIDEO_INFO:
    {
      kernelfs_video_info_type *video_info =
        (kernelfs_video_info_type *) kernelfs_generic;
      
      video_info->mode_type = multiboot_info.mode_type;
      video_info->width = multiboot_info.width;
      video_info->height = multiboot_info.height;
      video_info->depth = multiboot_info.depth;
      video_info->frame_buffer_address =
        (void *) multiboot_info.frame_buffer_address;

      debug_print ("%x %x\n", video_info->frame_buffer_address,
                   BASE_PROCESS_PAGETABLES);
      break;
    }
#endif

    /* Get the number of allocated port ranges. */

    case KERNELFS_CLASS_PORT_AMOUNT:
    {
      unsigned int *ports = (unsigned int *) kernelfs_generic;
      port_range_type *port = port_list;

      *ports = 0;

      while (port != NULL)
      {
        port = (port_range_type *) port->next;
        (*ports)++;
      }

      break;
    }

    /* Get information about the given port range. */

    case KERNELFS_CLASS_PORT_INFO:
    {
      kernelfs_port_info_type *port_info = 
        (kernelfs_port_info_type *) kernelfs_generic;
      port_range_type *port = port_list;
      unsigned int index = 0;

      while (port != NULL && index < port_info->port)
      {
        port = (port_range_type *) port->next;
        index++;
      }

      /* Did we get it? */

      if (port == NULL)
      {
        return STORM_RETURN_INVALID_ARGUMENT;
      }

      port_info->base = port->start;
      port_info->length = port->length;
      port_info->process_id = port->process_id;
      port_info->cluster_id = port->cluster_id;
      port_info->thread_id = port->thread_id;
      string_copy_max (port_info->description, port->description, 128);

      break;
    }
    
    /* Get information about registered IRQ levels in the system. */

    case KERNELFS_CLASS_IRQ_AMOUNT:
    {
      unsigned int *irqs = (unsigned int *) kernelfs_generic;
      unsigned int index;

      *irqs = 0;

      for (index = 0; index < IRQ_LEVELS; index++)
      {
        if (irq[index].allocated)
        {
          (*irqs)++;
        }
      }

      break;
    }

    /* Get information about the given IRQ level. */

    case KERNELFS_CLASS_IRQ_INFO:
    {
      kernelfs_irq_info_type *kernelfs_irq_info =
        (kernelfs_irq_info_type *) kernelfs_generic;
      unsigned int index;
      unsigned int which = 0;

      /* Perform a basic range check. */

      if (which >= IRQ_LEVELS)
      {
        return STORM_RETURN_INVALID_ARGUMENT;
      }
      
      for (index = 0; index < IRQ_LEVELS; index++)
      {
        if (irq[index].allocated)
        {
          which++;
        }

        if (which > kernelfs_irq_info->which)
        {
          break;
        }
      }

      if (index == IRQ_LEVELS)
      {
        return STORM_RETURN_INVALID_ARGUMENT;
      }
      
      kernelfs_irq_info->level = index;
      kernelfs_irq_info->process_id = irq[index].process_id;
      kernelfs_irq_info->cluster_id = irq[index].cluster_id;
      kernelfs_irq_info->thread_id = irq[index].thread_id;
      kernelfs_irq_info->occurred = irq[index].occurred;
      string_copy (kernelfs_irq_info->description, irq[index].description);
            
      break;
    }

    /* Try to read a log entry from the kernel log. If there is
       nothing to be read and the block flag is set, this will block
       the reader. */

    case KERNELFS_CLASS_LOG_READ:
    {
      kernelfs_log_type *kernelfs_log =
        (kernelfs_log_type *) kernelfs_generic;
      log_list_type *old_log_list;

      /* If there is nothing in the list. */

      if (log_list == NULL)
      {
        if (kernelfs_log->block)
        {
          current_tss->state = STATE_LOG_READ;
          dispatch_next ();
        }
        else
        {
          kernelfs_log->string = NULL;
          break;
        }
      }

      /* There is a log entry in the list. */

      string_copy_max (kernelfs_log->string,
                       log_list->string,
                       kernelfs_log->max_string_length);

      old_log_list = log_list;
      log_list = (log_list_type *) log_list->next;

      memory_global_deallocate (old_log_list);

      break;
    }
    
    /* Read the current chaos time. */

    case KERNELFS_CLASS_TIME_READ:
    {
      kernelfs_time_type *kernelfs_time =
        (kernelfs_time_type *) kernelfs_generic;
      
      kernelfs_time->time = time;
      break;
    }

    /* Anything else is undefined. */

    default:
    {
      return STORM_RETURN_INVALID_ARGUMENT;
    }
  }

  return STORM_RETURN_SUCCESS;
}
