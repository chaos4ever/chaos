// Abstract: Function definitions for the console library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

#include <system/system.h>
#include <console/console.h>

C_EXTERN_BEGIN

extern return_type console_init(console_structure_type *console_structure, tag_type *tag,
                                unsigned int connection_class);
extern return_type console_open(console_structure_type *console_structure,
                                ipc_console_attribute_type console_attribute);
extern return_type console_mode_set(console_structure_type *console_structure, unsigned int width,
                                    unsigned int height, unsigned int depth, int mode_type);

extern return_type console_resize(console_structure_type *console_structure, unsigned int width,
                                  unsigned int height);
extern return_type console_print(console_structure_type *console_structure, const char *string);

// Prints the given data to the console, similar to printf.
extern return_type console_print_formatted(console_structure_type *console_structure,
                                           const char *string, ...) __attribute__ ((format (printf, 2, 3)));
extern return_type console_clear(console_structure_type *console_structure);

// Moves the cursor. Note that the position is zero-indexed; the top-left coordinate is 0,0.
extern return_type console_cursor_move(console_structure_type *console_structure, unsigned int x,
                                       unsigned int y);
extern return_type console_attribute_set(console_structure_type *console_structure, int foreground,
                                         int background, int attributes);
extern return_type console_cursor_appearance_set(console_structure_type *console_structure,
                                                 bool visibility, bool block);
extern return_type console_use_keyboard(console_structure_type *console_structure, bool which, int type);
extern return_type console_use_mouse(console_structure_type *console_structure, bool which);
extern return_type console_event_wait(console_structure_type *console_structure, void *event_data,
                                      int *type, bool block);

// Prints output to a console using double-buffering, at the given
// cursor position.
extern return_type console_buffer_print(
    console_structure_type *console_structure, int x, int y,
    uint8_t attribute, const char *string
);

// Prints printf-formatted output to a console using double-buffering, at
// the given cursor position.
extern return_type console_buffer_print_formatted(
    console_structure_type *console_structure, int x, int y,
    uint8_t attribute, const char *string, ...
) __attribute__ ((format (printf, 5, 6)));

// Flips double-buffered content to the "real" console. If this console
// uses double-buffering, this function must be called whenever a
// "frame" is ready to be displayed.
extern return_type console_flip(console_structure_type *console_structure);

C_EXTERN_END
