/* $Id$ */
/* Abstract: Server for 3Com PCI ethernet cards. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#define _3COM_VENDOR_ID          0x10B7
#define _3COM_DEVICE_ID_3C905    0x9050

static log_structure_type log_structure;
static ipc_structure_type pci_structure;

/* An empty tag list. */

static tag_type empty_tag =
{
  0, 0, ""
};

/* Function for handling 3C905 cards (Boomerang) */

static void handle_boomerang (pci_device_info_type *device_info)
{
  u16 base = device_info[0].resource[0].start;

  
  system_call_port_range_register (base, device_info[0].resource[0].end -
                                   base + 1, "3C905");
}

int main (void)
{
  pci_device_probe_type probe;
  pci_device_info_type *device_info;
  unsigned int number_of_devices;
  unsigned int counter;

  system_call_process_parent_unblock ();
    
  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initialising");

  if (log_init (&log_structure, PACKAGE_NAME, &empty_tag) !=
      LOG_RETURN_SUCCESS)
  {
    return -1;
  }

  if (pci_init (&pci_structure, &empty_tag) != PCI_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY, 
               "Couldn't create connection to PCI service.");
    return -1;
  }

  probe.vendor_id = _3COM_VENDOR_ID;
  probe.device_id = _3COM_DEVICE_ID_3C905;
  pci_device_exists (&pci_structure, &probe, &device_info, &number_of_devices);

  for (counter = 0; counter < number_of_devices; counter++)
  {
    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      handle_boomerang (&device_info[counter]);
    }
  }

  return 0;
}
