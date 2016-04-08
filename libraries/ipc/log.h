// Abstract: Log protocol.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <ipc/protocols.h>

// Defines and enumerations.
#define IPC_LOG_MAX_MESSAGE_LENGTH      256
#define IPC_LOG_MAX_CLASS_LENGTH        256

enum
{
  // Print a message to the log. The type of the message is specified in ipc_log_print_type.
  IPC_LOG_PRINT = (IPC_PROTOCOL_LOG << 16),
};

// Type definitions.
typedef struct
{
  char message[IPC_LOG_MAX_MESSAGE_LENGTH];
  char log_class[IPC_LOG_MAX_CLASS_LENGTH];

  // How urgent is this log message? 0 is most urgent, and higher numbers are of less importance.
  unsigned int urgency;
} ipc_log_print_type;
