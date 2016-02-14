// Abstract: Defines used by the console library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

// Welcome, to the Real World. In this, we use EGA colors in the year of 2KAD...
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
