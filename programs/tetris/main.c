/* Abstract: Main function for the tetris program. */
/* Author: Per Lundberg <per@chaosdev.io> */

/* Copyright 2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include "config.h"
#include "tetris.h"

console_structure_type console_structure;
tag_type empty_tag = {
    0, 0, ""
};

int main(void) {
    if (console_init(&console_structure, &empty_tag, IPC_CONSOLE_CONNECTION_CLASS_CLIENT) != CONSOLE_RETURN_SUCCESS) {
        return -1;
    }

    ipc_console_attribute_type console_attribute =
    {
        .width = 80,
        .height = 50,
        .depth = 4,
        .mode_type = VIDEO_MODE_TYPE_TEXT,
        .activate = TRUE,
        .enable_buffer = FALSE
    };

    if (console_open(&console_structure, console_attribute) != CONSOLE_RETURN_SUCCESS) {
        return -1;
    }

    tetris();
    return 0;
}
