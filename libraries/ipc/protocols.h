// Abstract: Definition of the protocol types used in the chaos system. Also define all the message types those protocols support.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

// Those are in chronological order. When adding protocols, it must of course be put at the end to avoid recompiling everything...

enum
{
    // No protocol.
    IPC_PROTOCOL_NONE,

    // The general IPC protocol is used for communicating with the service mailboxes.
    IPC_PROTOCOL_GENERAL,

    IPC_PROTOCOL_MOUSE,
    IPC_PROTOCOL_KEYBOARD,
    IPC_PROTOCOL_CONSOLE,
    IPC_PROTOCOL_VIDEO,
    IPC_PROTOCOL_PCI,
    IPC_PROTOCOL_ETHERNET,
    IPC_PROTOCOL_IPV4,
    IPC_PROTOCOL_BLOCK,
    IPC_PROTOCOL_VIRTUAL_FILE_SYSTEM,
    IPC_PROTOCOL_FILE,
    IPC_PROTOCOL_LOG,
    IPC_PROTOCOL_SERIAL,
    IPC_PROTOCOL_SOUND,
};

// Ugly, but those rely on the previous defines... so we can't do in a nicer way.
#include <ipc/block.h>
#include <ipc/console.h>
#include <ipc/ethernet.h>
#include <ipc/file.h>
#include <ipc/general.h>
#include <ipc/ipv4.h>
#include <ipc/keyboard.h>
#include <ipc/log.h>
#include <ipc/mouse.h>
#include <ipc/pci.h>
#include <ipc/serial.h>
#include <ipc/sound.h>
#include <ipc/video.h>
