/* $Id$ */
/* Abstract: Type definitions and function prototypes for the PCI
   server. */
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
   USA */

#ifndef __PCI_H__
#define __PCI_H__

#include "config.h"

/* Under PCI, each device has 256 bytes of configuration address space,
   of which the first 64 bytes are standardized as follows: */
/* 16 bits. */

#define PCI_VENDOR_ID                   0x00

/* 16 bits. */

#define PCI_DEVICE_ID                   0x02

/* 16 bits. */

#define PCI_COMMAND                     0x04

/* Enable response in I/O space. */

#define  PCI_COMMAND_IO                 0x1

/* Enable response in Memory space. */

#define  PCI_COMMAND_MEMORY             0x2

/* Enable bus mastering. */

#define  PCI_COMMAND_MASTER             0x4

/* Enable response to special cycles. */

#define  PCI_COMMAND_SPECIAL            0x8

/* Use memory write and invalidate. */

#define  PCI_COMMAND_INVALIDATE         0x10

/* Enable palette snooping. */

#define  PCI_COMMAND_VGA_PALETTE        0x20

/* Enable parity checking. */

#define  PCI_COMMAND_PARITY             0x40

/* Enable address/data stepping. */

#define  PCI_COMMAND_WAIT               0x80

/* Enable SERR. */

#define  PCI_COMMAND_SERR               0x100

/* Enable back-to-back writes. */

#define  PCI_COMMAND_FAST_BACK          0x200

/* 16 bits. */

#define PCI_STATUS                      0x06

/* Support Capability List. */

#define  PCI_STATUS_CAPABILITY_LIST	0x10

/* Support 66 Mhz PCI 2.1 bus. */

#define  PCI_STATUS_66MHZ               0x20

/* Support User Definable Features [obsolete]. */

#define  PCI_STATUS_UDF                 0x40

/* Accept fast-back to back. */

#define  PCI_STATUS_FAST_BACK           0x80

/* Detected parity error. */

#define  PCI_STATUS_PARITY              0x100

/* DEVSEL timing. */

#define  PCI_STATUS_DEVSEL_MASK         0x600
#define  PCI_STATUS_DEVSEL_FAST         0x000	
#define  PCI_STATUS_DEVSEL_MEDIUM       0x200
#define  PCI_STATUS_DEVSEL_SLOW         0x400

/* Set on target abort. */

#define  PCI_STATUS_SIG_TARGET_ABORT    0x800

/* Master ack of target abort. */

#define  PCI_STATUS_REC_TARGET_ABORT    0x1000

/* Set on master abort. */

#define  PCI_STATUS_REC_MASTER_ABORT    0x2000

/* Set when we drive SERR. */

#define  PCI_STATUS_SIG_SYSTEM_ERROR    0x4000

/* Set on parity error. */

#define  PCI_STATUS_DETECTED_PARITY     0x8000

/* High 24 bits are class, low 8 revision. */

#define PCI_CLASS_REVISION              0x08

/* Revision ID. */

#define PCI_REVISION_ID                 0x08

/* Reg. Level Programming Interface. */

#define PCI_CLASS_PROG                  0x09

/* Device class. */

#define PCI_CLASS_DEVICE                0x0A

/* 8 bits. */

#define PCI_CACHE_LINE_SIZE             0x0C

/* 8 bits. */

#define PCI_LATENCY_TIMER               0x0D

/* 8 bits. */

#define PCI_HEADER_TYPE                 0x0E
#define  PCI_HEADER_TYPE_NORMAL         0
#define  PCI_HEADER_TYPE_BRIDGE         1
#define  PCI_HEADER_TYPE_CARDBUS        2

/* 8 bits. */

#define PCI_BIST                        0x0F

/* Return result. */

#define PCI_BIST_CODE_MASK              0x0F

/* 1 to start BIST, 2 secs or less. */

#define PCI_BIST_START                  0x40

/* 1 if BIST capable. */

#define PCI_BIST_CAPABLE                0x80

/* Base addresses specify locations in memory or I/O space.  Decoded
   size can be determined by writing a value of 0xFFFFFFFF to the
   register, and reading it back.  Only 1 bits are decoded. */
/* 32 bits. */

#define PCI_BASE_ADDRESS_0              0x10

/* 32 bits [htype 0,1 only]. */

#define PCI_BASE_ADDRESS_1              0x14

/* 32 bits [htype 0 only]. */

#define PCI_BASE_ADDRESS_2              0x18

/* 32 bits. */

#define PCI_BASE_ADDRESS_3              0x1C

/* 32 bits. */

#define PCI_BASE_ADDRESS_4              0x20

/* 32 bits. */

#define PCI_BASE_ADDRESS_5              0x24

/* 0 = memory, 1 = I/O. */

#define  PCI_BASE_ADDRESS_SPACE         0x01
#define  PCI_BASE_ADDRESS_SPACE_IO      0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY  0x00
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06

/* 32 bit address. */

#define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00

/* Below 1M [obsolete]. */

#define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02

/* 64 bit address. */

#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04

/* Prefetchable? */

#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0FUL)
#define  PCI_BASE_ADDRESS_IO_MASK	(~0x03UL)

/* Bit 1 is reserved if address_space = 1. */

/* Header type 0 (normal devices) */

#define PCI_CARDBUS_CIS                 0x28
#define PCI_SUBSYSTEM_VENDOR_ID         0x2C
#define PCI_SUBSYSTEM_ID                0x2E

/* Bits 31..11 are address, 10..1 reserved. */

#define PCI_ROM_ADDRESS                 0x30
#define  PCI_ROM_ADDRESS_ENABLE         0x01
#define PCI_ROM_ADDRESS_MASK            (~0x7FFUL)

/* Offset of first capability list entry. */

#define PCI_CAPABILITY_LIST             0x34

/* 0x35-0x3B are reserved. */
/* 8 bits. */

#define PCI_INTERRUPT_LINE              0x3C

/* 8 bits. */

#define PCI_INTERRUPT_PIN               0x3D

/* 8 bits. */

#define PCI_MINIMUM_GRANULARITY         0x3E

/* 8 bits. */

#define PCI_MAXIMUM_LATENCY             0x3F

/* Header type 1 (PCI-to-PCI bridges). */
/* Primary bus number. */

#define PCI_PRIMARY_BUS                 0x18

/* Secondary bus number. */

#define PCI_SECONDARY_BUS               0x19

/* Highest bus number behind the bridge. */

#define PCI_SUBORDINATE_BUS             0x1A

/* Latency timer for secondary interface. */

#define PCI_SEC_LATENCY_TIMER           0x1B

/* I/O range behind the bridge. */

#define PCI_IO_BASE                     0x1C
#define PCI_IO_LIMIT                    0x1D

/* I/O bridging type. */

#define  PCI_IO_RANGE_TYPE_MASK         0x0F
#define  PCI_IO_RANGE_TYPE_16           0x00
#define  PCI_IO_RANGE_TYPE_32           0x01
#define  PCI_IO_RANGE_MASK              ~0x0F

/* Secondary status register, only bit 14 used. */

#define PCI_SEC_STATUS                  0x1E

/* Memory range behind. */

#define PCI_MEMORY_BASE                 0x20
#define PCI_MEMORY_LIMIT                0x22
#define  PCI_MEMORY_RANGE_TYPE_MASK     0x0F
#define  PCI_MEMORY_RANGE_MASK          ~0x0F

/* Prefetchable memory range behind. */

#define PCI_PREF_MEMORY_BASE            0x24
#define PCI_PREF_MEMORY_LIMIT           0x26
#define  PCI_PREF_RANGE_TYPE_MASK       0x0F
#define  PCI_PREF_RANGE_TYPE_32         0x00
#define  PCI_PREF_RANGE_TYPE_64         0x01
#define  PCI_PREF_RANGE_MASK            ~0x0F

/* Upper half of prefetchable memory range. */

#define PCI_PREF_BASE_UPPER32           0x28
#define PCI_PREF_LIMIT_UPPER32          0x2C

/* Upper half of I/O addresses. */

#define PCI_IO_BASE_UPPER16             0x30
#define PCI_IO_LIMIT_UPPER16            0x32

/* 0x34 same as for header type 0. */
/* 0x35-0x3B is reserved. */

/* Same as PCI_ROM_ADDRESS, but for header type 1. */

#define PCI_ROM_ADDRESS1                0x38

/* 0x3C-0x3D are same as for header type 0. */

#define PCI_BRIDGE_CONTROL              0x3E

/* Enable parity detection on secondary interface. */

#define  PCI_BRIDGE_CONTROL_PARITY	0x01

/* The same for SERR forwarding. */

#define  PCI_BRIDGE_CONTROL_SERR	0x02

/* Disable bridging of ISA ports. */

#define  PCI_BRIDGE_CONTROL_NO_ISA	0x04

/* Forward VGA addresses. */

#define  PCI_BRIDGE_CONTROL_VGA         0x08

/* Report master aborts. */

#define  PCI_BRIDGE_CONTROL_MASTER_ABORT 0x20

/* Secondary bus reset. */

#define  PCI_BRIDGE_CONTROL_BUS_RESET   0x40

/* Fast Back2Back enabled on secondary interface. */

#define  PCI_BRIDGE_CONTROL_FAST_BACK   0x80

/* Header type 2 (CardBus bridges). */

#define PCI_CARDBUS_CAPABILITY_LIST	0x14

/* 0x15 reserved. */
/* Secondary status. */

#define PCI_CARDBUS_SEC_STATUS          0x16

/* PCI bus number. */

#define PCI_CARDBUS_PRIMARY_BUS         0x18

/* CardBus bus number. */

#define PCI_CARDBUS_CARD_BUS		0x19

/* Subordinate bus number. */

#define PCI_CARDBUS_SUBORDINATE_BUS	0x1A

/* CardBus latency timer. */

#define PCI_CARDBUS_LATENCY_TIMER	0x1B
#define PCI_CARDBUS_MEMORY_BASE_0	0x1C
#define PCI_CARDBUS_MEMORY_LIMIT_0	0x20
#define PCI_CARDBUS_MEMORY_BASE_1	0x24
#define PCI_CARDBUS_MEMORY_LIMIT_1	0x28
#define PCI_CARDBUS_IO_BASE_0           0x2C
#define PCI_CARDBUS_IO_BASE_0_HI	0x2E
#define PCI_CARDBUS_IO_LIMIT_0          0x30
#define PCI_CARDBUS_IO_LIMIT_0_HI	0x32
#define PCI_CARDBUS_IO_BASE_1           0x34
#define PCI_CARDBUS_IO_BASE_1_HI	0x36
#define PCI_CARDBUS_IO_LIMIT_1          0x38
#define PCI_CARDBUS_IO_LIMIT_1_HI	0x3A
#define  PCI_CARDBUS_IO_RANGE_MASK	~0x03

/* 0x3C-0x3D are same as for header type 0. */

#define PCI_CARDBUS_BRIDGE_CONTROL	0x3E

/* Similar to standard bridge control register. */

#define  PCI_CARDBUS_BRIDGE_CONTROL_PARITY      0x01
#define  PCI_CARDBUS_BRIDGE_CONTROL_SERR	0x02
#define  PCI_CARDBUS_BRIDGE_CONTROL_ISA		0x04
#define  PCI_CARDBUS_BRIDGE_CONTROL_VGA		0x08
#define  PCI_CARDBUS_BRIDGE_CONTROL_MASTER_ABORT 0x20

/* CardBus reset. */

#define  PCI_CARDBUS_BRIDGE_CONTROL_CARDBUS_RESET 0x40

/* Enable interrupt for 16-bit cards. */

#define  PCI_CARDBUS_BRIDGE_CONTROL_16BIT_INT   0x80

/* Prefetch enable for both memory regions. */

#define  PCI_CARDBUS_BRIDGE_CONTROL_PREFETCH_MEMORY0 0x100
#define  PCI_CARDBUS_BRIDGE_CONTROL_PREFETCH_MEMORY1 0x200
#define  PCI_CARDBUS_BRIDGE_CONTROL_POST_WRITES	0x400
#define PCI_CARDBUS_SUBSYSTEM_VENDOR_ID         0x40
#define PCI_CARDBUS_SUBSYSTEM_ID                0x42

/* 16-bit PC Card legacy mode base address (ExCa). */

#define PCI_CARDBUS_LEGACY_MODE_BASE            0x44

/* 0x48-0x7F reserved. */

/* Capability lists. */
/* Capability ID. */

#define PCI_CAPABILITY_LIST_ID                  0

/* Power Management. */

#define  PCI_CAPABILITY_ID_PM                   0x01

/* Accelerated Graphics Port. */

#define  PCI_CAPABILITY_ID_AGP                  0x02

/* Vital Product Data. */

#define  PCI_CAPABILITY_ID_VPD                  0x03

/* Slot Identification. */

#define  PCI_CAPABILITY_ID_SLOTID               0x04

/* Message Signalled Interrupts. */

#define  PCI_CAPABILITY_ID_MSI                  0x05

/* CompactPCI HotSwap. */

#define  PCI_CAPABILITY_ID_CHSWP                0x06

/* Next capability in the list. */

#define PCI_CAPABILITY_LIST_NEXT                1

/* Capability defined flags (16 bits). */

#define PCI_CAPABILITY_FLAGS                    2
#define PCI_CAPABILITY_SIZEOF                   4

/* Power Management Registers. */
/* Version. */

#define  PCI_PM_CAPABILITY_VER_MASK             0x0007

/* PME clock required. */

#define  PCI_PM_CAPABILITY_PME_CLOCK            0x0008

/* Auxilliary power support. */

#define  PCI_PM_CAPABILITY_AUX_POWER            0x0010

/* Device specific initialization. */

#define  PCI_PM_CAPABILITY_DSI                  0x0020

/* D1 power state support. */

#define  PCI_PM_CAPABILITY_D1                   0x0200

/* D2 power state support. */

#define  PCI_PM_CAPABILITY_D2                   0x0400

/* PME pin supported. */

#define  PCI_PM_CAPABILITY_PME                  0x0800

/* PM control and status register. */

#define PCI_PM_CONTROL                          4

/* Current power state (D0 to D3). */

#define  PCI_PM_CONTROL_STATE_MASK              0x0003

/* PME pin enable. */

#define  PCI_PM_CONTROL_PME_ENABLE              0x0100

/* Data select (??). */

#define  PCI_PM_CONTROL_DATA_SEL_MASK           0x1E00

/* Data scale (??). */

#define  PCI_PM_CONTROL_DATA_SCALE_MASK         0x6000

/* PME pin status. */

#define  PCI_PM_CONTROL_PME_STATUS              0x8000

/* PPB support extensions (??). */

#define PCI_PM_PPB_EXTENSIONS                   6

/* Stop clock when in D3hot (??). */

#define  PCI_PM_PPB_B2_B3                       0x40

/* Bus power/clock control enable (??). */

#define  PCI_PM_BPCC_ENABLE                     0x80

/* (??) */

#define PCI_PM_DATA_REGISTER                    7
#define PCI_PM_SIZEOF                           8

/* AGP registers. */
/* BCD version number. */

#define PCI_AGP_VERSION                         2

/* Rest of capability flags. */

#define PCI_AGP_RFU                             3

/* Status register. */

#define PCI_AGP_STATUS                          4

/* Maximum number of requests - 1. */

#define  PCI_AGP_STATUS_RQ_MASK                 0xFF000000

/* Sideband addressing supported. */

#define  PCI_AGP_STATUS_SBA                     0x0200

/* 64-bit addressing supported. */

#define  PCI_AGP_STATUS_64BIT                   0x0020

/* FW transfers supported. */

#define  PCI_AGP_STATUS_FW                      0x0010

/* 4x transfer rate supported. */

#define  PCI_AGP_STATUS_RATE4                   0x0004

/* 2x transfer rate supported. */

#define  PCI_AGP_STATUS_RATE2                   0x0002

/* 1x transfer rate supported. */

#define  PCI_AGP_STATUS_RATE1                   0x0001

/* Control register. */

#define PCI_AGP_COMMAND                         8

/* Master: Maximum number of requests. */

#define  PCI_AGP_COMMAND_RQ_MASK                0xFF000000

/* Sideband addressing enabled. */

#define  PCI_AGP_COMMAND_SBA                    0x0200

/* Allow processing of AGP transactions. */

#define  PCI_AGP_COMMAND_AGP                    0x0100

/* Allow processing of 64-bit addresses. */

#define  PCI_AGP_COMMAND_64BIT                  0x0020

/* Force FW transfers. */

#define  PCI_AGP_COMMAND_FW                     0x0010

/* Use 4x rate. */

#define  PCI_AGP_COMMAND_RATE4                  0x0004

/* Use 2x rate. */

#define  PCI_AGP_COMMAND_RATE2                  0x0002

/* Use 1x rate. */

#define  PCI_AGP_COMMAND_RATE1                  0x0001
#define PCI_AGP_SIZEOF                          12

/* Slot Identification. */
/* Expansion Slot Register. */

#define PCI_SID_ESR                             2

/* Number of expansion slots available. */

#define  PCI_SID_ESR_NSLOTS                     0x1F

/* First In Chassis Flag. */

#define  PCI_SID_ESR_FIC                        0x20

/* Chassis Number. */

#define PCI_SID_CHASSIS_NR                      3

/* Message Signalled Interrupts registers. */
/* Various flags. */

#define PCI_MSI_FLAGS                           2

/* 64-bit addresses allowed. */

#define  PCI_MSI_FLAGS_64BIT                    0x80

/* Message queue size configured. */

#define  PCI_MSI_FLAGS_QSIZE                    0x70

/* Maximum queue size available. */

#define  PCI_MSI_FLAGS_QMASK                    0x0E

/* MSI feature enabled. */

#define  PCI_MSI_FLAGS_ENABLE                   0x01

/* Rest of capability flags. */

#define PCI_MSI_RFU                             3

/* Lower 32 bits. */

#define PCI_MSI_ADDRESS_LO                      4

/* Upper 32 bits (if PCI_MSI_FLAGS_64BIT set). */

#define PCI_MSI_ADDRESS_HI                      8

/* 16 bits of data for 32-bit devices. */

#define PCI_MSI_DATA_32                         8

/* 16 bits of data for 64-bit devices. */

#define PCI_MSI_DATA_64                         12

#define PCI_BASE                                0xCF8
#define PCI_DATA                                (PCI_BASE + 4)
#define PCI_PORTS                               8
#define PCI_DEVICE_COUNT_COMPATIBLE             4
#define PCI_DEVICE_COUNT_IRQ                    2

/* Register numbers. */
/* Type 0 devices. */

#define PCI_REGISTER_MAPS                       0x10

/* Capability lists. */
/* Capability ID. */

#define PCI_CAPABILITY_LIST_ID                  0

/* Power Management. */

#define PCI_CAPABILITY_ID_PM                    0x01

/* Accelerated Graphics Port. */

#define PCI_CAPABILITY_ID_AGP                   0x02

/* Next capability in the list. */

#define PCI_CAPABILITY_LIST_NEXT                1

/* Device classes and subclasses. */

#define PCI_CLASS_NOT_DEFINED                   0x0000
#define PCI_CLASS_NOT_DEFINED_VGA               0x0001

#define PCI_BASE_CLASS_STORAGE                  0x01
#define PCI_CLASS_STORAGE_SCSI                  0x0100
#define PCI_CLASS_STORAGE_IDE                   0x0101
#define PCI_CLASS_STORAGE_FLOPPY                0x0102
#define PCI_CLASS_STORAGE_IPI                   0x0103
#define PCI_CLASS_STORAGE_RAID                  0x0104
#define PCI_CLASS_STORAGE_OTHER                 0x0180

#define PCI_BASE_CLASS_NETWORK                  0x02
#define PCI_CLASS_NETWORK_ETHERNET              0x0200
#define PCI_CLASS_NETWORK_TOKEN_RING            0x0201
#define PCI_CLASS_NETWORK_FDDI                  0x0202
#define PCI_CLASS_NETWORK_ATM                   0x0203
#define PCI_CLASS_NETWORK_OTHER                 0x0280

#define PCI_BASE_CLASS_DISPLAY                  0x03
#define PCI_CLASS_DISPLAY_VGA                   0x0300
#define PCI_CLASS_DISPLAY_XGA                   0x0301
#define PCI_CLASS_DISPLAY_OTHER                 0x0380

#define PCI_BASE_CLASS_MULTIMEDIA               0x04
#define PCI_CLASS_MULTIMEDIA_VIDEO              0x0400
#define PCI_CLASS_MULTIMEDIA_AUDIO              0x0401
#define PCI_CLASS_MULTIMEDIA_OTHER              0x0480

#define PCI_BASE_CLASS_MEMORY                   0x05
#define PCI_CLASS_MEMORY_RAM                    0x0500
#define PCI_CLASS_MEMORY_FLASH                  0x0501
#define PCI_CLASS_MEMORY_OTHER                  0x0580

#define PCI_BASE_CLASS_BRIDGE                   0x06
#define PCI_CLASS_BRIDGE_HOST                   0x0600
#define PCI_CLASS_BRIDGE_ISA                    0x0601
#define PCI_CLASS_BRIDGE_EISA                   0x0602
#define PCI_CLASS_BRIDGE_MC                     0x0603
#define PCI_CLASS_BRIDGE_PCI                    0x0604
#define PCI_CLASS_BRIDGE_PCMCIA                 0x0605
#define PCI_CLASS_BRIDGE_NUBUS                  0x0606
#define PCI_CLASS_BRIDGE_CARDBUS                0x0607
#define PCI_CLASS_BRIDGE_OTHER                  0x0680

#define PCI_BASE_CLASS_COMMUNICATION            0x07
#define PCI_CLASS_COMMUNICATION_SERIAL          0x0700
#define PCI_CLASS_COMMUNICATION_PARALLEL        0x0701
#define PCI_CLASS_COMMUNICATION_OTHER           0x0780

#define PCI_BASE_CLASS_SYSTEM                   0x08
#define PCI_CLASS_SYSTEM_PIC                    0x0800
#define PCI_CLASS_SYSTEM_DMA                    0x0801
#define PCI_CLASS_SYSTEM_TIMER                  0x0802
#define PCI_CLASS_SYSTEM_RTC                    0x0803
#define PCI_CLASS_SYSTEM_OTHER                  0x0880

#define PCI_BASE_CLASS_INPUT                    0x09
#define PCI_CLASS_INPUT_KEYBOARD                0x0900
#define PCI_CLASS_INPUT_PEN                     0x0901
#define PCI_CLASS_INPUT_MOUSE                   0x0902
#define PCI_CLASS_INPUT_OTHER                   0x0980

#define PCI_BASE_CLASS_DOCKING                  0x0A
#define PCI_CLASS_DOCKING_GENERIC               0x0A00
#define PCI_CLASS_DOCKING_OTHER                 0x0A01

#define PCI_BASE_CLASS_PROCESSOR                0x0B
#define PCI_CLASS_PROCESSOR_386                 0x0B00
#define PCI_CLASS_PROCESSOR_486                 0x0B01
#define PCI_CLASS_PROCESSOR_PENTIUM             0x0B02
#define PCI_CLASS_PROCESSOR_ALPHA               0x0B10
#define PCI_CLASS_PROCESSOR_POWERPC             0x0B20
#define PCI_CLASS_PROCESSOR_CO                  0x0B40

#define PCI_BASE_CLASS_SERIAL                   0x0C
#define PCI_CLASS_SERIAL_FIREWIRE               0x0C00
#define PCI_CLASS_SERIAL_ACCESS                 0x0C01
#define PCI_CLASS_SERIAL_SSA                    0x0C02
#define PCI_CLASS_SERIAL_USB                    0x0C03
#define PCI_CLASS_SERIAL_FIBER                  0x0C04

#define PCI_CLASS_HOT_SWAP_CONTROLLER           0xFF00

#define PCI_CLASS_OTHERS                        0xFF

/* The PCI interface treats multi-function devices as independent
   devices.  The slot/function address of each device is encoded in a
   single byte as follows:
 
     Bit 7-3: slot
     Bit 2-0: function */

#define PCI_DEVFN(slot,func) \
  ((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(device_function) \
  (((device_function) >> 3) & 0x1f)
#define PCI_FUNC(device_function) \
  ((device_function) & 0x07)

/* For PCI devices, the region numbers are assigned this way:
 
 	0-5	standard PCI regions.
 	6	expansion ROM.
 	7-10    bridges: address space assigned to buses behind the
                bridge. */

#define PCI_ROM_RESOURCE        6
#define PCI_BRIDGE_RESOURCES    7
  
/* These bits of resource flags tell us the PCI region flags. */

#define PCI_REGION_FLAG_MASK    0x0F

typedef struct
{
  /* Node in list of buses. */

  struct pci_bus_type *previous;
  struct pci_bus_type *next;

  /* List of devices on this bus. */

  struct pci_device_type *devices;

  /* Bridge device as seen by parent. */

  struct pci_device_type *self;

  /* Bus number. */
  
  unsigned char	number;

  /* Number of primary bridge. */

  unsigned char	primary;

  /* Number of secondary bridge. */

  unsigned char	secondary;

  /* Max number of subordinate buses. */

  unsigned char	subordinate;

  /* Pointer to the PCI functions we should use when accessing this
     bus. */
  
  struct pci_operation_type *operation;

  char name[48];
  u16 vendor_id;
  u16 device_id;

  /* Serial number. */

  unsigned int serial;

  /* Plug & Play version. */

  unsigned char	pnpver;

  /* Product version. */

  unsigned char	productver;

  /* If zero - checksum passed. */

  unsigned char	checksum;
  unsigned char	pad1;
} pci_bus_type;

typedef struct
{
  /* Node in list of all PCI devices. */

  struct pci_device_type *next;
  struct pci_device_type *previous;

  /* Bus this device is on. */

  pci_bus_type *bus;

  /* Bus this device bridges to. */

  pci_bus_type *subordinate;
  
  /* Encoded device & function index (7 bits device, 3 bits function). */

  unsigned int device_function;

  u16 vendor_id;
  u16 device_id;
  u16 subsystem_vendor_id;
  u16 subsystem_device_id;

  /* 3 bytes: (base, sub, prog-if) */

  u32 class;

  /* PCI header type (`multi' flag masked out). */

  u8 header_type;

  /* Which config register controls the ROM? */

  u8 rom_base_reg;
  
  /* Device is compatible with these IDs. */

  u16 vendor_compatible[PCI_DEVICE_COUNT_COMPATIBLE];
  u16 device_compatible[PCI_DEVICE_COUNT_COMPATIBLE];
  
  /* The IRQ line this device is using, if any. */
  
  unsigned int irq;
  
  /* I/O and memory regions + expansion ROMs. */

  pci_resource_type resource[PCI_NUMBER_OF_RESOURCES];

  /* Device name. */

  char name[80];

  /* Slot name. */

  char slot_name[8];
} pci_device_type;

typedef struct
{
  u8 (*read_u8)(pci_device_type *, int where);
  u16 (*read_u16)(pci_device_type *, int where);
  u32 (*read_u32)(pci_device_type *, int where);
  void (*write_u8)(pci_device_type *, int where, u8 value);
  void (*write_u16)(pci_device_type *, int where, u16 value);
  void (*write_u32)(pci_device_type *, int where, u32 value);
} pci_operation_type;

typedef struct
{
  u16 device_id;
  u16 vendor_id;
  char *name;
} pci_device_id_type;

typedef struct
{
  u16 vendor_id;
  char *name;
} pci_vendor_id_type;

#endif /* !__PCI_H__ */
