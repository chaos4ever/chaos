/* $Id$ */
/* Abstract: DEC 21x4x driver. */
/* Author: Anders Ohrt <doa@chaosdev.org> */

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

/* Partially based on the Linux driver, written by Donald Becker. */

#include "config.h"
#include "types.h"
#include "defines.h"
#include "functions.h"
#include "enumerations.h"

static char version[] =
"Linux Tulip driver version 0.9.9 (August 11, 2000)\n";


/* A few user-configurable values. */

/* Maximum events (Rx packets, etc.) to handle at each interrupt. */
static int max_interrupt_work = 25;

#define MAX_UNITS 8

/* Used to pass the full-duplex flag, etc. */

static int full_duplex[MAX_UNITS] = {0, };
static int options[MAX_UNITS] = {0, };

/* Jumbo MTU for interfaces. */

static int mtu[MAX_UNITS] = {0, };


/*  The possible media types that can be set in options[] are: */

const char * const medianame[] = {
  "10baseT", "10base2", "AUI", "100baseTx",
  "10baseT-FD", "100baseTx-FD", "100baseT4", "100baseFx",
  "100baseFx-FD", "MII 10baseT", "MII 10baseT-FD", "MII",
  "10baseT(forced)", "MII 100baseTx", "MII 100baseTx-FD", "MII 100baseT4",
};

/* Set the copy breakpoint for the copy-only-tiny-buffer Rx structure. */

#if defined(__alpha__) || defined(__arm__) || defined(__hppa__) \
	|| defined(__sparc_)
static int rx_copybreak = 1518;
#else
static int rx_copybreak = 100;
#endif

/*
  Set the bus performance register.
	Typical: Set 16 longword cache alignment, no burst limit.
	Cache alignment bits 15:14	     Burst length 13:8
		0000	No alignment  0x00000000 unlimited		0800 8 longwords
		4000	8  longwords		0100 1 longword		1000 16 longwords
		8000	16 longwords		0200 2 longwords	2000 32 longwords
		C000	32  longwords		0400 4 longwords
	Warning: many older 486 systems are broken and require setting 0x00A04800
	   8 longword cache alignment, 8 longword burst.
	ToDo: Non-Intel setting could be better.
*/

#if defined(__alpha__)
static int csr0 = 0x01A00000 | 0xE000;
#elif defined(__i386__) || defined(__powerpc__) || defined(__hppa__)
static int csr0 = 0x01A00000 | 0x8000;
#elif defined(__sparc__)

/* The UltraSparc PCI controllers will disconnect at every 64-byte
   crossing anyways so it makes no sense to tell Tulip to burst any
   more than that. */

static int csr0 = 0x01A00000 | 0x9000;
#elif defined(__arm__)
static int csr0 = 0x01A00000 | 0x4800;
#else
#warning Processor architecture undefined!
static int csr0 = 0x00A00000 | 0x4800;
#endif

/* Operational parameters that usually are not changed. */

/* Time in jiffies before concluding the transmitter is hung. */

#define TX_TIMEOUT  (4*HZ)

/* Fixme: Linux module crap. Remove? */
/*
  MODULE_AUTHOR("The Linux Kernel Team");
  MODULE_DESCRIPTION("Digital 21*4* Tulip ethernet driver");
  MODULE_PARM(tulip_debug, "i");
  MODULE_PARM(max_interrupt_work, "i");
  MODULE_PARM(rx_copybreak, "i");
  MODULE_PARM(csr0, "i");
  MODULE_PARM(options, "1-" __MODULE_STRING(MAX_UNITS) "i");
  MODULE_PARM(full_duplex, "1-" __MODULE_STRING(MAX_UNITS) "i");
*/

#define TULIP_MODULE_NAME "tulip"
#define PFX TULIP_MODULE_NAME ": "

#ifdef TULIP_DEBUG
int tulip_debug = TULIP_DEBUG;
#else
int tulip_debug = 1;
#endif



/*
 * This table use during operation for capabilities and media timer.
 *
 * It is indexed via the values in 'enum chips'
 */

tulip_chip_table tulip_tbl[] =
{
  {
    "Digital DC21040 Tulip", 128, 0x0001ebef, 0, tulip_timer
  },
  {
    "Digital DC21041 Tulip", 128, 0x0001ebef,
    HAS_MEDIA_TABLE | HAS_NWAY, tulip_timer
  },
  {
    "Digital DS21140 Tulip", 128, 0x0001ebef,
    HAS_MII | HAS_MEDIA_TABLE | CSR12_IN_SROM, tulip_timer
  },
  {
    "Digital DS21143 Tulip", 128, 0x0801fbff,
    HAS_MII | HAS_MEDIA_TABLE | ALWAYS_CHECK_MII | HAS_ACPI | HAS_NWAY |
    HAS_INTR_MITIGATION, t21142_timer
  },
  {
    "Lite-On 82c168 PNIC", 256, 0x0001ebef,
    HAS_MII | HAS_PNICNWAY, pnic_timer
  },
  {
    "Macronix 98713 PMAC", 128, 0x0001ebef,
    HAS_MII | HAS_MEDIA_TABLE | CSR12_IN_SROM, mxic_timer
  },
  {
    "Macronix 98715 PMAC", 256, 0x0001ebef,
    HAS_MEDIA_TABLE, mxic_timer
  },
  {
    "Macronix 98725 PMAC", 256, 0x0001ebef,
    HAS_MEDIA_TABLE, mxic_timer
  },
  {
    "ASIX AX88140", 128, 0x0001fbff,
    HAS_MII | HAS_MEDIA_TABLE | CSR12_IN_SROM | MULTICAST_HASH_ONLY | IS_ASIX, tulip_timer
  },
  {
    "Lite-On PNIC-II", 256, 0x0801fbff,
    HAS_MII | HAS_NWAY | HAS_8023X, t21142_timer
  },
  {
    "ADMtek Comet", 256, 0x0001abef,
    MULTICAST_HASH_ONLY, comet_timer
  },
  {
    "Compex 9881 PMAC", 128, 0x0001ebef,
    HAS_MII | HAS_MEDIA_TABLE | CSR12_IN_SROM, mxic_timer
  },
  {
    "Intel DS21145 Tulip", 128, 0x0801fbff,
    HAS_MII | HAS_MEDIA_TABLE | ALWAYS_CHECK_MII | HAS_ACPI | HAS_NWAY,
    t21142_timer
  },
  {
    NULL, 0, 0, 0, NULL
  }
};


static pci_device_id tulip_pci_tabel[] =
{
  {
    0x1011, 0x0002, PCI_ANY_ID, PCI_ANY_ID, 0, 0, DC21040
  },
  {
    0x1011, 0x0014, PCI_ANY_ID, PCI_ANY_ID, 0, 0, DC21041
  },
  {
    0x1011, 0x0009, PCI_ANY_ID, PCI_ANY_ID, 0, 0, DC21140
  },
  {
    0x1011, 0x0019, PCI_ANY_ID, PCI_ANY_ID, 0, 0, DC21143
  },
  {
    0x11AD, 0x0002, PCI_ANY_ID, PCI_ANY_ID, 0, 0, LC82C168
  },
  {
    0x10d9, 0x0512, PCI_ANY_ID, PCI_ANY_ID, 0, 0, MX98713
  },
  {
    0x10d9, 0x0531, PCI_ANY_ID, PCI_ANY_ID, 0, 0, MX98715
  },
  
  /*
    {
    0x10d9, 0x0531, PCI_ANY_ID, PCI_ANY_ID, 0, 0, MX98725
    },
  */

  {
    0x125B, 0x1400, PCI_ANY_ID, PCI_ANY_ID, 0, 0, AX88140
  },
  {
    0x11AD, 0xc115, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PNIC2
  },
  {
    0x1317, 0x0981, PCI_ANY_ID, PCI_ANY_ID, 0, 0, COMET
  },
  {
    0x1317, 0x0985, PCI_ANY_ID, PCI_ANY_ID, 0, 0, COMET
  },
  {
    0x1317, 0x1985, PCI_ANY_ID, PCI_ANY_ID, 0, 0, COMET
  },
  {
    0x13D1, 0xAB02, PCI_ANY_ID, PCI_ANY_ID, 0, 0, COMET
  },
  {
    0x13D1, 0xAB03, PCI_ANY_ID, PCI_ANY_ID, 0, 0, COMET
  },
  {
    0x11F6, 0x9881, PCI_ANY_ID, PCI_ANY_ID, 0, 0, COMPEX9881
  },
  {
    0x8086, 0x0039, PCI_ANY_ID, PCI_ANY_ID, 0, 0, I21145
  },
  {
    0x1282, 0x9100, PCI_ANY_ID, PCI_ANY_ID, 0, 0, DC21140
  },
  {
    0x1282, 0x9102, PCI_ANY_ID, PCI_ANY_ID, 0, 0, DC21140
  },
  {
    0x1113, 0x1217, PCI_ANY_ID, PCI_ANY_ID, 0, 0, MX98715
  },
  {
    0, 0, 0, 0, 0, 0, 0
  }
};

/* Fixme: More crap. Remove? */
/*
  MODULE_DEVICE_TABLE(pci, tulip_pci_tbl);
*/

/* A full-duplex map for media types. */
const char tulip_media_cap[] =
{0,0,0,16,  3,19,16,24,  27,4,7,5, 0,20,23,20 };
u8 t21040_csr13[] = {2,0x0C,8,4,  4,0,0,0, 0,0,0,0, 4,0,0,0};

/* 21041 transceiver register settings: 10-T, 10-2, AUI, 10-T, 10T-FD*/
u16 t21041_csr13[] = {
  csr13_mask_10bt,		/* 10-T */
  csr13_mask_auibnc,		/* 10-2 */
  csr13_mask_auibnc,		/* AUI */
  csr13_mask_10bt,		/* 10-T */
  csr13_mask_10bt,		/* 10T-FD */
};

u16 t21041_csr14[] = { 0xFFFF, 0xF7FD, 0xF7FD, 0x7F3F, 0x7F3D, };
u16 t21041_csr15[] = { 0x0008, 0x0006, 0x000E, 0x0008, 0x0008, };

static void tulip_tx_timeout (net_device *device);
static void tulip_init_ring (net_device *device);
static int tulip_start_transmit (sk_buff *skb, net_device *device);
static int tulip_open (net_device *device);
static int tulip_close (net_device *device);
static void tulip_up (net_device *device);
static void tulip_down (net_device *device);
static net_device_stats *tulip_get_stats (net_device *device);
static int private_ioctl (net_device *device, ifreq *rq, int cmd);
static void set_rx_mode (net_device *device);



static void tulip_up (net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int next_tick = 3*HZ;
  int i;

  DPRINTK ("ENTER\n");

  /* Wake the chip from sleep/snooze mode. */
  if (tp->flags & HAS_ACPI)
  {
    pci_write_config_dword (tp->pci_device, 0x40, 0);
  }

  /* On some chip revs we must set the MII/SYM port before the reset!? */
  if (tp->mii_cnt || (tp->mtable && tp->mtable->has_mii))
  {
    tulip_outl_csr (tp, 0x00040000, CSR6);
  }

  /* Reset the chip, holding bit 0 set at least 50 PCI cycles. */
  system_port_out_u32 (ioaddr + CSR0, 0x00000001);
  udelay (100);

  /* Deassert reset.
     Wait the specified 50 PCI cycles after a reset by initializing
     Tx and Rx queues and the address filter list. */
  system_port_out_u32 (ioaddr + CSR0, tp->csr0);
  udelay (100);

  if (tulip_debug > 1)
  {    
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: tulip_up(), irq==%d.\n", device->name, device->irq);
  }

  if (tp->flags & MULTICAST_HASH_ONLY)
  {
    u32 addr_low = cpu_to_le32
      (system_get_unaligned_u32 ((u32 *) device->address));
    u32 addr_high = cpu_to_le32
      (system_get_unaligned_u16 ((u16 *) (device->address+4)));

    if (tp->chip_id == AX88140)
    {
      system_port_out_u32 (ioaddr + CSR13, 0);
      system_port_out_u32 (ioaddr + CSR14, addr_low);
      system_port_out_u32 (ioaddr + CSR13, 1);
      system_port_out_u32 (ioaddr + CSR14, addr_high);
    }
    else if (tp->chip_id == COMET)
    {
      system_port_out_u32 (ioaddr + 0xA4, addr_low);
      system_port_out_u32 (ioaddr + 0xA8, addr_high);
      system_port_out_u32 (ioaddr + 0xAC, 0);
      system_port_out_u32 (ioaddr + 0xB0, 0);
    }
  }
  else {
    /* This is set_rx_mode(), but without starting the transmitter. */
    u16 *eaddrs = (u16 *)device->address;
    u16 *setup_frm = &tp->setup_frame[15*6];
    dma_addr_t mapping;

    /* 21140 bug: you must add the broadcast address. */

    //    memset(tp->setup_frame, 0xff, sizeof(tp->setup_frame));
    memory_set_u8 ((void *) tp->setup_frame, 0xff, sizeof(tp->setup_frame));

    /* Fill the final entry of the table with our physical address. */

    *setup_frm++ = eaddrs[0];
    *setup_frm++ = eaddrs[0];
    *setup_frm++ = eaddrs[1];
    *setup_frm++ = eaddrs[1];
    *setup_frm++ = eaddrs[2];
    *setup_frm++ = eaddrs[2];

    mapping = pci_map_single(tp->pci_device, tp->setup_frame,
                             sizeof(tp->setup_frame),
                             PCI_DMA_TO_DEVICE);
    tp->tx_buffers[0].skb = NULL;
    tp->tx_buffers[0].mapping = mapping;

    /* Put the setup frame on the Tx list. */
    tp->tx_ring[0].length = cpu_to_le32(0x08000000 | 192);
    tp->tx_ring[0].buffer1 = cpu_to_le32(mapping);
    tp->tx_ring[0].status = cpu_to_le32(DescOwned);

    tp->cur_tx++;
  }

  system_port_out_u32 (ioaddr + CSR3, tp->rx_ring_dma);
  system_port_out_u32 (ioaddr + CSR4, tp->tx_ring_dma);

  tp->saved_if_port = device->if_port;
  if (device->if_port == 0)
  {
    device->if_port = tp->default_port;
  }

  /* Allow selecting a default media. */

  i = 0;
  if (tp->mtable == NULL)
  {
    goto media_picked;
  }
  if (device->if_port)
  {
    int looking_for =
      tulip_media_cap[device->if_port] & MediaIsMII ?
      (11) : (device->if_port == 12 ? 0 : device->if_port);
    for (i = 0; i < tp->mtable->leaf_count; i++)
    {
      if (tp->mtable->mleaf[i].media == looking_for)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: Using user-specified media %s.\n",
           device->name, medianame[device->if_port]);
        goto media_picked;
      }
    }
  }
  if ((tp->mtable->default_media & 0x0800) == 0)
  {
    int looking_for = tp->mtable->default_media & 15;
    for (i = 0; i < tp->mtable->leaf_count; i++)
    {
      if (tp->mtable->mleaf[i].media == looking_for)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: Using EEPROM-set media %s.\n",
           device->name, medianame[looking_for]);
        goto media_picked;
      }
    }
  }
  /* Start sensing first non-full-duplex media. */
  for (i = tp->mtable->leaf_count - 1;
       (tulip_media_cap[tp->mtable->mleaf[i].media] & MediaAlwaysFD) && i > 0;
       i--)
  {

  }

 media_picked:

  tp->csr6 = 0;
  tp->cur_index = i;
  tp->nwayset = 0;
  if (device->if_port == 0 && tp->chip_id == DC21041)
  {
    tp->nway = 1;
  }

  if (device->if_port == 0 && tp->chip_id == DC21142)
  {
    if (tp->mii_cnt)
    {
      tulip_select_media(device, 1);
      if (tulip_debug > 1)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: Using MII transceiver %d, status %4.4x.\n",
           device->name, tp->phys[0],
           tulip_mdio_read(device, tp->phys[0], 1));
      }
      tulip_outl_csr(tp, csr6_mask_defstate, CSR6);
      tp->csr6 = csr6_mask_hdcap;
      device->if_port = 11;
      system_port_out_u32 (ioaddr + CSR13, 0x0000);
      system_port_out_u32 (ioaddr + CSR14, 0x0000);
    }
    else
    {
      t21142_start_nway(device);
    }
  }
  else if (tp->chip_id == PNIC2)
  {
    t21142_start_nway(device);
  }
  else if (tp->chip_id == LC82C168 && !tp->medialock)
  {
    if (tp->mii_cnt)
    {
      device->if_port = 11;
      tp->csr6 = 0x814C0000 | (tp->full_duplex ? 0x0200 : 0);
      system_port_out_u32 (ioaddr + CSR15, 0x0001);
    }
    else if (system_port_in_u32 (ioaddr + CSR5) & TPLinkPass)
    {
      pnic_do_nway(device);
    }
    else
    {
      /* Start with 10mbps to do autonegotiation. */

      system_port_out_u32 (ioaddr + CSR12, 0x32);
      tp->csr6 = 0x00420000;
      system_port_out_u32 (ioaddr + 0xB8, 0x0001B078);
      system_port_out_u32 (ioaddr + 0xB8, 0x0201B078);
      next_tick = 1*HZ;
    }
  }
  else if ((tp->chip_id == MX98713 || tp->chip_id == COMPEX9881) &&
           !tp->medialock)
  {
    device->if_port = 0;
    tp->csr6 = 0x01880000 | (tp->full_duplex ? 0x0200 : 0);
    system_port_out_u32 (ioaddr + 0x80, 0x0f370000 |
                         system_port_in_u16 (ioaddr + 0x80));
  }
  else if (tp->chip_id == MX98715 || tp->chip_id == MX98725)
  {
    /* Provided by BOLO, Macronix - 12/10/1998. */

    device->if_port = 0;
    tp->csr6 = 0x01a80200;
    system_port_out_u32 (ioaddr + 0x80, 0x0f370000 |
                         system_port_in_u16 (ioaddr + 0x80));
    system_port_out_u32 (ioaddr + 0xa0, 0x11000 |
                         system_port_in_u16 (ioaddr + 0xa0));
  }
  else if (tp->chip_id == DC21143 &&
           tulip_media_cap[device->if_port] & MediaIsMII)
  {
    /* We must reset the media CSRs when we force-select MII mode. */
    system_port_out_u32 (ioaddr + CSR13, 0x0000);
    system_port_out_u32 (ioaddr + CSR14, 0x0000);
    system_port_out_u32 (ioaddr + CSR15, 0x0008);
  }
  else if (tp->chip_id == COMET)
  {
    device->if_port = 0;
    tp->csr6 = 0x00040000;
  }
  else if (tp->chip_id == AX88140)
  {
    tp->csr6 = tp->mii_cnt ? 0x00040100 : 0x00000100;
  }
  else
  {
    tulip_select_media(device, 1);
  }

  /* Start the chip's Tx to process setup frame. */

  tulip_outl_csr(tp, tp->csr6, CSR6);
  tulip_outl_csr(tp, tp->csr6 | csr6_st, CSR6);

  /* Enable interrupts by setting the interrupt mask. */

  system_port_out_u32 (ioaddr + CSR5, tulip_tbl[tp->chip_id].valid_interrupts);
  system_port_out_u32 (ioaddr + CSR7, tulip_tbl[tp->chip_id].valid_interrupts);
  tulip_outl_csr(tp, tp->csr6 | csr6_st | csr6_sr, CSR6);

  /* Rx poll demand */

  system_port_out_u32 (ioaddr + CSR2, 0);


  if (tulip_debug > 2)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: Done tulip_open(), CSR0 %8.8x, CSR5 %8.8x CSR6 %8.8x.\n",
       device->name,
       (unsigned int) system_port_in_u32 (ioaddr + CSR0),
       (unsigned int) system_port_in_u32 (ioaddr + CSR5),
       (unsigned int) system_port_in_u32 (ioaddr + CSR6));
  }

  /* Set the timer to switch to check for link beat and perhaps switch
     to an alternate media type. */

  init_timer(&tp->timer);
  tp->timer.expires = RUN_AT(next_tick);
  tp->timer.data = (unsigned long) device;
  tp->timer.function = tulip_tbl[tp->chip_id].media_timer;
  add_timer(&tp->timer);
}


static int tulip_open (net_device *device)
{
  int retval;
  MOD_INC_USE_COUNT;

  retval = request_irq (device->irq, &tulip_interrupt, SA_SHIRQ,
                        device->name, device);

  if (retval)
  {
    MOD_DEC_USE_COUNT;
    return retval;
  }

  tulip_init_ring (device);
	
  tulip_up (device);
	
  netif_start_queue (device);
	
  return 0;
}


static void tulip_tx_timeout (net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
	
  DPRINTK("ENTER\n");

  /* Fixme: Mutex here. */
  //  spin_lock_irqsave (&tp->lock, flags);

  if (tulip_media_cap[device->if_port] & MediaIsMII)
  {
    /* Do nothing -- the media monitor should handle this. */

    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_WARNING,
         "%s: Transmit timeout using MII device.\n",
         device->name);
    }
  }
  else if (tp->chip_id == DC21040)
  {
    if ( !tp->medialock && system_port_in_u32 (ioaddr + CSR12) & 0x0002)
    {
      device->if_port = (device->if_port == 2 ? 0 : 2);
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s: 21040 transmit timed out, switching to %s.\n",
         device->name, medianame[device->if_port]);
      tulip_select_media(device, 0);
    }
    goto out;
  }
  else if (tp->chip_id == DC21041)
  {
    int csr12 = system_port_in_u32 (ioaddr + CSR12);

    log_print_formatted
      (&log_structure, LOG_URGENCY_WARNING,
       "%s: 21041 transmit timed out, status %8.8x, "
       "CSR12 %8.8x, CSR13 %8.8x, CSR14 %8.8x, resetting...\n",
       device->name,
       (unsigned int) system_port_in_u32 (ioaddr + CSR5), csr12,
       (unsigned int) system_port_in_u32 (ioaddr + CSR13),
       (unsigned int) system_port_in_u32 (ioaddr + CSR14));
    tp->mediasense = 1;

    if (!tp->medialock)
    {
      if (device->if_port == 1 || device->if_port == 2)
      {
        if (csr12 & 0x0004)
        {
          device->if_port = 2 - device->if_port;
        }
        else
        {
          device->if_port = 0;
        }
      }
      else
      {
        device->if_port = 1;
      }
      tulip_select_media(device, 0);
    }
  }
  else if (tp->chip_id == DC21140 || tp->chip_id == DC21142 ||
           tp->chip_id == MX98713 || tp->chip_id == COMPEX9881)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_WARNING,
       "%s: 21140 transmit timed out, status %8.8x, "
       "SIA %8.8x %8.8x %8.8x %8.8x, resetting...\n",
       device->name,
       (unsigned int) system_port_in_u32 (ioaddr + CSR5),
       (unsigned int) system_port_in_u32 (ioaddr + CSR12),
       (unsigned int) system_port_in_u32 (ioaddr + CSR13),
       (unsigned int) system_port_in_u32 (ioaddr + CSR14),
       (unsigned int) system_port_in_u32 (ioaddr + CSR15));
    if (!tp->medialock && tp->mtable)
    {
      do
      {
        --tp->cur_index;
      }
      while (tp->cur_index >= 0 &&
             (tulip_media_cap[tp->mtable->mleaf[tp->cur_index].media] &
              MediaIsFD));

      if (--tp->cur_index < 0)
      {
        /* We start again, but should instead look for default. */

        tp->cur_index = tp->mtable->leaf_count - 1;
      }
      tulip_select_media(device, 0);

      log_print_formatted
        (&log_structure, LOG_URGENCY_WARNING,
         "%s: transmit timed out, switching to %s media.\n",
         device->name, medianame[device->if_port]);
    }
  }
  else
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_WARNING,
       "%s: Transmit timed out, status %8.8x, CSR12 %8.8x, resetting...\n",
       device->name,
       (unsigned int) system_port_in_u32 (ioaddr + CSR5),
       (unsigned int) system_port_in_u32 (ioaddr + CSR12));

    device->if_port = 0;
  }

#if defined(way_too_many_messages)

  if (tulip_debug > 3)
  {
    int i;
    for (i = 0; i < RX_RING_SIZE; i++)
    {
      u8 *buf = (u8 *)(tp->rx_ring[i].buffer1);
      int j;
      log_print_formatted
        (&log_structure, LOG_URGENCY_DEBUG,
         "%2d: %8.8x %8.8x %8.8x %8.8x  "
         "%2.2x %2.2x %2.2x.\n",
         i, (unsigned int)tp->rx_ring[i].status,
         (unsigned int)tp->rx_ring[i].length,
         (unsigned int)tp->rx_ring[i].buffer1,
         (unsigned int)tp->rx_ring[i].buffer2,
         buf[0], buf[1], buf[2]);
      for (j = 0; buf[j] != 0xee && j < 1600; j++)
      {
        if (j < 100)
        {
          log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                               "%2.2x", buf[j]);
        }
      }
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "j = %d.\n", j);
    }

    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "  Rx ring %8.8x: ", (int)tp->rx_ring);

    for (i = 0; i < RX_RING_SIZE; i++)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%8.8x", (unsigned int) tp->rx_ring[i].status);
    }

    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "Tx ring %8.8x: ", (int)tp->tx_ring);

    for (i = 0; i < TX_RING_SIZE; i++)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           " %8.8x", (unsigned int)tp->tx_ring[i].status);
    }
    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
  }

#endif /* defined(way_too_many_messages) */

  /* Stop and restart the chip's Tx processes . */

  tulip_restart_rxtx(tp, tp->csr6);

  /* Trigger an immediate transmit demand. */

  system_port_out_u32 (ioaddr + CSR1, 0);

  tp->stats.tx_errors++;

 out:
  device->transmission_start = jiffies;

  /* Fixme: Release mutex here. */
  //  spin_unlock_irqrestore (&tp->lock, flags);
}


/* Initialize the Rx and Tx rings, along with various 'dev' bits. */

static void tulip_init_ring(net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  int i;

  DPRINTK("ENTER\n");

  tp->tx_full = 0;
  tp->cur_rx = tp->cur_tx = 0;
  tp->dirty_rx = tp->dirty_tx = 0;
  tp->susp_rx = 0;
  tp->ttimer = 0;
  tp->nir = 0;

  for (i = 0; i < RX_RING_SIZE; i++)
  {
    tp->rx_ring[i].status = 0x00000000;
    tp->rx_ring[i].length = cpu_to_le32(PKT_BUF_SZ);
    tp->rx_ring[i].buffer2 = cpu_to_le32(tp->rx_ring_dma +
                                         sizeof(tulip_rx_desc) * (i + 1));
    tp->rx_buffers[i].skb = NULL;
    tp->rx_buffers[i].mapping = 0;
  }

  /* Mark the last entry as wrapping the ring. */

  tp->rx_ring[i-1].length = cpu_to_le32(PKT_BUF_SZ | DESC_RING_WRAP);
  tp->rx_ring[i-1].buffer2 = cpu_to_le32(tp->rx_ring_dma);

  for (i = 0; i < RX_RING_SIZE; i++)
  {
    dma_addr_t mapping;

    /* Note the receive buffer must be longword aligned.
       device_alloc_skb() provides 16 byte alignment.  But do *not*
       use skb_reserve() to align the IP header! */
    sk_buff *skb = device_alloc_skb(PKT_BUF_SZ);
    tp->rx_buffers[i].skb = skb;
    if (skb == NULL)
    {
      break;
    }

    mapping = pci_map_single(tp->pci_device, skb->tail,
                             PKT_BUF_SZ, PCI_DMA_FROM_DEVICE);
    tp->rx_buffers[i].mapping = mapping;

    /* Mark as being used by this device. */

    skb->device = device;

    /* Owned by Tulip chip */

    tp->rx_ring[i].status = cpu_to_le32(DescOwned);
    tp->rx_ring[i].buffer1 = cpu_to_le32(mapping);
  }
  tp->dirty_rx = (unsigned int)(i - RX_RING_SIZE);

  /* The Tx buffer descriptor is filled in as needed, but we
     do need to clear the ownership bit. */
  for (i = 0; i < TX_RING_SIZE; i++)
  {
    tp->tx_buffers[i].skb = NULL;
    tp->tx_buffers[i].mapping = 0;
    tp->tx_ring[i].status = 0x00000000;
    tp->tx_ring[i].buffer2 =
      cpu_to_le32(tp->tx_ring_dma + sizeof(tulip_tx_desc) * (i + 1));
  }
  tp->tx_ring[i-1].buffer2 = cpu_to_le32(tp->tx_ring_dma);
}

static int tulip_start_transmit(sk_buff *skb, net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  int entry;
  u32 flag;
  dma_addr_t mapping;

  /* Caution: the write order is important here, set the field
     with the ownership bits last. */

  /* Fixme: Mutex here. */
  //  spin_lock_irq(&tp->lock);

  /* Calculate the next Tx descriptor entry. */

  entry = tp->cur_tx % TX_RING_SIZE;

  tp->tx_buffers[entry].skb = skb;
  mapping = pci_map_single(tp->pci_device, skb->data,
                           skb->length, PCI_DMA_TO_DEVICE);
  tp->tx_buffers[entry].mapping = mapping;
  tp->tx_ring[entry].buffer1 = cpu_to_le32(mapping);

  if (tp->cur_tx - tp->dirty_tx < TX_RING_SIZE/2)
  {
    /* Typical path */

    /* No interrupt */

    flag = 0x60000000;
  }
  else if (tp->cur_tx - tp->dirty_tx == TX_RING_SIZE/2)
  {
    /* Tx-done intr. */

    flag = 0xe0000000;
  }
  else if (tp->cur_tx - tp->dirty_tx < TX_RING_SIZE - 2)
  {
    /* No Tx-done intr. */

    flag = 0x60000000;
  }
  else
  {
    /* Leave room for set_rx_mode() to fill entries. */

    tp->tx_full = 1;

    /* Tx-done intr. */

    flag = 0xe0000000;
    netif_stop_queue(device);
  }

  if (entry == TX_RING_SIZE-1)
  {
    flag = 0xe0000000 | DESC_RING_WRAP;
  }

  tp->tx_ring[entry].length = cpu_to_le32(skb->length | flag);
  tp->tx_ring[entry].status = cpu_to_le32(DescOwned);
  tp->cur_tx++;

  /* Trigger an immediate transmit demand. */

  system_port_out_u32 (device->base_address + CSR1, 0);

  /* Fixme: Release mutex here. */
  //  spin_unlock_irq(&tp->lock);

  device->transmission_start = jiffies;

  return 0;
}

static void tulip_down (net_device *device)
{
  long ioaddr = device->base_address;
  tulip_private *tp = (tulip_private *) device->private;

  del_timer_sync (&tp->timer);

  /* Fixme: Mutex here. */
  //  spin_lock_irqsave (&tp->lock, flags);

  /* Disable interrupts by clearing the interrupt mask. */

  system_port_out_u32 (ioaddr + CSR7, 0x00000000);

  /* Stop the Tx and Rx processes. */

  tulip_stop_rxtx(tp, system_port_in_u32 (ioaddr + CSR6));

  /* 21040 -- Leave the card in 10baseT state. */

  if (tp->chip_id == DC21040)
  {
    system_port_out_u32 (ioaddr + CSR13, 0x00000004);
  }

  if (system_port_in_u32 (ioaddr + CSR6) != 0xffffffff)
  {
    tp->stats.rx_missed_errors += system_port_in_u32 (ioaddr + CSR8) & 0xffff;
  }

  /* Fixme: Release mutex here. */
  //  spin_unlock_irqrestore (&tp->lock, flags);

  device->if_port = tp->saved_if_port;

  /* Leave the driver in snooze, not sleep, mode. */

  if (tp->flags & HAS_ACPI)
  {
    pci_write_config_dword (tp->pci_device, 0x40, 0x40000000);
  }
}
  
  
static int tulip_close (net_device *device)
{
  long ioaddr = device->base_address;
  tulip_private *tp = (tulip_private *) device->private;
  int i;
	
  netif_stop_queue (device);

  tulip_down (device);

  if (tulip_debug > 1)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: Shutting down ethercard, status was %2.2lx.\n",
       device->name,
       system_port_in_u32 (ioaddr + CSR5));
  }

  free_irq (device->irq, device);

  /* Free all the skbuffs in the Rx queue. */

  for (i = 0; i < RX_RING_SIZE; i++)
  {
    sk_buff *skb = tp->rx_buffers[i].skb;
    dma_addr_t mapping = tp->rx_buffers[i].mapping;

    tp->rx_buffers[i].skb = NULL;
    tp->rx_buffers[i].mapping = 0;

    /* Not owned by Tulip chip. */

    tp->rx_ring[i].status = 0;
    tp->rx_ring[i].length = 0;

    /* An invalid address. */

    tp->rx_ring[i].buffer1 = 0xBADF00D0;
    if (skb)
    {
      pci_unmap_single(tp->pci_device, mapping, PKT_BUF_SZ,
                       PCI_DMA_FROM_DEVICE);
      device_kfree_skb (skb);
    }
  }
  for (i = 0; i < TX_RING_SIZE; i++)
  {
    sk_buff *skb = tp->tx_buffers[i].skb;

    if (skb != NULL)
    {
      pci_unmap_single(tp->pci_device, tp->tx_buffers[i].mapping,
                       skb->length, PCI_DMA_TO_DEVICE);
      device_kfree_skb (skb);
    }
    tp->tx_buffers[i].skb = NULL;
    tp->tx_buffers[i].mapping = 0;
  }

  MOD_DEC_USE_COUNT;

  return 0;
}

static net_device_stats *tulip_get_stats(net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;

  if (netif_running(device))
  {
    /* Fixme: Mutex this. */
    //  spin_lock_irqsave (&tp->lock, flags);

    tp->stats.rx_missed_errors += system_port_in_u32 (ioaddr + CSR8) & 0xffff;

    /* Fixme: Release mutex here. */
    //  spin_unlock_irqrestore(&tp->lock, flags);
  }

  return &tp->stats;
}


/* Provide ioctl() calls to examine the MII xcvr state. */
static int private_ioctl(net_device *device, ifreq *rq, int cmd)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  u16 *data = (u16 *)&rq->ifr_data;
  int phy = tp->phys[0] & 0x1f;

  switch (cmd)
  {
    /* Get the address of the PHY in use. */

    case SIOCDEVPRIVATE:
    {
      if (tp->mii_cnt)
      {
        data[0] = phy;
      }
      else if (tp->flags & HAS_NWAY)
      {
        data[0] = 32;
      }
      else if (tp->chip_id == COMET)
      {
        data[0] = 1;
      }
      else
      {
        return -ENODEV;
      }
    }

    /* Read the specified MII register. */

    case SIOCDEVPRIVATE+1:
    {
      if (data[0] == 32 && (tp->flags & HAS_NWAY))
      {
        int csr12 = system_port_in_u32 (ioaddr + CSR12);
        int csr14 = system_port_in_u32 (ioaddr + CSR14);
        switch (data[1])
        {
          case 0:
          {
            data[3] = (csr14<<5) & 0x1000;
            break;
          }
          case 1:
          {
            data[3] = 0x7848 + ((csr12&0x7000) == 0x5000 ? 0x20 : 0) +
              (csr12&0x06 ? 0x04 : 0);
            break;
          }
          case 4:
          {
            data[3] = ((csr14>>9)&0x07C0) +
              ((system_port_in_u32 (ioaddr + CSR6) >> 3) & 0x0040) +
              ((csr14 >> 1) & 0x20) + 1;
            break;
          }
          case 5:
          {
            data[3] = csr12 >> 16;
            break;
          }
          default:
          {
            data[3] = 0; 
            break;
          }
        }
      }
      else
      {
        /* Fixme: Mutex here. */
        //  spin_lock_irqsave (&tp->lock, flags);
        data[3] = tulip_mdio_read(device, data[0] & 0x1f, data[1] & 0x1f);
        /* Fixme: Release mutex here. */
        //  spin_unlock_irqrestore (&tp->lock, flags);
      }
      return 0;
    }

    /* Write the specified MII register */

    case SIOCDEVPRIVATE+2:
    {
      if (!capable(CAP_NET_ADMIN))
      {
        return -EPERM;
      }
      if (data[0] == 32 && (tp->flags & HAS_NWAY))
      {
        if (data[1] == 5)
        {
          tp->to_advertise = data[2];
        }
      }
      else
      {
        /* Fixme: Mutex here. */
        //  spin_lock_irqsave (&tp->lock, flags);
        tulip_mdio_write(device, data[0] & 0x1f, data[1] & 0x1f, data[2]);
        /* Fixme: Release mutex here. */
        //  spin_unlock_irqrestore(&tp->lock, flags);
      }
      return 0;
    }
    default:
    {
      return -EOPNOTSUPP;
    }
  }
  return -EOPNOTSUPP;
}


/* Set or clear the multicast filter for this adaptor.  Note that we
   only use exclusion around actually queueing the new frame, not
   around filling tp->setup_frame.  This is non-deterministic when
   re-entered but still correct. */

/* The little-endian AUTODIN32 ethernet CRC calculation.
   N.B. Do not use for bulk data, use a table-based routine instead.
   This is common code and should be moved to net/core/crc.c */

static unsigned const ethernet_polynomial_le = 0xedb88320U;

static inline u32 ether_crc_le(int length, unsigned char *data)
{
  /* Initial value. */

  u32 crc = 0xffffffff;
  while(--length >= 0)
  {
    unsigned char current_octet = *data++;
    int bit;
    for (bit = 8; --bit >= 0; current_octet >>= 1) {
      if ((crc ^ current_octet) & 1)
      {
        crc >>= 1;
        crc ^= ethernet_polynomial_le;
      }
      else
      {
        crc >>= 1;
      }
    }
  }
  return crc;
}


static unsigned const ethernet_polynomial = 0x04c11db7U;

static inline u32 ether_crc(int length, unsigned char *data)
{
  int crc = -1;

  while(--length >= 0)
  {
    unsigned char current_octet = *data++;
    int bit;
    for (bit = 0; bit < 8; bit++, current_octet >>= 1)
    {
      crc = (crc << 1) ^
        ((crc < 0) ^ (current_octet & 1) ? ethernet_polynomial : 0);
    }
  }
  return crc;
}

static void set_rx_mode(net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int csr6, need_lock = 0;

  DPRINTK("ENTER\n");

  /* Fixme: Mutex here. */
  //  spin_lock_irqsave(&tp->lock, flags);
  csr6 = system_port_in_u32 (ioaddr + CSR6) & ~0x00D5;
  /* Fixme: Release mutex here. */
  //  spin_unlock_irqrestore(&tp->lock, flags);

  tp->csr6 &= ~0x00D5;
  if (device->flags & IFF_PROMISC)
  {
    /* Set promiscuous. */

    tp->csr6 |= 0x00C0;
    csr6 |= 0x00C0;

    /* Unconditionally log net taps. */

    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE,
       "%s: Promiscuous mode enabled.\n", device->name);
		
    need_lock = 1;
  }
  else if ((device->mc_count > 1000) || (device->flags & IFF_ALLMULTI))
  {
    /* Too many to filter well -- accept all multicasts. */

    tp->csr6 |= 0x0080;
    csr6 |= 0x0080;
		
    need_lock = 1;
  }
  else if (tp->flags & MULTICAST_HASH_ONLY)
  {
    /* Some work-alikes have only a 64-entry hash filter table. */
    /* Should verify correctness on big-endian/__powerpc__ */

    device_mc_list *mclist;
    int i;

    /* Multicast hash filter */

    u32 mc_filter[2];

    /* Arbitrary non-effective limit. */
 
    if (device->mc_count > 64)
    {
      tp->csr6 |= 0x0080;
      csr6 |= 0x0080;
      need_lock = 1;
    }
    else
    {
      mc_filter[1] = mc_filter[0] = 0;
      for (i = 0, mclist = device->mc_list;
           mclist && i < device->mc_count;
           i++, mclist = mclist->next)
      {
        set_bit(ether_crc(ETH_ALEN, mclist->dmi_addr)>>26, mc_filter);
      }

      if (tp->chip_id == AX88140)
      {
        /* Fixme: Mutex here. */
        //  spin_lock_irqsave(&tp->lock, flags);
        system_port_out_u32 (ioaddr + CSR13, 2);
        system_port_out_u32 (ioaddr + CSR14, mc_filter[0]);
        system_port_out_u32 (ioaddr + CSR13, 3);
        system_port_out_u32 (ioaddr + CSR14, mc_filter[1]);

        /* need_lock = 0; */
      }
      else if (tp->chip_id == COMET)
      {
        /* Has a simple hash filter. */

        /* Fixme: Mutex here. */
        // spin_lock_irqsave(&tp->lock, flags);
        system_port_out_u32 (ioaddr + 0xAC, mc_filter[0]);
        system_port_out_u32 (ioaddr + 0xB0, mc_filter[1]);

        /* need_lock = 0; */
      }
      else
      {
        need_lock = 1;
      }
    }
  }
  else
  {
    u16 *eaddrs, *setup_frm = tp->setup_frame;
    device_mc_list *mclist;
    u32 tx_flags = 0x08000000 | 192;
    int i;

    /* Note that only the low-address shortword of setup_frame is
       valid! The values are doubled for big-endian architectures. */

    if (device->mc_count > 14)
    {
      /* Must use a multicast hash table. */

      u16 hash_table[32];

      /* Use hash filter. */

      tx_flags = 0x08400000 | 192;
      memory_set_u8 ((void *) hash_table, 0, sizeof(hash_table));

      /* Broadcast entry */

      set_bit(255, hash_table);

      /* This should work on big-endian machines as well. */

      for (i = 0, mclist = device->mc_list;
           mclist && i < device->mc_count;
           i++, mclist = mclist->next)
      {
        set_bit(ether_crc_le(ETH_ALEN, mclist->dmi_addr) & 0x1ff,
                hash_table);
      }
      for (i = 0; i < 32; i++)
      {
        *setup_frm++ = *setup_frm++ = hash_table[i];
      }
      setup_frm = &tp->setup_frame[13*6];
    }
    else
    {

      /* We have <= 14 addresses so we can use the wonderful 16
         address perfect filtering of the Tulip. */

      for (i = 0, mclist = device->mc_list;
           i < device->mc_count;
           i++, mclist = mclist->next)
      {
        eaddrs = (u16 *)mclist->dmi_addr;
        *setup_frm++ = *setup_frm++ = *eaddrs++;
        *setup_frm++ = *setup_frm++ = *eaddrs++;
        *setup_frm++ = *setup_frm++ = *eaddrs++;
      }

      /* Fill the unused entries with the broadcast address. */

      memory_set_u8 ((void *) setup_frm, 0xff, (15-i)*12);
      setup_frm = &tp->setup_frame[15*6];
    }

    /* Fill the final entry with our physical address. */

    eaddrs = (u16 *)device->address;
    *setup_frm++ = *setup_frm++ = eaddrs[0];
    *setup_frm++ = *setup_frm++ = eaddrs[1];
    *setup_frm++ = *setup_frm++ = eaddrs[2];

    /* Fixme: Mutex here. */
    // spin_lock_irqsave(&tp->lock, flags);

    if (tp->cur_tx - tp->dirty_tx > TX_RING_SIZE - 2)
    {
      /* Same setup recently queued, we need not add it. */
    }
    else
    {
      unsigned int entry;
      int dummy = -1;

      /* Now add this frame to the Tx list. */

      entry = tp->cur_tx++ % TX_RING_SIZE;

      if (entry != 0)
      {
        /* Avoid a chip errata by prefixing a dummy entry. */

        tp->tx_buffers[entry].skb = NULL;
        tp->tx_buffers[entry].mapping = 0;
        tp->tx_ring[entry].length =
          (entry == TX_RING_SIZE-1) ? cpu_to_le32(DESC_RING_WRAP) : 0;
        tp->tx_ring[entry].buffer1 = 0;

        /* Must set DescOwned later to avoid race with chip */

        dummy = entry;
        entry = tp->cur_tx++ % TX_RING_SIZE;
      }

      tp->tx_buffers[entry].skb = NULL;
      tp->tx_buffers[entry].mapping =
        pci_map_single(tp->pci_device, tp->setup_frame,
                       sizeof(tp->setup_frame),
                       PCI_DMA_TO_DEVICE);
      /* Put the setup frame on the Tx list. */

      if (entry == TX_RING_SIZE-1)
      {
        /* Wrap ring. */

        tx_flags |= DESC_RING_WRAP;
      }

      tp->tx_ring[entry].length = cpu_to_le32(tx_flags);
      tp->tx_ring[entry].buffer1 =
        cpu_to_le32(tp->tx_buffers[entry].mapping);
      tp->tx_ring[entry].status = cpu_to_le32(DescOwned);

      if (dummy >= 0)
      {
        tp->tx_ring[dummy].status = cpu_to_le32(DescOwned);
      }

      if (tp->cur_tx - tp->dirty_tx >= TX_RING_SIZE - 2)
      {
        netif_stop_queue(device);
        tp->tx_full = 1;
      }

      /* Trigger an immediate transmit demand. */

      system_port_out_u32 (ioaddr + CSR1, 0);
    }
  }
	
  if (need_lock)
  {
    /* Fixme: Mutex here. */
    //  spin_lock_irqsave(&tp->lock, flags);
  }

  /* Can someone explain to me what the OR here is supposed to
     accomplish???? */

  tulip_outl_csr(tp, csr6 | 0x0000, CSR6);

  /* Fixme: Release mutex here. */
  //  spin_unlock_irqrestore(&tp->lock, flags);
}


static int tulip_init_one (pci_dev *pci_device,
                           const pci_device_id *ent)
{
  /* Already printed version info. */

  static int did_version = 0;
  tulip_private *tp;

  /* See note below on the multiport cards. */

  static unsigned char last_phys_addr[6] = {0x00, 'C', 'h', 'a', 'o', 's'};
  static int last_irq = 0;
  static int multiport_cnt = 0;

  /* For four-port boards w/one EEPROM */

  u8 chip_rev;
  unsigned int i, irq;
  unsigned short sum;
  u8 ee_data[EEPROM_SIZE];
  net_device *device;
  long ioaddr;
  static int board_idx = -1;
  int chip_idx = ent->driver_data;
	
  board_idx++;

  if (tulip_debug > 0 && did_version++ == 0)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE, "%s", version);
  }

  if( pci_device->subsystem_vendor == 0x1376 )
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_ERROR, "Skipping LMC card.\n");
    return -ENODEV;
  }
	
  ioaddr = pci_resource_start (pci_device, 0);
  irq = pci_device->irq;

  /* init_etherdev ensures aligned and zeroed private structures */

  device = init_etherdev (NULL, sizeof (*tp));
  if (!device)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_ERROR,
       "Ether device alloc failed, aborting\n");

    return -ENOMEM;
  }

  /* grab all resources from both PIO and MMIO regions, as we don't
     want anyone else messing around with our hardware */

  if (!request_region (pci_resource_start (pci_device, 0),
                       pci_resource_len (pci_device, 0),
                       device->name))
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_ERROR,
       "I/O ports (0x%x@0x%lx) unavailable, aborting\n",
       tulip_tbl[chip_idx].io_size, ioaddr);

    goto err_out_free_netdev;
  }

  if (!request_mem_region (pci_resource_start (pci_device, 1),
                           pci_resource_len (pci_device, 1),
                           device->name)) {
    log_print_formatted
      (&log_structure, LOG_URGENCY_ERROR,
       "MMIO resource (0x%x@0x%lx) unavailable, aborting\n",
       tulip_tbl[chip_idx].io_size, ioaddr);

    goto err_out_free_pio_res;
  }

  if (pci_enable_device(pci_device))
    goto err_out_free_mmio_res;

  pci_set_master(pci_device);

  pci_read_config_byte (pci_device, PCI_REVISION_ID, &chip_rev);

  /* initialize priviate data structure 'tp' it is zeroed and aligned
     in init_etherdev */

  tp = device->private;

  tp->rx_ring =
    pci_alloc_consistent(pci_device, sizeof(tulip_rx_desc) * RX_RING_SIZE +
                         sizeof(tulip_tx_desc) * TX_RING_SIZE,
                         &tp->rx_ring_dma);
  tp->tx_ring = (tulip_tx_desc *)(tp->rx_ring + RX_RING_SIZE);
  tp->tx_ring_dma = tp->rx_ring_dma + sizeof(tulip_rx_desc) * RX_RING_SIZE;

  tp->chip_id = chip_idx;
  tp->flags = tulip_tbl[chip_idx].flags;
  tp->pci_device = pci_device;
  tp->base_address = ioaddr;
  tp->revision = chip_rev;
  tp->csr0 = csr0;
  /* Fixme: Mutex here. */
  //  spin_lock_init(&tp->lock);

  device->base_address = ioaddr;
  device->irq = irq;
  pci_device->driver_data = device;

#ifdef TULIP_FULL_DUPLEX
  tp->full_duplex = 1;
  tp->full_duplex_lock = 1;
#endif
#ifdef TULIP_DEFAULT_MEDIA
  tp->default_port = TULIP_DEFAULT_MEDIA;
#endif
#ifdef TULIP_NO_MEDIA_SWITCH
  tp->medialock = 1;
#endif

  log_print_formatted
    (&log_structure, LOG_URGENCY_INFORMATIVE,
     "%s: %s rev %d at %#3lx,",
     device->name, tulip_tbl[chip_idx].chip_name, chip_rev, ioaddr);

  /* bugfix: the ASIX must have a burst limit or horrible things happen. */

  if (chip_idx == AX88140)
  {
    if ((tp->csr0 & 0x3f00) == 0)
    {
      tp->csr0 |= 0x2000;
    }
  }
  else if (chip_idx == DC21143 && chip_rev == 65)
  {
    tp->csr0 &= ~0x01000000;
  }

  /* Stop the chip's Tx and Rx processes. */

  tulip_stop_rxtx(tp, system_port_in_u32 (ioaddr + CSR6));

  /* Clear the missed-packet counter. */

  (volatile int) system_port_in_u32 (ioaddr + CSR8);

  if (chip_idx == DC21041)
  {
    if (system_port_in_u32 (ioaddr + CSR9) & 0x8000)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           " 21040 compatible mode,");
      chip_idx = DC21040;
    }
    else
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           " 21041 mode,");
    }
  }

  /* The station address ROM is read byte serially.  The register must
     be polled, waiting for the value to be read bit serially from the
     EEPROM. */

  sum = 0;
  if (chip_idx == DC21040)
  {
    /* Reset the pointer with a dummy write. */

    system_port_out_u32 (ioaddr + CSR9, 0);
    for (i = 0; i < 6; i++)
    {
      int value, boguscnt = 100000;
      do
      {
        value = system_port_in_u32 (ioaddr + CSR9);
      }
      while (value < 0 && --boguscnt > 0);

      device->address[i] = value;
      sum += value & 0xff;
    }
  }
  else if (chip_idx == LC82C168)
  {
    for (i = 0; i < 3; i++)
    {
      int value, boguscnt = 100000;
      system_port_out_u32 (ioaddr + 0x98, 0x600 | i);

      do
      {
        value = system_port_in_u32 (ioaddr + CSR9);
      }
      while (value < 0 && --boguscnt > 0);

      system_put_unaligned_u16 (((u16 *) device->address) + 1,
                                system_little_endian_to_native_u16(value));
      sum += value & 0xffff;
    }
  }
  else if (chip_idx == COMET)
  {
    /* No need to read the EEPROM. */

    system_put_unaligned_u32 ((u32 *) device->address,
                              system_port_in_u32 (ioaddr + 0xA4));
    system_put_unaligned_u32 ((u16 *) (device->address + 4),
                              system_port_in_u32 (ioaddr + 0xA8));

    for (i = 0; i < 6; i ++)
    {
      sum += device->address[i];
    }
  }
  else
  {
    /* A serial EEPROM interface, we read now and sort it out later. */

    int sa_offset = 0;
    int ee_addr_size = tulip_read_eeprom(ioaddr, 0xff, 8) & 0x40000 ? 8 : 6;

    for (i = 0; i < sizeof(ee_data)/2; i++)
    {
      ((u16 *)ee_data)[i] =
        system_little_endian_to_native_u16 (tulip_read_eeprom(ioaddr, i, ee_addr_size));

      /* DEC now has a specification (see Notes) but early board makers
         just put the address in the first EEPROM locations. */

      /* This does memcmp(eedata, eedata+16, 8) */

      for (i = 0; i < 8; i ++)
      {
        if (ee_data[i] != ee_data[16+i])
        {
          sa_offset = 20;
        }
      }
      if (ee_data[0] == 0xff && ee_data[1] == 0xff &&  ee_data[2] == 0)
      {
        /* Grrr, damn Matrox boards. */

        sa_offset = 2;
        multiport_cnt = 4;
      }
      for (i = 0; i < 6; i ++)
      {
        device->address[i] = ee_data[i + sa_offset];
        sum += ee_data[i + sa_offset];
      }
    }

    /* Lite-On boards have the address byte-swapped. */

    if ((device->address[0] == 0xA0 || device->address[0] == 0xC0) &&
        device->address[1] == 0x00)
    {
      for (i = 0; i < 6; i+=2)
      {
        char tmp = device->address[i];
        device->address[i] = device->address[i+1];
        device->address[i+1] = tmp;
      }
    }

    /* On the Zynx 315 Etherarray and other multiport boards only the
       first Tulip has an EEPROM.

       The addresses of the subsequent ports are derived from the first.
       Many PCI BIOSes also incorrectly report the IRQ line, so we
       correct that here as well. */

    if (sum == 0 || sum == 6*0xff)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "EEPROM not present,");
      for (i = 0; i < 5; i++)
      {
        device->address[i] = last_phys_addr[i];
      }
      device->address[i] = last_phys_addr[i] + 1;

#if defined(__i386__)

      /* Patch up x86 BIOS bug. */

      if (last_irq)
        irq = last_irq;
#endif

    }

    for (i = 0; i < 6; i++)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%c%2.2X", (i ? ':' : ' '),
                           last_phys_addr[i] = device->address[i]);
    }
    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                         ", IRQ %d.\n", irq);
    last_irq = irq;

    /* The lower four bits are the media type. */
    if (board_idx >= 0 && board_idx < MAX_UNITS)
    {
      tp->default_port = options[board_idx] & 15;
      if ((options[board_idx] & 0x90) || full_duplex[board_idx] > 0)
      {
        tp->full_duplex = 1;
      }
      if (mtu[board_idx] > 0)
      {
        device->mtu = mtu[board_idx];
      }
    }
    if (device->mem_start)
    {
      tp->default_port = device->mem_start;
    }
    if (tp->default_port)
    {
      tp->medialock = 1;
      if (tulip_media_cap[tp->default_port] & MediaAlwaysFD)
      {
        tp->full_duplex = 1;
      }
    }
    if (tp->full_duplex)
    {
      tp->full_duplex_lock = 1;
    }

    if (tulip_media_cap[tp->default_port] & MediaIsMII)
    {
      u16 media2advert[] = { 0x20, 0x40, 0x03e0, 0x60, 0x80, 0x100, 0x200 };
      tp->to_advertise = media2advert[tp->default_port - 9];
    }
    else if (tp->flags & HAS_8023X)
    {
      tp->to_advertise = 0x05e1;
    }
    else
    {
      tp->to_advertise = 0x01e1;
    }

    /* This is logically part of _init_one(), but too complex to write
       inline. */

    if (tp->flags & HAS_MEDIA_TABLE)
    {
      memory_copy (tp->eeprom, ee_data, sizeof(tp->eeprom));
      tulip_parse_eeprom(device);
    }

    if ((tp->flags & ALWAYS_CHECK_MII) ||
        (tp->mtable && tp->mtable->has_mii) ||
        (!tp->mtable && (tp->flags & HAS_MII)))
    {
      unsigned int phy, phy_idx;
      if (tp->mtable && tp->mtable->has_mii)
      {
        for (i = 0; i < tp->mtable->leaf_count; i++)
        {
          if (tp->mtable->mleaf[i].media == 11)
          {
            tp->cur_index = i;
            tp->saved_if_port = device->if_port;
            tulip_select_media(device, 1);
            device->if_port = tp->saved_if_port;
            break;
          }
        }
      }

      /* Find the connected MII xcvrs.
         Doing this in open() would allow detecting external xcvrs later,
         but takes much time. */

      for (phy = 0, phy_idx = 0;
           phy < 32 && phy_idx < sizeof(tp->phys);
           phy++)
      {
        int mii_status = tulip_mdio_read(device, phy, 1);
        if ((mii_status & 0x8301) == 0x8001 ||
            ((mii_status & 0x8000) == 0 && (mii_status & 0x7800) != 0))
        {
          int mii_reg0 = tulip_mdio_read(device, phy, 0);
          int mii_advert = tulip_mdio_read(device, phy, 4);
          int reg4 = ((mii_status>>6) & tp->to_advertise) | 1;
          tp->phys[phy_idx] = phy;
          tp->advertising[phy_idx++] = reg4;
          log_print_formatted
            (&log_structure, LOG_URGENCY_INFORMATIVE,
             "%s:  MII transceiver #%d config %4.4x "
             "status %4.4x advertising %4.4x.\n",
             device->name, phy, mii_reg0, mii_status, mii_advert);

          /* Fixup for DLink with miswired PHY. */

          if (mii_advert != reg4)
          {
            log_print_formatted
              (&log_structure, LOG_URGENCY_DEBUG,
               "%s:  Advertising %4.4x on PHY %d,"
               " previously advertising %4.4x.\n",
               device->name, reg4, phy, mii_advert);

            log_print_formatted
              (&log_structure, LOG_URGENCY_DEBUG,
               "%s:  Advertising %4.4x (to advertise is %4.4x).\n",
               device->name, reg4, tp->to_advertise);

            tulip_mdio_write(device, phy, 4, reg4);
          }

          /* Enable autonegotiation: some boards default to off. */

          tulip_mdio_write(device, phy, 0, mii_reg0 |
                           (tp->full_duplex ? 0x1100 : 0x1000) |
                           (tulip_media_cap[tp->default_port]&MediaIs100 ? 0x2000:0));
        }
      }
      tp->mii_cnt = phy_idx;
      if (tp->mtable && tp->mtable->has_mii && phy_idx == 0)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: ***WARNING***: No MII transceiver found!\n",
           device->name);
        tp->phys[0] = 1;
      }
    }

    /* The Tulip-specific entries in the device structure. */

    /* Fixme: Casts needed during mid-porting stage only, remove when
       struct have been completed. */

    /*
      int (* open) (void *device);
      int (* hard_start_transmit) (void *skb, void *device);
      void (* tx_timeout) (void *device);
      int (* stop) (void *device);
      int (* get_stats) (void *device);
    */

    device->open = (int *) tulip_open;
    device->hard_start_transmit = tulip_start_transmit;
    device->tx_timeout = tulip_tx_timeout;
    device->watchdog_timeo = TX_TIMEOUT;
    device->stop = tulip_close;
    device->get_stats = tulip_get_stats;
    device->do_ioctl = private_ioctl;
    device->set_multicast_list = set_rx_mode;

    if ((tp->flags & HAS_NWAY) || tp->chip_id == DC21041)
    {
      tp->link_change = t21142_lnk_change;
    }
    else if (tp->flags & HAS_PNICNWAY)
    {
      tp->link_change = pnic_lnk_change;
    }

    /* Reset the xcvr interface and turn on heartbeat. */

    switch (chip_idx)
    {
      case DC21041:
      {
        tp->to_advertise = 0x0061;
        system_port_out_u32 (ioaddr + CSR13, 0x00000000);
        system_port_out_u32 (ioaddr + CSR14, 0xFFFFFFFF);

        /* Listen on AUI also. */

        system_port_out_u32 (ioaddr + CSR15, 0x00000008);
        tulip_outl_csr(tp, system_port_in_u32 (ioaddr + CSR6) | csr6_fd, CSR6);
        system_port_out_u32 (ioaddr + CSR13, 0x0000EF05);
        break;
      }
      case DC21040:
      {
        system_port_out_u32 (ioaddr + CSR13, 0x00000000);
        system_port_out_u32 (ioaddr + CSR13, 0x00000004);
        break;
      }
      case DC21140:
      default:
      {
        if (tp->mtable)
        {
          system_port_out_u32 (ioaddr + CSR12, tp->mtable->csr12dir | 0x100);
        }
        break;
      }
      case DC21142:
      case PNIC2:
      {
        if (tp->mii_cnt || tulip_media_cap[device->if_port] & MediaIsMII)
        {
          tulip_outl_csr(tp, csr6_mask_defstate, CSR6);
          system_port_out_u32 (ioaddr + CSR13, 0x0000);
          system_port_out_u32 (ioaddr + CSR14, 0x0000);
          tulip_outl_csr(tp, csr6_mask_hdcap, CSR6);
        }
        else
        {
          t21142_start_nway(device);
        }
        break;
      }
      case LC82C168:
      {
        if (!tp->mii_cnt)
        {
          tp->nway = 1;
          tp->nwayset = 0;
          tulip_outl_csr(tp, csr6_ttm | csr6_ca, CSR6);
          system_port_out_u32 (ioaddr + CSR12, 0x30);
          tulip_outl_csr(tp, 0x0001F078, CSR6);

          /* Turn on autonegotiation. */

          tulip_outl_csr(tp, 0x0201F078, CSR6);
        }
        break;
      }
      case MX98713:
      case COMPEX9881:
      {
        tulip_outl_csr(tp, 0x00000000, CSR6);

        /* Turn on NWay. */

        system_port_out_u32 (ioaddr + CSR14, 0x000711C0);
        system_port_out_u32 (ioaddr + CSR13, 0x00000001);
        break;
      }
      case MX98715:
      case MX98725:
      {
        tulip_outl_csr(tp, 0x01a80000, CSR6);
        system_port_out_u32 (ioaddr + CSR14, 0xFFFFFFFF);
        system_port_out_u32 (ioaddr + CSR12, 0x00001000);
        break;
      }
      case COMET:
      {
        /* No initialization necessary. */

        break;
      }
    }

    /* put the chip in snooze mode until opened */

    if (tulip_tbl[chip_idx].flags & HAS_ACPI)
    {
      pci_write_config_dword(pci_device, 0x40, 0x40000000);
    }

    return 0;

  err_out_free_mmio_res:
    release_mem_region (pci_resource_start (pci_device, 1),
                        pci_resource_len (pci_device, 1));
  err_out_free_pio_res:
    release_region (pci_resource_start (pci_device, 0),
                    pci_resource_len (pci_device, 0));
  err_out_free_netdev:
    unregister_netdev (device);
    kfree (device);
    return -ENODEV;
  }


  static void tulip_suspend (pci_dev *pci_device)
    {
      net_device *device = pci_device->driver_data;

      if (device && netif_device_present (device))
      {
        netif_device_detach (device);
        tulip_down (device);
      }
      //	pci_set_power_state(pci_device, 3);
    }


  static void tulip_resume(pci_dev *pci_device)
    {
      net_device *device = pci_device->driver_data;

      pci_enable_device(pci_device);
      if (device && !netif_device_present (device))
      {
        tulip_up (device);
        netif_device_attach (device);
      }
    }


  static void tulip_remove_one (pci_dev *pci_device)
    {
      net_device *device = pci_device->driver_data;

      if (device)
      {
        tulip_private *tp = (tulip_private *)device->private;
        pci_free_consistent(pci_device,
                            sizeof(tulip_rx_desc) * RX_RING_SIZE +
                            sizeof(tulip_tx_desc) * TX_RING_SIZE,
                            tp->rx_ring,
                            tp->rx_ring_dma);
        unregister_netdev(device);
        release_mem_region (pci_resource_start (pci_device, 1),
                            pci_resource_len (pci_device, 1));
        release_region (pci_resource_start (pci_device, 0),
                        pci_resource_len (pci_device, 0));
        kfree(device);
      }
    }


  static pci_driver tulip_driver =
    {
      name:		TULIP_MODULE_NAME,
      id_table:	tulip_pci_tbl,
      probe:		tulip_init_one,
      remove:		tulip_remove_one,
      suspend:	tulip_suspend,
      resume:		tulip_resume,
    };


  static int tulip_init (void)
    {
      /* copy module parms into globals */

      tulip_rx_copybreak = rx_copybreak;
      tulip_max_interrupt_work = max_interrupt_work;
	
      /* probe for and init boards */

      return pci_module_init (&tulip_driver);
    }


  static void tulip_cleanup (void)
    {
      pci_unregister_driver (&tulip_driver);
    }

  /* Fixme: More crap. Remove? */
  /*
    module_init(tulip_init);
    module_exit(tulip_cleanup);
  */
