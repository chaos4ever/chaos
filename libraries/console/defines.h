// Abstract: Defines used by the console library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

// Welcome, to the Real World. In this, we use EGA colors in the year of 2KAD...
// FIXME: I wonder what this is, really. These are _neither_ EGA nor
// ANSI colours in reality. They resemble ANSI more than EGA but not
// 100%...
enum
{
    CONSOLE_COLOUR_BLACK,
    CONSOLE_COLOUR_RED,
    CONSOLE_COLOUR_GREEN,
    CONSOLE_COLOUR_CYAN,
    CONSOLE_COLOUR_BLUE,
    CONSOLE_COLOUR_PURPLE,
    CONSOLE_COLOUR_BROWN,
    CONSOLE_COLOUR_GRAY,
};

// These colours are used when working with buffered console output.
// Note that their numeral values are different to the ones above;
// this is used so that buffers can copied to EGA-compatible text
// mode hardware without conversion.
enum
{
    CONSOLE_BUFFER_COLOUR_BLACK,            // 0
    CONSOLE_BUFFER_COLOUR_BLUE,
    CONSOLE_BUFFER_COLOUR_GREEN,
    CONSOLE_BUFFER_COLOUR_CYAN,
    CONSOLE_BUFFER_COLOUR_RED,
    CONSOLE_BUFFER_COLOUR_MAGENTA,
    CONSOLE_BUFFER_COLOUR_BROWN,
    CONSOLE_BUFFER_COLOUR_LIGHT_GRAY,

    CONSOLE_BUFFER_COLOUR_DARK_GRAY,        // 8
    CONSOLE_BUFFER_COLOUR_BRIGHT_BLUE,
    CONSOLE_BUFFER_COLOUR_BRIGHT_GREEN,
    CONSOLE_BUFFER_COLOUR_BRIGHT_CYAN,
    CONSOLE_BUFFER_COLOUR_BRIGHT_RED,
    CONSOLE_BUFFER_COLOUR_BRIGHT_MAGENTA,
    CONSOLE_BUFFER_COLOUR_BRIGHT_YELLOW,
    CONSOLE_BUFFER_COLOUR_BRIGHT_WHITE,

    CONSOLE_BUFFER_BG_COLOUR_BLACK,         // 0x10
    CONSOLE_BUFFER_BG_COLOUR_BLUE,
    CONSOLE_BUFFER_BG_COLOUR_GREEN,
    CONSOLE_BUFFER_BG_COLOUR_CYAN,
    CONSOLE_BUFFER_BG_COLOUR_RED,
    CONSOLE_BUFFER_BG_COLOUR_MAGENTA,
    CONSOLE_BUFFER_BG_COLOUR_BROWN,
    CONSOLE_BUFFER_BG_COLOUR_LIGHT_GRAY
};

enum
{
    CONSOLE_ATTRIBUTE_RESET = 0,
    CONSOLE_ATTRIBUTE_BRIGHT = 1,
    CONSOLE_ATTRIBUTE_BLINK = 5,
};

enum
{
    CONSOLE_KEYBOARD_NORMAL,
    CONSOLE_KEYBOARD_EXTENDED
};

enum
{
    CONSOLE_EVENT_KEYBOARD,
    CONSOLE_EVENT_MOUSE
};
