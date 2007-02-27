/* $Id$ */
/* Abstract: Process management. Responsible for starting and stopping
   processes and threads. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
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

/* FIXME: Most of this should be put into the generic subdirectory. */

/* Define as TRUE for lots of debug info. */

#define DEBUG FALSE

#include <storm/state.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/process.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <storm/ia32/flags.h>
#include <storm/ia32/gdt.h>
#include <storm/ia32/trap.h>
#include <storm/ia32/tss.h>
#include <config.h>

static process_id_type free_process_id = 1;
static mutex_kernel_type process_id_mutex = MUTEX_UNLOCKED;
static tss_list_type kernel_tss_list;
static process_info_type kernel_process_info;

/* Tree used for holding information about the processes in the
   system. process_info_kernel is the root node, which points at the
   idle process. */

mutex_kernel_type process_tree_mutex = MUTEX_UNLOCKED;
process_info_type *process_list = NULL;

/* Only the first process will run. */

static bool first_process = TRUE;

/* The number of processes currently running in the system. */

unsigned int number_of_processes = 1;

/* A list of processes to wake up when a child to the kernel invokes
   process_parent_unblock (). This is used for a smooth startup
   sequence. */

kernel_child_list_type *kernel_child_list = NULL;

/* The 'unnamed' state. */

static const char *unnamed = "unnamed";

/* Find the given process in the tree. */

process_info_type *process_find (process_id_type process_id)
{
  process_info_type *list = process_list;

  while (list != NULL)
  {
    if (list->process_id == process_id)
    {
      return list;
    }
    list = (process_info_type *) list->next;
  }

  DEBUG_HALT ("Process not found.");
  return NULL;
}

/* Adds the given process to the process tree. */

void process_link (process_info_type *process_info)
{
  process_info->next = (struct process_info_type *) process_list;
  process_list = process_info;
}

/* Initialise process support. */

void process_init (void)
{
  /* Set up the kernel tss. */

  kernel_tss = (storm_tss_type *) BASE_VIRTUAL_KERNEL_TSS;

  /* Insert the kernel TSS into the structures. It is used for the idle
     thread. */

  kernel_tss->state = STATE_IDLE;
  kernel_tss->process_id = PROCESS_ID_KERNEL;
  kernel_tss->thread_id = THREAD_ID_KERNEL;
  kernel_tss->cr3 = (u32) kernel_page_directory;
  thread_link (kernel_tss);
  number_of_tasks++;

  string_copy (current_tss->thread_name, "Starting up");

  kernel_tss_list.tss = kernel_tss;
  kernel_tss_list.thread_id = THREAD_ID_KERNEL;
  kernel_tss_list.previous = kernel_tss_list.next = NULL;

  kernel_process_info.process_id = PROCESS_ID_KERNEL;
  kernel_process_info.thread_list = &kernel_tss_list;
  kernel_process_info.name = PACKAGE_NAME " " PACKAGE_VERSION;
  kernel_process_info.number_of_threads = 1;
  process_link (&kernel_process_info);
  kernel_tss->process_info = (struct process_info_type *) &kernel_process_info;
}

/* Get the first available process_id. */

static process_id_type process_get_free_id (void)
{
  process_id_type process_id;

  mutex_kernel_wait (&process_id_mutex);
  process_id = free_process_id++;
  mutex_kernel_signal (&process_id_mutex);

  return process_id;
}

/* Create a new process. */
/* FIXME: More error checking (especially memory related stuff). */
/* FIXME: Split this function into several, if possible. It's far too
   long right now. */
/* FIXME: Reimplement the parameter passing (in
   process_data->parameter_string) */

return_type process_create (process_create_type *process_data)
{
  storm_tss_type *process_tss;
  unsigned page_directory_page;
  unsigned counter;
  page_directory_entry_page_table *page_directory =
    (page_directory_entry_page_table *) BASE_PROCESS_TEMPORARY;
  u32 code_base, data_base = 0;
  u32 avl_intro_base, avl_array_base;
  avl_header_type *new_avl_header;
  unsigned int process_avl_intro_pages, process_avl_array_pages;
  u32 physical_page;
  process_info_type *process_info;

  /* Make sure all sections take up some space. */

  if (process_data->code_section_size == 0 ||
      process_data->data_section_size == 0)
  {
    DEBUG_MESSAGE (TRUE, "process_data->code_section_size == 0 || process_data->data_section_size == 0");
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  /* Check if the code section is within allowed limits. */
  
  if (process_data->code_section_base < BASE_PROCESS_SPACE_START ||
      (process_data->code_section_base + 
       process_data->code_section_size) >= BASE_PROCESS_SPACE_END)
  {
    return STORM_RETURN_SECTION_MISPLACED;
  }

  /* Check if the code section is within allowed limits. */
  
  if (process_data->data_section_base < BASE_PROCESS_SPACE_START ||
      (process_data->data_section_base + 
       process_data->data_section_size) >= BASE_PROCESS_SPACE_END)
  {
    return STORM_RETURN_SECTION_MISPLACED;
  }

  /* Make sure all sections are page aligned. */

  if (process_data->code_section_base % SIZE_PAGE != 0 ||
      process_data->data_section_base % SIZE_PAGE != 0)
  {
    return STORM_RETURN_UNALIGNED_SECTION;
  }

  /* Get the next PROCESS_ID. */

  *process_data->process_id = process_get_free_id ();

  /* Create a TSS for the new process. This must be done before we
     set up the page directory, so we can set the CR3. (used by
     memory_virtual_map_other) */
  
  DEBUG_MESSAGE (DEBUG, "Allocating memory for the process TSS.");

  process_tss = (storm_tss_type *)
    memory_global_allocate (sizeof (storm_tss_type));

  /* Zap it. */
  
  memory_set_u8 ((u8 *) process_tss, 0, sizeof (storm_tss_type));
  
  /* Add this thread to the TSS structures. */
 
  mutex_kernel_wait (&tss_tree_mutex);
  process_tss->thread_id = thread_get_free_id ();
  thread_link (process_tss);
  process_tss->state = STATE_UNREADY;
  process_tss->initialised = FALSE;
  
  mutex_kernel_signal (&tss_tree_mutex);

  /* Set up a page directory for the new task. */

  DEBUG_MESSAGE (DEBUG, "Setting up page_directory.");

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Page directory");

  page_directory_page = physical_page;

  process_tss->allocated_pages = 1;
  process_tss->cr3 = page_directory_page * SIZE_PAGE;

  /* Map it for ourselves so that we can clear it out. */

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY),
                      page_directory_page, 1, PAGE_KERNEL);

  memory_set_u8 ((u8 *) BASE_PROCESS_TEMPORARY, 0, SIZE_PAGE);

  /* Map it for the new task. */

  memory_virtual_map_other (process_tss, 
                            GET_PAGE_NUMBER (BASE_PROCESS_PAGE_DIRECTORY),
                            page_directory_page, 1, PAGE_KERNEL);

  /* Map GDT and IDT. */

  memory_virtual_map_other (process_tss, GET_PAGE_NUMBER (BASE_GDT),
                            GET_PAGE_NUMBER (BASE_GDT), 1, PAGE_KERNEL |
                            PAGE_GLOBAL);

  /* Map kernel. */
    
  memory_virtual_map_other (process_tss, GET_PAGE_NUMBER (BASE_KERNEL),
                            GET_PAGE_NUMBER (BASE_KERNEL),
                            SIZE_IN_PAGES ((u32) &_end - BASE_KERNEL),
                            PAGE_KERNEL | PAGE_GLOBAL);

  /* Map kernel TSS. */

  memory_virtual_map_other (process_tss,
                            GET_PAGE_NUMBER (BASE_VIRTUAL_KERNEL_TSS),
                            GET_PAGE_NUMBER (BASE_KERNEL_TSS),
                            1, PAGE_KERNEL | PAGE_GLOBAL);

  /* Map trap handlers' TSS:es. */

  memory_virtual_map_other (process_tss,
                            GET_PAGE_NUMBER (BASE_PROCESS_TRAP_TSS),
                            GET_PAGE_NUMBER (trap_tss),
                            SIZE_IN_PAGES (SIZE_TRAP_TSS),
                            PAGE_KERNEL | PAGE_GLOBAL);

  /* Allocate the necessary pages for the stack segments and map the
     stack. */
  
  /* FIXME: this should not be hardcoded but calculated from
     defines. There were some problems with the size being very large,
     though... */

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Process stack.");

  memory_virtual_map_other (process_tss, 0x100000 - 1, physical_page, 1,
                            PAGE_WRITABLE | PAGE_NON_PRIVILEGED);
    
  /* ...and do the same for the PL0 stack. */

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "PL0 stack.");
    
  memory_virtual_map_other (process_tss, 0x100000 - (16 * 1024),
                            physical_page, 1, PAGE_KERNEL);
  process_tss->allocated_pages += 2;

  /* Map video memory, so the kernel can print debug messages. */
  
  memory_virtual_map_other (process_tss, GET_PAGE_NUMBER (BASE_SCREEN),
                            GET_PAGE_NUMBER (BASE_SCREEN),
                            16, PAGE_WRITABLE | PAGE_CACHE_DISABLE);

  /* FIXME: Reserve the memory our code, data and BSS sections use in
     the virtual addressing space. */  

  /* FIXME: Support larger sections than one meg. Should be perfectly
     enough for now though... but will it do it for Quake?? */
  
  /* Now we have to move over the code and data to the new process'
     address space. Start by mapping it. */

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page,
                            (SIZE_IN_PAGES (process_data->code_section_size)),
                            "Process code.");

  DEBUG_MESSAGE (DEBUG, "Allocating %u pages for the code section (%u bytes)",
                 SIZE_IN_PAGES (process_data->code_section_size),
                 process_data->code_section_size);
  code_base = physical_page;
  process_tss->allocated_pages +=
    SIZE_IN_PAGES (process_data->code_section_size);

  DEBUG_MESSAGE (DEBUG, "Allocating %u pages for the data section",
                 SIZE_IN_PAGES (process_data->data_section_size +
                                process_data->bss_section_size));
  DEBUG_MESSAGE (DEBUG, "Data section size: %u\n",
                 process_data->data_section_size +
                 process_data->bss_section_size);
  DEBUG_MESSAGE (DEBUG, "In pages: %u\n", 
                 SIZE_IN_PAGES (process_data->data_section_size +
                                process_data->bss_section_size));

  if (SIZE_IN_PAGES (process_data->data_section_size +
                     process_data->bss_section_size) != 0)
  {
    /* FIXME: Check return value. */

    memory_physical_allocate (&physical_page, (SIZE_IN_PAGES (
      process_data->bss_section_base +
      process_data->bss_section_size -
      process_data->data_section_base)), "Process data.");
  
    data_base = physical_page;
    process_tss->allocated_pages +=
      (SIZE_IN_PAGES (process_data->bss_section_base +
                      process_data->bss_section_size -
                      process_data->data_section_base));
  }    

  /* Map the code section temporarily in our addressing space, and
     copy the code to there. */
  
  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_CREATE), code_base,
                      SIZE_IN_PAGES (process_data->code_section_size),
                      PAGE_KERNEL);

  memory_copy ((void *) BASE_PROCESS_CREATE,
               process_data->code_section_address,
               process_data->code_section_size);

  if (process_data->code_section_size % SIZE_PAGE != 0)
  {
    memory_set_u8 ((u8 *) (BASE_PROCESS_CREATE +
                           process_data->code_section_size), 0,
                   SIZE_PAGE - (process_data->code_section_size % SIZE_PAGE));
  }

  memory_virtual_map_other (process_tss,
                            GET_PAGE_NUMBER (process_data->code_section_base),
                            code_base,
                            SIZE_IN_PAGES (process_data->code_section_size),
                            PAGE_NON_PRIVILEGED);

  /* Do the same for the data segment. */
  /* FIXME: There may be some space between the .data and .bss
     sections which we ought to clear. */

  if (data_base != 0)
  {

    DEBUG_MESSAGE (DEBUG, "Mapping %u pages", 
                   SIZE_IN_PAGES (process_data->bss_section_base +
                                  process_data->bss_section_size -
                                  process_data->data_section_base));

    memory_virtual_map
      (GET_PAGE_NUMBER (BASE_PROCESS_CREATE), data_base,
       SIZE_IN_PAGES (process_data->bss_section_base +
                      process_data->bss_section_size -
                      process_data->data_section_base), PAGE_KERNEL);
      
    memory_copy ((void *) BASE_PROCESS_CREATE,
                 process_data->data_section_address,
                 process_data->data_section_size);
    
    DEBUG_MESSAGE (DEBUG, "BSS base: %x", BASE_PROCESS_CREATE +
                   (process_data->bss_section_base -
                    process_data->data_section_base));
                   

    memory_set_u8 ((u8 *) BASE_PROCESS_CREATE +
                   (process_data->bss_section_base -
                    process_data->data_section_base), 0,
                   process_data->bss_section_size);

    DEBUG_MESSAGE (DEBUG, "Clearing 0x%x bytes",
                   process_data->bss_section_size);

    memory_virtual_map_other 
      (process_tss, GET_PAGE_NUMBER (process_data->data_section_base),
       data_base, SIZE_IN_PAGES (process_data->bss_section_base +
                                 process_data->bss_section_size -
                                 process_data->data_section_base),
       PAGE_NON_PRIVILEGED | PAGE_WRITABLE);
  }      

  /* Save the bases and size in the TSS for later usage. */

  process_tss->virtual_code_base = process_data->code_section_base;
  process_tss->virtual_data_base = process_data->data_section_base;
  process_tss->code_base = code_base;
  process_tss->data_base = data_base;
  process_tss->code_pages = SIZE_IN_PAGES (process_data->code_section_size);
  process_tss->data_pages = SIZE_IN_PAGES (process_data->data_section_size +
                                           process_data->bss_section_size);
  process_tss->stack_pages = 1;

  /* Set up an AVL tree for the process, and map it. */

  process_avl_intro_pages = SIZE_IN_PAGES (sizeof (avl_header_type) +
                                           (MAX_VIRTUAL_AVL_NODES / 8));
  process_avl_array_pages = SIZE_IN_PAGES (sizeof (avl_node_type) *
                                           MAX_VIRTUAL_AVL_NODES);

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Process AVL bitmap.");

  avl_intro_base = physical_page;

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Process AVL array.");

  avl_array_base = physical_page;

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_CREATE), avl_intro_base, 
                      1, PAGE_KERNEL);
  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_CREATE) + 1, 
                      avl_array_base, 1, PAGE_KERNEL);

  memory_virtual_map_other (process_tss,
                            GET_PAGE_NUMBER (BASE_PROCESS_AVL_TREE),
                            avl_intro_base, 1, PAGE_KERNEL);
  memory_virtual_map_other (process_tss,
                            GET_PAGE_NUMBER (BASE_PROCESS_AVL_TREE) +
                            process_avl_intro_pages, avl_array_base, 1,
                            PAGE_KERNEL);
  new_avl_header = (avl_header_type *) BASE_PROCESS_CREATE;

  new_avl_header->limit_nodes = MAX_VIRTUAL_AVL_NODES;
  new_avl_header->number_of_nodes = 1;
  new_avl_header->limit_pages_bitmap = process_avl_intro_pages;
  new_avl_header->limit_pages_array = process_avl_array_pages;

  /* Only the first page of each of those is allocated at start. */

  new_avl_header->pages_allocated_bitmap = 1;
  new_avl_header->pages_allocated_array = 1;

  new_avl_header->growable = TRUE;

  //  /*  process_tss->virtual_base =
  //      MAX_OF_TWO (process_data->code_section_base +
  //      SIZE_IN_PAGES (process_data->code_section_size),
  //      process_data->data_section_base +
  //      SIZE_IN_PAGES (process_data->data_section_size)); */
  
  new_avl_header->root = new_avl_header->node_array =
    (avl_node_type *) (BASE_PROCESS_CREATE + 1 * SIZE_PAGE);

  /* The lowest 4 megs are reserved for kernel and other stuff. */

  avl_node_reset (new_avl_header->root, 0, 1024, MAX_PAGES - 1024, NULL);

  new_avl_header->root = new_avl_header->node_array =
    (avl_node_type *) (BASE_PROCESS_AVL_TREE +
                       process_avl_intro_pages * SIZE_PAGE);

  /* Insert the shared page table entries into the pagedirectory. */
  /* FIXME: This should be a function in memory_virtual.c */

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY),
                      page_directory_page, 1, PAGE_KERNEL);
    
  for (counter = 0; counter < SIZE_IN_PAGES (SIZE_GLOBAL) / 1024; counter++)
  {
    u32 index = (GET_PAGE_NUMBER (BASE_GLOBAL) / 1024) + counter;
      
    page_directory[index].present = 1;

    /* FIXME: Change this to PAGEDIRECTORY_FLAGS when moving to
       memory_virtual.c. */

    page_directory[index].flags = PAGE_WRITABLE;
    page_directory[index].accessed = 0;
    page_directory[index].zero = 0;
    page_directory[index].page_size = 0;
      
    /* This will boost performance a little, since this memory won't
       have to be cache invalidated on task switches. */
      
    if (cpu_info.flags.flags.pge)
    {
      page_directory[index].global = 1;
    }
    else
    {
      page_directory[index].global = 0;
    }
      
    page_directory[index].available = 0;
    page_directory[index].page_table_base =
      (GET_PAGE_NUMBER (shared_page_tables) + counter);

    /* Map the shared page tables. */
      
    memory_virtual_map_other (process_tss,
                              GET_PAGE_NUMBER (BASE_PROCESS_PAGE_TABLES) +
                              index,
                              GET_PAGE_NUMBER (shared_page_tables) + counter,
                              1, PAGE_KERNEL);
  }

  memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY),
                      GET_PAGE_NUMBER (shared_page_tables),
                      1, PAGE_KERNEL);

  /* Set up the rest of the data in the TSS. */

  process_tss->eip = process_data->initial_eip;

  /* We put a 'magic cookie' in the general purpose registers to
     easify debugging. Also, it makes it possible for programs to
     detect if they're running under storm very easily. ;-) */

  process_tss->eax = process_tss->ebx = process_tss->ecx = process_tss->edx =
    0xC0CAC01A;

  /* Fill up the segment registers. */

  process_tss->cs = SELECTOR_PROCESS_CODE;
  process_tss->ds = SELECTOR_DATA;
  process_tss->es = SELECTOR_DATA;
  process_tss->fs = SELECTOR_DATA;
  process_tss->gs = SELECTOR_DATA;
  process_tss->ss = SELECTOR_DATA;

  /* Allocate a PL3 stack. */

  process_tss->eflags = THREAD_NEW_EFLAGS;
  process_tss->esp = 0;

  /* Allocate a PL0 stack. */

  process_tss->ss0 = SELECTOR_KERNEL_DATA;
  process_tss->esp0 = BASE_PROCESS_STACK + SIZE_PAGE;

  /* Fill in some other neat-O-stuff. */

  process_tss->process_type = process_data->process_type;
  process_tss->process_id = *process_data->process_id;

  process_tss->state = STATE_DISPATCH;
  string_copy (process_tss->thread_name, "unnamed");

  /* Set up an I/O map with no size for now. It will be resized when
     the process registers and/or deregisters ports. */

  process_tss->iomap_base = sizeof (storm_tss_type);
  
  /* Don't ever forget about your parent's birthday! */

  if (initialised)
  {
    process_tss->parent_tss = (struct storm_tss_type *) current_tss;
  }
  else
  {
    process_tss->parent_tss = (struct storm_tss_type *) kernel_tss;
  }

  /* Now, add this process to the process tree. */

  mutex_kernel_wait (&process_tree_mutex);
  process_info = memory_global_allocate (sizeof (process_info_type));
  process_info->process_id = process_tss->process_id;
  process_info->thread_list = NULL;
  process_info->name = (char *) unnamed;
  process_info->number_of_threads = 1;
  process_link (process_info);
  thread_link_list (&process_info->thread_list, process_tss);
  process_tss->process_info = (struct process_info_type *) process_info;
  number_of_processes++;
  mutex_kernel_signal (&process_tree_mutex);

  mutex_kernel_wait (&tss_tree_mutex);
  number_of_tasks++;

  if (process_data->block)
  {
    if (initialised)
    {
      /* We are being called from userland. Block. */

      current_tss->state = STATE_BLOCKED_PARENT;
      mutex_kernel_signal (&tss_tree_mutex);
      dispatch_next ();
      return STORM_RETURN_SUCCESS;
    }
    else
    {
      kernel_child_list_type *new_kernel_child =
        memory_global_allocate (sizeof (kernel_child_list_type));
      kernel_child_list_type *last_kernel_child;

      /* We are in the bootup sequence of storm. */

      if (!first_process)
      {
        process_tss->state = STATE_BLOCKED_BOOT;
        
        /* Link this process to the wake up list at the end. */
        
        new_kernel_child->next = NULL;
        new_kernel_child->tss = process_tss;
        
        if (kernel_child_list == NULL)
        {
          kernel_child_list = new_kernel_child;
        }
        else
        {
          last_kernel_child = kernel_child_list;
          
          while (last_kernel_child->next != NULL)
          {
            last_kernel_child =
              (kernel_child_list_type *) last_kernel_child->next;
          }
          
          last_kernel_child->next =
            (struct kernel_child_list_type *) new_kernel_child;
        }
      }

      if (first_process)
      {
        first_process = FALSE;
      }
    }
  }
  mutex_kernel_signal (&tss_tree_mutex);

  return STORM_RETURN_SUCCESS;
}

/* Set the name of a process. */
/* FIXME: Update process_name in all the TSS's belonging to this
   process. */

return_type process_name_set (char *name)
{
  process_info_type *process_info = 
    (process_info_type *) current_tss->process_info;

  mutex_kernel_wait (&tss_tree_mutex);
  if (process_info->name != unnamed)
  {
    memory_global_deallocate (process_info->name);
  }
  process_info->name = memory_global_allocate (string_length (name) + 1);
  string_copy (process_info->name, name);
  mutex_kernel_signal (&tss_tree_mutex);

  return STORM_RETURN_SUCCESS;
}

/* Unblock a parent. */

return_type process_parent_unblock (void)
{
  if (current_tss->parent_tss == (struct storm_tss_type *) kernel_tss)
  {
    /* Check the list of kernel childs to see if there is something to
       do. */

    if (kernel_child_list != NULL)
    {
      kernel_child_list->tss->state = STATE_DISPATCH;

      kernel_child_list = (kernel_child_list_type *) kernel_child_list->next;
    }
  }

  else if (current_tss->parent_tss != NULL &&
           ((storm_tss_type *) current_tss->parent_tss)->state ==
           STATE_BLOCKED_PARENT)
  {
    /* We have a blocked parent, wake it up. */

    ((storm_tss_type *) current_tss->parent_tss)->state = STATE_DISPATCH;
  }

  return STORM_RETURN_SUCCESS;
}
