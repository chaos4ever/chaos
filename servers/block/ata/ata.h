/* $Id$ */
/* Abstract: ATA driver */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Anders Ohrt <doa@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development */

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
   USA */

#ifndef __ATA_H__
#define __ATA_H__

#include <system/system.h>
#include "config.h"

#define BIT_VALUE(n) (1 << (n))

/* ATA registers. */

#define REGISTER_DATA 0
#define REGISTER_ERROR 1
#define REGISTER_FEATURE REGISTER_ERROR
#define REGISTER_SECTOR_COUNT 2
#define REGISTER_SECTOR_NUMBER 3
#define REGISTER_CYLINDER_LOW 4
#define REGISTER_CYLINDER_HIGH 5
#define REGISTER_DEVICE_HEAD 6
#define REGISTER_COMMAND 7
#define REGISTER_STATUS REGISTER_COMMAND

#define REGISTER_DEVICE_CONTROL 0x206
#define REGISTER_ALTERNATE_STATUS REGISTER_DEVICE_CONTROL

/* ATA commands. */

#define COMMAND_READ_SECTORS_RETRY               0x20
#define COMMAND_READ_SECTORS_NO_RETRY            0x21
#define COMMAND_IDENTIFY_DEVICE                  0xEC

/* Internal ATA device structure. */

typedef struct
{
  u16 id[256];

  char model_number[41];
  char serial_number[21];
  bool lba;
  bool dma;
  u32 number_of_sectors;
  u32 number_of_heads;
  u32 sectors_per_track;
  u32 number_of_cylinders;

  unsigned int type;
} device_type;

/* Internal ATA interface structure. */

typedef struct
{
  unsigned int irq;
  unsigned int io_base;
  mailbox_id_type mailbox_id;
  device_type *device[2];
} interface_type;

/* Service structure. */

typedef struct
{
  /* Which ATA device is this partition/disk located on? */

  interface_type *interface;
  unsigned int device;

  /* Where on this ATA device is it located and how big is it? */

  u32 start_sector;
  u32 number_of_sectors;
} service_type;

/* Internal ATA device types. */

enum
{
  DEVICE_TYPE_NONE,
  DEVICE_TYPE_UNKNOWN,
  DEVICE_TYPE_ATA
};

/* Device-head register bits. */

#define REGISTER_DEVICE_HEAD_DEFAULT    (BIT_VALUE (5) | BIT_VALUE (7))
#define DEVICE_MASTER                   (REGISTER_DEVICE_HEAD_DEFAULT)
#define DEVICE_SLAVE                    (REGISTER_DEVICE_HEAD_DEFAULT | \
                                         BIT_VALUE (4))
#define BIT_LBA                         BIT_VALUE (6)

/* Device-control register bits. */

#define REGISTER_DEVICE_CONTROL_DEFAULT 0
#define BIT_NIEN                        BIT_VALUE (1)
#define BIT_SRST                        BIT_VALUE (2)

/* Alternate status register bits. */

#define BIT_BSY                         BIT_VALUE (7)
#define BIT_DRDY                        BIT_VALUE (6)

/* Error register bits. */

#define BIT_ABRT                        BIT_VALUE (2)

/* Status register bits. */

#define BIT_ERR                         BIT_VALUE (0)

/* Size of the ATA identify structure. */

#define ATA_SECTOR_WORDS                256

/* ATA device types. */

enum
{
  ATA_DEVICE_NONE,
  ATA_DEVICE_ATA,
  ATA_DEVICE_ATAPI
};

/* External functions and variables. */

extern log_structure_type log_structure;
extern tag_type empty_tag;

extern void handle_connection (mailbox_id_type reply_mailbox_id,
                               service_type *service);
extern void handle_service (service_type *service);

extern bool ata_init_interface (interface_type *interface);

extern bool ata_read_sectors (interface_type *interface, unsigned int device,
                              u32 sector_number, unsigned int number_of_sectors,
                              void *buffer);

#endif /* !__ATA_H__ */
