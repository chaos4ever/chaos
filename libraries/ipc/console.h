// Abstract: Console server IPC message types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

// Message types.
enum
{
    IPC_CONSOLE_CONNECTION_CLASS_SET = (IPC_PROTOCOL_CONSOLE << 16),
    IPC_CONSOLE_CURSOR_MOVE,
    IPC_CONSOLE_OPEN,
    IPC_CONSOLE_MODE_SET,
    IPC_CONSOLE_CLOSE,
    IPC_CONSOLE_OUTPUT,
    IPC_CONSOLE_OUTPUT_ALL,
    IPC_CONSOLE_RESIZE,
    IPC_CONSOLE_ENABLE_KEYBOARD,
    IPC_CONSOLE_DISABLE_KEYBOARD,
    IPC_CONSOLE_KEYBOARD_EXTENDED,
    IPC_CONSOLE_KEYBOARD_NORMAL,
    IPC_CONSOLE_ENABLE_MOUSE,
    IPC_CONSOLE_DISABLE_MOUSE,

    // An input event on the keyboard.
    IPC_CONSOLE_KEYBOARD_EVENT,

    // A mouse event.
    IPC_CONSOLE_MOUSE_EVENT
};

// Connection classes.
enum
{
    IPC_CONSOLE_CONNECTION_CLASS_NONE,
    IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_KEYBOARD,
    IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_MOUSE,
    IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_JOYSTICK,
    IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_VIDEO,
    IPC_CONSOLE_CONNECTION_CLASS_CLIENT,
};

// Resize the current console.
typedef struct
{
    unsigned int width;
    unsigned int height;
} ipc_console_resize_type;

// Physical console attribute type.
typedef struct
{
    // The width in pixels (for VIDEO_MODE_TYPE_GRAPHIC) or characters
    // (for VIDEO_MODE_TYPE_TEXT) of the console being opened.
    unsigned int width;

    // The height in pixels (for VIDEO_MODE_TYPE_GRAPHIC) or lines (for
    // VIDEO_MODE_TYPE_TEXT) of the console being opened.
    unsigned int height;

    // For VIDEO_MODE_TYPE_GRAPHIC, indicates the depth in bits (15, 16,
    // 24 etc) of the mode being requested.
    unsigned int depth;

    // Set to either VIDEO_MODE_TYPE_TEXT or VIDEO_MODE_TYPE_GRAPHIC.
    unsigned int mode_type;

    // Indicates that the console should be activated upon creation. Can
    // only be done once; if a console is already active, this flag will
    // be ignored by the console server.
    bool activate;

    // Set to TRUE if the client desires a local buffer for
    // double-buffering console output. This is useful in applications
    // with a lot of console outputs and/or real-time visualization
    // requirements.
    bool enable_buffer;
} ipc_console_attribute_type;
