// Abstract: IPC library functions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

#include <system/system.h>

C_EXTERN_BEGIN

extern return_type ipc_send(mailbox_id_type mailbox_id, message_parameter_type *message_parameter);
extern return_type ipc_receive(mailbox_id_type mailbox_id, message_parameter_type *message_parameter, unsigned int *buffer_size);
extern return_type ipc_service_create(const char *service_name, ipc_structure_type *ipc_structure, tag_type *tag);
extern return_type ipc_service_connection_wait(ipc_structure_type *ipc_structure);
extern return_type ipc_service_connection_request(ipc_structure_type *ipc_structure);
extern return_type ipc_service_resolve(const char *service_name,  mailbox_id_type *mailbox_id, unsigned int *number_of_mailbox_ids,
    time_type timeout, tag_type *tag);
extern return_type ipc_connection_establish(ipc_structure_type *ipc_structure);
extern return_type ipc_connection_close(ipc_structure_type *ipc_structure, bool notify);

C_EXTERN_END