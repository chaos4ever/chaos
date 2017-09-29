// Abstract: General IPC messages.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    IPC_GENERAL_ECHO_REQUEST,
    IPC_GENERAL_ECHO_REPLY,
    IPC_GENERAL_IDENTIFY_REQUEST,
    IPC_GENERAL_IDENTIFY_REPLY,
    IPC_GENERAL_CONNECTION_REQUEST,
    IPC_GENERAL_CONNECTION_REPLY,
    IPC_GENERAL_CONNECTION_REFUSED,
    IPC_GENERAL_CONNECTION_CLOSE,
};
