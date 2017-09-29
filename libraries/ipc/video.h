// Abstract: Video protocol.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    // Set the specified video mode.
    IPC_VIDEO_MODE_SET = (IPC_PROTOCOL_VIDEO << 16),

    // Place the cursor on the given position.
    IPC_VIDEO_CURSOR_PLACE,

    // Set the full 256 color palette.
    IPC_VIDEO_PALETTE_SET,

    // Get the full 256 color palette.
    IPC_VIDEO_PALETTE_GET,

    // Set the font.
    IPC_VIDEO_FONT_SET,
};

// IPC structures for video.
typedef struct
{
    unsigned int x;
    unsigned int y;
} video_cursor_type;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} video_palette_type;

// Videomode set flags.
enum
{
    VIDEO_MODE_TYPE_TEXT,
    VIDEO_MODE_TYPE_GRAPHIC
};

enum
{
    // The call completed successfully.
    IPC_VIDEO_RETURN_SUCCESS,

    // The requested mode could not be set.
    IPC_VIDEO_RETURN_MODE_UNAVAILABLE,
};
