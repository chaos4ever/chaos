/* $Id$ */
/* Abstract: ATA server for chaos. Follows the standard chaos block
   service format. */
/* Author: Henrik Hallin <hal@chaosdev.org>
           Anders Öhrt <doa@chaosdev.org> 
           Per Lundberg <plundis@chaosdev.org> */

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

#include "ata.h"
#include "config.h"
#include "partition.h"

log_structure_type log_structure;
tag_type empty_tag =
{
  0, 0, ""
};

static inline u8 lba_to_sector_number (device_type *device, u32 linear_sector)
{
  return (linear_sector % device->sectors_per_track) + 1;
}

static inline u8 lba_to_cylinder_low (device_type *device, u32 linear_sector)
{
  return (linear_sector / (device->number_of_heads *
           device->sectors_per_track)) & 0xFF;
}

static inline u8 lba_to_cylinder_high (device_type *device, u32 linear_sector)
{
  return ((linear_sector / (device->number_of_heads *
            device->sectors_per_track)) >> 8) & 0xFF;
}

static inline u8 lba_to_head (device_type *device, u32 linear_sector)
{
  return ((linear_sector / device->sectors_per_track) %
          device->number_of_heads);
}

/* Select a device on an interface. Returns TRUE if successful. */

static bool select_device (interface_type *interface, unsigned int device)
{
  unsigned int timeout = 0;

  /* Wait for BSY to clear for up to one second. */
  
  while ((system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) & 
          BIT_BSY) != 0 && timeout < 10)
  {
    timeout++;
    system_sleep (100);
  }

  if ((system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) & 
       BIT_BSY) != 0)
  {
    return FALSE;
  }

  /* Select device. */
  /* FIXME: Optimize this by having an interface->selected_device flag
     to avoid reselecting the same device over and over. */
  
  system_port_out_u8 (interface->io_base + REGISTER_DEVICE_HEAD,
                      (device == 0) ? DEVICE_MASTER : DEVICE_SLAVE);

  /* Wait for controller for up to one second. */

  timeout = 0;

  while ((system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) &
          (BIT_BSY | BIT_DRDY)) != BIT_DRDY &&
         timeout < 10)
  {
    timeout++;
    system_sleep (100);
  }
  
  if ((system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) &
       (BIT_BSY | BIT_DRDY)) != BIT_DRDY)
  {
    return FALSE;
  }

  return TRUE;
}

/* FIXME: Remove. */

static void interrupt_handler (interface_type *interface)
{
  message_parameter_type message_parameter;

  if (system_call_irq_register (interface->irq, "ATA interface") !=
      SYSTEM_RETURN_SUCCESS)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Could not allocate IRQ %u.",
                         interface->irq);
    return;
  }

  message_parameter.length = 0;
  message_parameter.block = TRUE;
  message_parameter.data = NULL;

  while (TRUE)
  {
    system_call_irq_wait (interface->irq);
    system_call_irq_acknowledge (interface->irq);

    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "IRQ %u.", interface->irq);
    system_call_mailbox_send (interface->mailbox_id, &message_parameter);
  }
}

/* Send a command and await an IRQ. */

static void send_command (interface_type *interface, u8 command)
{
  message_parameter_type message_parameter;

  message_parameter.length = 0;
  message_parameter.block = TRUE;
  message_parameter.data = NULL;

  system_port_out_u8 (interface->io_base + REGISTER_COMMAND, command);
  system_call_mailbox_receive (interface->mailbox_id, &message_parameter);
}

/* Read one or more sectors from a device. */
/* FIXME: Do rangechecking in all neccessary ways. */

bool ata_read_sectors (interface_type *interface, unsigned int device,
                       u32 sector_number, unsigned int number_of_sectors,
                       void *buffer)
{
  u8 status, error;

  /* Check if the device exists. */

  if (interface->device[device] == NULL)
  {
    return FALSE;
  }

  /* Range check number of sectors to read, and convert to ATA way. */

  if (number_of_sectors == 0 || number_of_sectors > 256)
  {
    return FALSE;
  }
  if (number_of_sectors == 256)
  {
    number_of_sectors = 0;
  }

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "Trying to read sector number %lu.",
                       sector_number);

  /* Select the requested device. */

  if (!select_device (interface, device))
  {
    return FALSE;
  }

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "Reading %u sectors...", number_of_sectors);

  /* No features. */

  system_port_out_u8 (interface->io_base + REGISTER_FEATURE, 0);

  /* How many sectors to read? */

  system_port_out_u8 (interface->io_base + REGISTER_SECTOR_COUNT,
                      number_of_sectors);
  
  if (interface->device[device]->lba)
  {
    /* This device supports LBA addressing mode. */

    log_print (&log_structure, LOG_URGENCY_DEBUG, "LBA");

    system_port_out_u8 (interface->io_base + REGISTER_SECTOR_NUMBER,
                        sector_number & 0xFF);
    system_port_out_u8 (interface->io_base + REGISTER_CYLINDER_LOW,
                        (sector_number >> 8) & 0xFF);
    system_port_out_u8 (interface->io_base + REGISTER_CYLINDER_HIGH,
                        (sector_number >> 16) & 0xFF);
    system_port_out_u8 (interface->io_base + REGISTER_DEVICE_HEAD,
                        ((sector_number >> 24) & 0x0F) | BIT_LBA |
                        ((device == 0) ? DEVICE_MASTER : DEVICE_SLAVE));
  }
  else
  {
    /* CHS addressing mode. */

    log_print (&log_structure, LOG_URGENCY_DEBUG, "CHS");

    system_port_out_u8 (interface->io_base + REGISTER_SECTOR_NUMBER,
                        lba_to_sector_number (interface->device[device],
                                              sector_number));
    system_port_out_u8 (interface->io_base + REGISTER_CYLINDER_LOW,
                        lba_to_cylinder_low (interface->device[device],
                                             sector_number));
    system_port_out_u8 (interface->io_base + REGISTER_CYLINDER_HIGH,
                        lba_to_cylinder_high (interface->device[device],
                                              sector_number));
    system_port_out_u8 (interface->io_base + REGISTER_DEVICE_HEAD,
                        lba_to_head (interface->device[device],
                                     sector_number) |
                        ((device == 0) ? DEVICE_MASTER : DEVICE_SLAVE));
  }

  /* Send the command. */

  send_command (interface, COMMAND_READ_SECTORS_RETRY);

  /* Read status and ACK controller. */

  error = system_port_in_u8 (interface->io_base + REGISTER_ERROR);
  status = system_port_in_u8 (interface->io_base + REGISTER_STATUS);

  if ((status & BIT_ERR) == 0)
  {
    /* Read the sector(s). */

    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Status OK, reading %d bytes from device...",
                         512 * number_of_sectors);

    system_port_in_u16_string (interface->io_base + REGISTER_DATA,
                               (u16 *) buffer, 256 * number_of_sectors);
    return TRUE;
  }
  else
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "An error occured while reading sector(s). status = 0x%X, error = 0x%X.",
                         status, error);

    return FALSE;
  }
}

/* "Check the Internet!" ('Bone Collector') */

static bool check_extended (interface_type *interface, unsigned int device,
                            u32 extended_start_sector, u32 offset)
{
  u8 *sector_buffer;
  partition_entry_type *partition;
  int index;
  service_type *service;

  memory_allocate ((void **) &sector_buffer, 512);

  /* Now, read the sector of the disk containing the partition
     table. */

  if (!ata_read_sectors (interface, device, extended_start_sector + offset,
                         1, sector_buffer))
  {
    return FALSE;
  }

  if (sector_buffer[511] != 0xAA ||
      sector_buffer[510] != 0x55)
  {
    return FALSE;
  }

  /* Parse the partition table. */

  partition = (partition_entry_type *) &sector_buffer[446];

  for (index = 0; index < 4; index++)
  {
    if (partition[index].type != 0 && partition[index].type != 5)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "  Partition: Type 0x%02X, starts at sector %lu, size: %lu MB. %s",
                           partition[index].type,
                           partition[index].lba_starting_sector_number + 
                           extended_start_sector + offset,
                           partition[index].lba_number_of_sectors * 512 / 1024 / 1024,
                           (partition[index].active == 1) ? "(Active)" : "");

      memory_allocate ((void **) &service, sizeof (service_type));

      service->interface = interface;
      service->device = device;
      service->start_sector = (extended_start_sector + offset +
                               partition[index].lba_starting_sector_number);
      service->number_of_sectors = partition[index].lba_number_of_sectors;

      /* Create a block service for this partition. */
      
      if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
      {
        handle_service (service);
      }
    }
    else if (partition[index].type == PARTITION_TYPE_EXTENDED_DOS ||
             partition[index].type == PARTITION_TYPE_EXTENDED_LINUX ||
             partition[index].type == PARTITION_TYPE_EXTENDED_WINDOWS)
    {
      /* Extended partition. */

      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "  Extended Partition: Starts at sector %lu, size: %lu MB. %s",
                           partition[index].lba_starting_sector_number +
                           extended_start_sector,
                           partition[index].lba_number_of_sectors * 512 / 1024 / 1024,
                           (partition[index].active == 1) ? "(Active)" : "");

      if (!check_extended (interface, device, extended_start_sector,
                           partition[index].lba_starting_sector_number))
      {
        return FALSE;
      }
    }
  }

  memory_deallocate ((void **) &sector_buffer);
  return TRUE;
}

/* This is a recursive function which will (hopefully ;) find all
   partitions on a disk. */

static bool check_primary (interface_type *interface, unsigned int device,
                           u32 sector_number)
{
  u8 *sector_buffer;
  partition_entry_type *partition;
  int index;
  service_type *service;

  memory_allocate ((void **) &sector_buffer, 512);

  /* Now, read the sector of the disk containing the partition
     table. */

  if (!ata_read_sectors (interface, device, sector_number, 1, sector_buffer))
  {
    return FALSE;
  }

  if (sector_buffer[511] != 0xAA ||
      sector_buffer[510] != 0x55)
  {
    return FALSE;
  }

  /* Parse the partition table. */

  partition = (partition_entry_type *) &sector_buffer[446];

  for (index = 0; index < 4; index++)
  {
    if (partition[index].type != 0 && partition[index].type != 5)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "  Partition: Type 0x%02X, starts at sector %lu, size: %lu MB. %s",
                           partition[index].type,
                           partition[index].lba_starting_sector_number,
                           partition[index].lba_number_of_sectors * 512 / 1024 / 1024,
                           (partition[index].active == 1) ? "(Active)" : "");

      memory_allocate ((void **) &service, sizeof (service_type));

      service->interface = interface;
      service->device = device;
      service->start_sector = partition[index].lba_starting_sector_number;
      service->number_of_sectors = partition[index].lba_number_of_sectors;

      /* Create a block service for this partition. */
      
      if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
      {
        handle_service (service);
      }
    }
    else if (partition[index].type == PARTITION_TYPE_EXTENDED_DOS ||
             partition[index].type == PARTITION_TYPE_EXTENDED_LINUX ||
             partition[index].type == PARTITION_TYPE_EXTENDED_WINDOWS)
    {
      /* Extended partition. */

      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "  Extended Partition: Starts at sector %lu, size: %lu MB. %s",
                           partition[index].lba_starting_sector_number,
                           partition[index].lba_number_of_sectors * 512 / 1024 / 1024,
                           (partition[index].active == 1) ? "(Active)" : "");

      if (!check_extended (interface, device,
                           partition[index].lba_starting_sector_number, 0))
      {
        return FALSE;
      }
    }
  }

  memory_deallocate ((void **) &sector_buffer);
  return TRUE;
}

/* Initialise a device. */

static void init_device (interface_type *interface, unsigned int device)
{
  service_type *service;

  /* First of all, create a service and a thread for the whole
     disk. */

  memory_allocate ((void **) &service, sizeof (service_type));

  service->interface = interface;
  service->device = device;
  service->start_sector = 0;
  service->number_of_sectors = interface->device[device]->number_of_sectors;

  if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    handle_service (service);
  }

  /* Now, create block services for all partitions on the disk. */

  if (!check_primary (interface, device, 0))
  {
    log_print (&log_structure, LOG_URGENCY_ERROR,
               "Reading partition tables failed.");
  }
}

/* Allocate resources for an interface and try to identify all devices
   on it. */

bool ata_init_interface (interface_type *interface)
{
  u8 status;
  unsigned int device;
  int index;
  unsigned int timeout = 0;

  if (system_call_port_range_register
      (interface->io_base, 8, "ATA interface") !=
      STORM_RETURN_SUCCESS)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Could not allocate hardware ports 0x%X-0x%X.",
                         interface->io_base,
                         interface->io_base + 7);
    return FALSE;
  }

  if (system_call_port_range_register
      (interface->io_base + REGISTER_DEVICE_CONTROL, 1, "ATA interface") !=
      STORM_RETURN_SUCCESS)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Could not allocate hardware port 0x%X.",
                         interface->io_base + REGISTER_DEVICE_CONTROL);
    return FALSE;
  }

  /* FIXME: Remove the mailbox concept. Wait for interrupts whenever
     needed instead. */

  /* Create a mailbox for this interface. */
  /* FIXME: The mailbox system needs to be modified slightly to
     support NULL messages correctly. */

  system_call_mailbox_create (&interface->mailbox_id, 1000,
                              PROCESS_ID_NONE,
                              CLUSTER_ID_NONE,
                              THREAD_ID_NONE);

  if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    system_thread_name_set ("IRQ handler");
    interrupt_handler (interface);
    log_print (&log_structure, LOG_URGENCY_DEBUG,
               "Unexpected return from interrupt handler.");
    system_exit ();
  }

  /* Software reset the interface. */

  log_print (&log_structure, LOG_URGENCY_DEBUG,
             "Software resetting interface...");

  system_port_out_u8 (interface->io_base + REGISTER_DEVICE_CONTROL,
                      BIT_SRST | REGISTER_DEVICE_CONTROL_DEFAULT);
  system_sleep_microseconds (1);
  system_port_out_u8 (interface->io_base + REGISTER_DEVICE_CONTROL,
                      REGISTER_DEVICE_CONTROL_DEFAULT);
  system_sleep_microseconds (1);

  /* Wait for up to 5 seconds (Should be 31) or until BSY is cleared. */

  while (timeout < 50 &&
         (system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) & BIT_BSY) != 0)
  {
    timeout++;
    system_sleep (100);
  }

  if ((system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) & BIT_BSY) == 0)
  {
    /* The device did not time out. */

    log_print (&log_structure, LOG_URGENCY_DEBUG,
               "BSY bit cleared correctly.");
  }
  else
  {
    log_print (&log_structure, LOG_URGENCY_DEBUG,
               "Software reset failed (BSY timeout).");
    return FALSE;
  }

  /* Now wait for DRDY to be set. */

  timeout = 0;

  while (timeout < 50 &&
         (system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) & BIT_DRDY) == 0)
  {
    timeout++;
    system_sleep (100);
  }

  if ((system_port_in_u8 (interface->io_base + REGISTER_ALTERNATE_STATUS) & BIT_DRDY) != 0)
  {
    /* The device did not time out. */

    log_print (&log_structure, LOG_URGENCY_DEBUG,
               "DRDY bit set correctly.");
  }
  else
  {
    log_print (&log_structure, LOG_URGENCY_DEBUG,
               "Software reset failed (DRDY timeout).");
    return FALSE;
  }

  /* The software reset was successful. */

  log_print (&log_structure, LOG_URGENCY_DEBUG,
             "Software reset succeeded.");

  /* Ok. We have things set up for this interface. Now, let's probe
     for devices on it. */
  
  for (device = 0; device < 2; device++)
  {
    unsigned int type = ATA_DEVICE_NONE;
    u8 sector_count = system_port_in_u8 (interface->io_base + REGISTER_SECTOR_COUNT),
      sector_number = system_port_in_u8 (interface->io_base + REGISTER_SECTOR_NUMBER),
      cylinder_low = system_port_in_u8 (interface->io_base + REGISTER_CYLINDER_LOW),
      cylinder_high = system_port_in_u8 (interface->io_base + REGISTER_CYLINDER_HIGH);

    if (select_device (interface, device))
    {
      if (sector_count == 0x01 &&
          sector_number == 0x01 &&
          cylinder_low == 0x00 &&
          cylinder_high == 0x00)
      {
        type = ATA_DEVICE_ATA;
      }
      else if (sector_count == 0x01 &&
               sector_number == 0x01 &&
               cylinder_low == 0x14 &&
               cylinder_high == 0xEB)
      {
        type = ATA_DEVICE_ATAPI;
      }

      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "%X %X %X %X %d",
                           sector_count,
                           sector_number,
                           cylinder_low,
                           cylinder_high,
                           type);

      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Found a %s device on interface.",
                           (device == 0) ? "master" : "slave");
      
      memory_allocate ((void **) &interface->device[device],
                       sizeof (device_type));
      memory_set_u8 ((u8 *) interface->device[device], 0,
                     sizeof (device_type));
      
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Sending identify command to device.");
      send_command (interface, COMMAND_IDENTIFY_DEVICE);
      
      /* Read the status register and ACK the controller at the same time. */
      
      status = system_port_in_u8 (interface->io_base + REGISTER_STATUS);
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Command status was 0x%X. Reading data...", status);

      /* Read the identification data from the device. */

      system_port_in_u16_string (interface->io_base + REGISTER_DATA,
                                 (u16 *) interface->device[device]->id,
                                 ATA_SECTOR_WORDS);

      /* Copy out some strings, byteswap them and NULL-terminate them. */

      for (index = 0; index < 20; index++)
      {
        interface->device[device]->model_number[index * 2 + 1] =
          interface->device[device]->id[27 + index];
        interface->device[device]->model_number[index * 2] =
          (interface->device[device]->id[27 + index] >> 8);
      }

      index = 39;

      while (interface->device[device]->model_number[index] == ' ' &&
        index >= 0)
      {
        index--;
      }

      interface->device[device]->model_number[index + 1] = '\0';

      /* Take the important information out of the identification structure. */

      interface->device[device]->lba = ((interface->device[device]->id[49] & BIT_VALUE (9)) != 0);
      interface->device[device]->dma = ((interface->device[device]->id[49] & BIT_VALUE (8)) != 0);

#if TRUE
      interface->device[device]->number_of_cylinders = interface->device[device]->id[1];
      interface->device[device]->number_of_heads = interface->device[device]->id[3];
      interface->device[device]->sectors_per_track = interface->device[device]->id[6];
#endif

      if (!interface->device[device]->lba)
      {
        interface->device[device]->number_of_cylinders = interface->device[device]->id[1];
        interface->device[device]->number_of_heads = interface->device[device]->id[3];
        interface->device[device]->sectors_per_track = interface->device[device]->id[6];
        interface->device[device]->number_of_sectors = 
          interface->device[device]->number_of_cylinders *
          interface->device[device]->number_of_heads *
          interface->device[device]->sectors_per_track;
      }
      else
      {
        interface->device[device]->number_of_sectors =
          (interface->device[device]->id[61] << 16) +
          interface->device[device]->id[60];
      }

      interface->device[device]->type = type;

      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%s device: %s. Capacity: %u MB. %s %s",
                           (type == ATA_DEVICE_ATA) ? "ATA" :
                           (type == ATA_DEVICE_ATAPI) ? "ATAPI" :
                           "Unknown",
                           interface->device[device]->model_number,
                           (unsigned int) interface->device[device]->number_of_sectors * 512 / 1024 / 1024,
                           interface->device[device]->lba ? "(LBA)" : "",
                           interface->device[device]->dma ? "(DMA)" : "");

      if (!interface->device[device]->lba)
      {
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "Geometry: Cylinders: %lu, heads: %lu, sectors: %lu.",
                             interface->device[device]->number_of_cylinders,
                             interface->device[device]->number_of_heads,
                             interface->device[device]->sectors_per_track);
      }

      /* Initialise this device. This includes reading the partition
         table and create a thread and a service for each found
         partition. */
      
      init_device (interface, device);
    }
    else
    {
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "No %s device found on interface.",
                           (device == 0) ? "master" : "slave");
    }
  }

  if (interface->device[0] == NULL &&
      interface->device[1] == NULL)
  {
    /* No devices on interface. */

    system_call_irq_unregister (interface->irq);
    system_call_port_range_unregister (interface->io_base);
    system_call_port_range_unregister (interface->io_base + REGISTER_DEVICE_CONTROL);

    /* FIXME: Do something about the IRQ handler as well. I think that
       we can do without any IRQ handler at all. */

    return FALSE;
  }
  else
  {
    /* One or more devices found on this interface. */

    return TRUE;
  }
}
