/* $Id$ */
/* Abstract: Function prototypes and external variables. */
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

#ifndef __CLUIDO_H__
#define __CLUDIO_H__

#include "config.h"

/* Function prototypes. */

extern void main_loop (void) __attribute__ ((noreturn));
extern void prompt_print (char *input);
extern char *environment_get (char *key);
extern void run (char *command_string);

/* Type definitions. */

typedef struct
{
  char name[64];
  char arguments[256];
  char description[70];
  void *pointer;
} command_type;

typedef struct
{
  int number_of_arguments;
  char **argument_start;
  char *arguments;
} arguments_type;

/* FIXME: Temporary until we get the memory allocation of our
   lives. */

typedef struct
{
  char key[16];
  char value[256];
} environment_type;

extern command_type command[];
extern int number_of_commands;
extern char working_directory[];
extern environment_type environment[];
extern ipc_structure_type vfs_structure;
extern console_structure_type console_structure;
extern ipc_structure_type ipv4_structure;
extern ipc_structure_type pci_structure;
extern char host_name[IPV4_HOST_NAME_LENGTH];
extern tag_type empty_tag;
extern bool has_net;

#endif /* !__CLUDIO_H__ */
