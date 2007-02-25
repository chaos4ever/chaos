/* $Id$ */
/* Abstract: Initialisation code. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#include "cluido.h"

console_structure_type console_structure;
ipc_structure_type ipv4_structure;
ipc_structure_type pci_structure;
tag_type empty_tag =
{
  0, 0, ""
};

bool has_net = FALSE;

/* Main function. */

int main (void)
{
  /* Set our names. */

  system_process_name_set ("cluido");
  system_thread_name_set ("Initialising");

  /* First of all, initiate a connection to the console service. */

  if (console_init (&console_structure, &empty_tag,
                    IPC_CONSOLE_CONNECTION_CLASS_CLIENT) !=
      CONSOLE_RETURN_SUCCESS)
  {
    return -1;
  }

  if (console_open (&console_structure, 80, 50, 4, VIDEO_MODE_TYPE_TEXT) !=
      CONSOLE_RETURN_SUCCESS)
  {
    return -1;
  }

  file_init (&vfs_structure, &empty_tag);
  memory_init ();

  if (ipv4_init (&ipv4_structure, &empty_tag) == IPV4_RETURN_SUCCESS)
  {
    has_net = TRUE;
    ipv4_host_name_get (&ipv4_structure, host_name);
  }

  pci_init (&pci_structure, &empty_tag);

  console_use_keyboard (&console_structure, TRUE, CONSOLE_KEYBOARD_NORMAL);

  console_attribute_set (&console_structure, CONSOLE_COLOUR_GRAY,
                         CONSOLE_COLOUR_BLUE, CONSOLE_ATTRIBUTE_RESET);
  console_clear (&console_structure);

  /* FIXME: This should not be put in cluido, really... */

  console_print (&console_structure, "\

NOTE! This is an unstable prerelease of chaos. Do expect system crashes and
strange behaviour. There is no need for you, as a user, to fix minor bugs and
send us patches, since the bug probably already have been fixed when the patch
arrives to us. If you find a serious bug though, please report to
chaos-devel@lists.chaosdev.org and hopefully we can work something out. Thank
you for your time and interest in the project.\n");

  console_print (&console_structure, "\

    \e[1mcluido version 0.0.1, Copyright (C) 1998-2000 chaos development.
    cluido and the rest of chaos comes with ABSOLUTELY NO WARRANTY.
    This is free software, and you are welcome to redistribute it under
    certain conditions; see the GNU GPL for details.\e[0;44m\n");

  console_print (&console_structure, "\

This is cluido, the Command Line User Interface. Type 'help' or '?' for
information about available commands. You can also cycle through the virtual
consoles using ALT+TAB. To 'bind' a console to a function key, use
CTRL+ALT+Fn. This console can later be switched to by pressing ALT+Fn.
Consoles having a blue background are shells, just like this one, and consoles
with a black background is the log consoles (one for servers and one for the
kernel). If tornado has been started, it will have its own graphical console
with a movable mouse cursor.

Some Emacs compatible bindings (C-a, C-e, etc) are available, and also some
DOS:ish ones (Home, End, arrows). You can go back and forth in the command
history with the up and down arrows.\n\n"); 

  main_loop ();
  return 0;
}
