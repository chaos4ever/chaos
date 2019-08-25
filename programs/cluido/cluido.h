// Abstract: Function prototypes and external variables.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

#include "config.h"

// Function prototypes.
extern void main_loop(void) __attribute__((noreturn));
extern void prompt_print(char *input);
extern char *environment_get(char *key);
extern void run(char *command_string);
extern void command_execute(int number_of_arguments, char **argument);

// Type definitions.
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

// FIXME: Temporary until we get the memory allocation of our lives.
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
