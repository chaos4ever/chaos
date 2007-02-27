/* $Id$ */
/* Abstract: Via-rhine driver. */
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

/* This driver is a port of the Linux driver, written by Donald Becker. */


#include "config.h"
#include "types.h"
#include "enumerations.h"

bool debug = FALSE;


/* Fixme: Fakes. Remove these. */
/* --------------------------- */
void netif_carrier_on (net_device *);
void netif_carrier_off (net_device *);
void add_timer(timer_type *timer);
void printk (char *str, ...);
/* --------------------------- */


static int mdio_read (net_device *device, int phy_id, int location);
static void mdio_write (net_device *device, int phy_id, int location,
                        int value);
static int via_rhine_open (net_device *device);
static void via_rhine_check_duplex (net_device *device);

static void via_rhine_timer (unsigned long data);
static void via_rhine_send_timeout (net_device *device);
static void via_rhine_init_ring (net_device *device);
static int via_rhine_start_send (sk_buff *skb, net_device *device);
static void via_rhine_interrupt (int irq, void *device_instance,
                                 pt_regs *regs);
static void via_rhine_send (net_device *device);
static void via_rhine_receive (net_device *device);
static void via_rhine_error (net_device *device, int intr_status);
static void via_rhine_set_receive_mode (net_device *device);
static net_device_stats *via_rhine_get_stats (net_device *device);
static int mii_ioctl (net_device *device, ifreq *rq, int cmd);
static int via_rhine_close (net_device *device);
static inline void clear_tally_counters (long ioaddr);

static log_structure_type log_structure;
static ipc_structure_type pci_structure;


/* Read and write over the MII Management Data I/O (MDIO) interface. */

static int mdio_read(net_device *device, int phy_id, int regnum)
{
  long io_address = device->base_address;
  int bogus_counter = 1024;

  /* Wait for a previous command to complete. */
  while (bogus_counter > 0)
  {
    if (system_port_in_u8 (io_address + MIICmd) & 0x60)
    {
      break;
    }
    bogus_counter--;
  }

  system_port_out_u8 (io_address + MIICmd, 0x00);
  system_port_out_u8 (io_address + MIIPhyAddr, phy_id);
  system_port_out_u8 (io_address + MIIRegAddr, regnum);

  /* Trigger read */

  system_port_out_u8 (io_address + MIICmd, 0x40);
  bogus_counter = 1024;

  while (bogus_counter > 0)
  {
    if (system_port_in_u8 (io_address + MIICmd) & 0x40)
    {
      break;
    }
    bogus_counter--;
  }

  return system_port_in_u16(io_address + MIIData);
}

static void mdio_write(net_device *device, int phy_id, int regnum, int value)
{
  net_device_private_type *np = (net_device_private_type *) device->private;
  long io_address = device->base_address;
  int bogus_counter = 1024;

  if (phy_id == np->phys[0])
  {
    switch (regnum)
    {
      /* Is user forcing speed/duplex? */

      case 0:
      {
        /* Autonegotiation. */

        if (value & 0x9000)
        {
          np->duplex_lock = 0;
        }
        else
        {
          np->full_duplex = (value & 0x0100) ? 1 : 0;
        }
        break;
      }
      case 4:
      {
        np->advertising = value;
        break;
      }
    }
  }

  /* Wait for a previous command to complete. */
  while ((system_port_in_u8 (io_address + MIICmd) & 0x60) && --bogus_counter > 0)
  {
  }

  system_port_out_u8 (io_address + MIICmd, 0x00);
  system_port_out_u8 (io_address + MIIPhyAddr, phy_id);
  system_port_out_u8 (io_address + MIIRegAddr, regnum);
  system_port_out_u16 (io_address + MIIData, value);

  /* Trigger write. */

  system_port_out_u8 (io_address + MIICmd, 0x20);
}


static void via_rhine_check_duplex(net_device *device)
{
  net_device_private_type *np = (net_device_private_type *)device->private;
  long io_address = device->base_address;
  int mii_reg5 = mdio_read(device, np->phys[0], 5);
  int negotiated = mii_reg5 & np->advertising;
  int duplex;

  if (np->duplex_lock  ||  mii_reg5 == 0xffff)
  {
    return;
  }
  duplex = (negotiated & 0x0100) || (negotiated & 0x01C0) == 0x0040;
  if (np->full_duplex != duplex)
  {
    np->full_duplex = duplex;
    if (debug)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%s: Setting %s-duplex based on MII #%d link"
                           " partner capability of %4.4x.\n", device->name,
                           duplex ? "full" : "half", np->phys[0], mii_reg5);
    }
    if (duplex)
    {
      np->chip_cmd |= CmdFDuplex;
    }
    else
    {
      np->chip_cmd &= ~CmdFDuplex;
    }
  system_port_out_u16 (io_address + ChipCmd, np->chip_cmd);
  }
}


static void via_rhine_timer(unsigned long data)
{
  net_device *device = (net_device *)data;
  net_device_private_type *np = (net_device_private_type *)device->private;
  long io_address = device->base_address;
  int next_tick = 10*HZ;
  int mii_status;

  if (debug > 3)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "%s: VIA Rhine monitor tick, status %4.4x.\n",
                         device->name, system_port_in_u16(io_address + IntrStatus));
  }

  /* Fixme: Mutex here. */
  //  spin_lock_irq (&np->lock);

  via_rhine_check_duplex(device);

  /* make IFF_RUNNING follow the MII status bit "Link established" */
  mii_status = mdio_read(device, np->phys[0], 1);
  if ( (mii_status & MIILink) != (np->mii_status & MIILink) )
  {
    if (mii_status & MIILink)
    {
      netif_carrier_on(device);
    }
    else
    {
      netif_carrier_off(device);
    }
  }
  np->mii_status = mii_status;

  /* Fixme: Mutex here. */
  //  spin_unlock_irq (&np->lock);

  np->timer.expires = jiffies + next_tick;
  add_timer(&np->timer);
}


static void via_rhine_send_timeout (net_device *device)
{
  net_device_private_type *np = (net_device_private_type *) device->private;
  long io_address = device->base_address;

  /* Lock to protect mdio_read and access to stats. A friendly
     advice to the implementor of the XXXs in this function is to be
     sure not to spin too long (whatever that means :) */

  /* Fixme: Mutex here. */
  //  spin_lock_irq (&np->lock);

  printk (KERN_WARNING "%s: Transmit timed out, status %4.4x, PHY status "
          "%4.4x, resetting...\n",
          device->name, system_port_in_u16 (io_address + IntrStatus),
          mdio_read (device, np->phys[0], 1));

  /* XXX Perhaps we should reinitialize the hardware here. */
  device->if_port = 0;

  /* Stop and restart the chip's Send processes . */
  /* XXX to do */

  /* Trigger an immediate transmit demand. */
  /* XXX to do */

  device->trans_start = jiffies;
  np->stats.send_errors++;

  /* Fixme: Mutex here. */
  //  spin_unlock_irq (&np->lock);
}


static int via_rhine_start_send(sk_buff *skb, net_device *device)
{
  net_device_private_type *np = (net_device_private_type *)device->private;
  unsigned entry;

  /* Caution: the write order is important here, set the field
     with the "ownership" bits last. */

  /* lock eth irq */

  /* Fixme: Mutex here. */
  //  spin_lock_irq (&np->lock);

  /* Calculate the next Send descriptor entry. */

  entry = np->cur_send % SEND_RING_SIZE;

  np->send_skbuff[entry] = skb;

  if ((np->drv_flags & ReqSendAlign) && ((long)skb->data & 3))
  {
    /* Must use alignment buffer. */

    memcpy(np->send_buf[entry], skb->data, skb->len);
    np->send_skbuff_dma[entry] = 0;
    np->send_ring[entry].addr = cpu_to_le32(np->send_bufs_dma +
                                          (np->send_buf[entry] - np->send_bufs));
  }
  else
  {
    np->send_skbuff_dma[entry] =
      pci_map_single(np->pci_device, skb->data, skb->len, PCI_DMA_TODEVICE);
    np->send_ring[entry].addr = cpu_to_le32(np->send_skbuff_dma[entry]);
  }

  np->send_ring[entry].desc_length = 
    cpu_to_le32(0x00E08000 | (skb->len >= ETH_ZLEN ? skb->len : ETH_ZLEN));
  np->send_ring[entry].send_status = cpu_to_le32(DescOwn);

  np->cur_send++;

  /* Non-x86 Todo: explicitly flush cache lines here. */

  /* Wake the potentially-idle transmit channel. */
  system_port_out_u16 (device->base_address + ChipCmd,
                       CmdSendDemand | np->chip_cmd);

  if (np->cur_send == np->dirty_send + SEND_QUEUE_LEN)
  {
    netif_stop_queue(device);
  }

  device->trans_start = jiffies;

  /* Fixme: Mutex here. */
  //  spin_unlock_irq (&np->lock);

  if (debug > 4)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "%s: Transmit frame #%d queued in slot %d.\n",
                         device->name, np->cur_send, entry);
  }
  return 0;
}


static void via_rhine_error(net_device *device, int intr_status)
{
  net_device_private_type *np = (net_device_private_type *)device->private;
  long io_address = device->base_address;

  /* Fixme: Mutex here. */
  //  spin_lock (&np->lock);

  if (intr_status & (IntrMIIChange | IntrLinkChange))
  {
    if (system_port_in_u8 (io_address + MIIStatus) & 0x02)
    {
      /* Link failed, restart autonegotiation. */
      if (np->drv_flags & HasDavicomPhy)
      {
        mdio_write(device, np->phys[0], 0, 0x3300);
      }
    }
    else
    {
      via_rhine_check_duplex(device);
    }
    if (debug)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                           "%s: MII status changed: Autonegotiation "
                           "advertising %4.4x  partner %4.4x.\n",
                           device->name, mdio_read(device, np->phys[0], 4),
                           mdio_read(device, np->phys[0], 5));
    }
  }
  if (intr_status & IntrStatsMax)
  {
    np->stats.receive_crc_errors	+= system_port_in_u16(io_address + ReceiveCRCErrs);
    np->stats.receive_missed_errors	+= system_port_in_u16(io_address + ReceiveMissed);
    clear_tally_counters(io_address);
  }
  if (intr_status & IntrSendAbort)
  {
    /* Stats counted in Send-done handler, just restart Send. */
    system_port_out_u16 (device->base_address + ChipCmd,
                         CmdSendDemand | np->chip_cmd);
  }
  if (intr_status & IntrSendUnderrun)
  {
    if (np->send_thresh < 0xE0)
    {
      system_port_out_u8 (io_address + SendConfig, np->send_thresh += 0x20);
    }

    if (debug > 1)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%s: Transmitter underrun, increasing Send "
                           "threshold setting to %2.2x.\n", device->name, np->send_thresh);
    }
  }
  if ((intr_status & ~( IntrLinkChange | IntrStatsMax |
                        IntrSendAbort | IntrSendAborted)) && debug > 1)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                         "%s: Something Wicked happened! %4.4x.\n",
                         device->name, intr_status);

    /* Recovery for other fault sources not known. */

    system_port_out_u16 (device->base_address + ChipCmd,
                         CmdSendDemand | np->chip_cmd);
  }

  /* Fixme: Mutex here. */
  //  spin_unlock (&np->lock);
}

static net_device_stats *via_rhine_get_stats(net_device *device)
{
  net_device_private_type *np = (net_device_private_type *)device->private;
  long io_address = device->base_address;
  unsigned long flags;

  /* Fixme: Mutex here. */
  //  spin_lock_irqsave(&np->lock, flags);

  np->stats.receive_crc_errors	+= system_port_in_u16(io_address + ReceiveCRCErrs);
  np->stats.receive_missed_errors	+= system_port_in_u16(io_address + ReceiveMissed);
  clear_tally_counters(io_address);

  /* Fixme: Mutex here. */
  //  spin_unlock_irqrestore(&np->lock, flags);

  return &np->stats;
}

/* Clears the "tally counters" for CRC errors and missed frames(?).
   It has been reported that some chips need a write of 0 to clear
   these, for others the counters are set to 1 when written to and
   instead cleared when read. So we clear them both ways ... */
static inline void clear_tally_counters(const long io_address)
{
  system_port_out_u32 (io_address + ReceiveMissed, 0);
  system_port_in_u16(io_address + ReceiveCRCErrs);
  system_port_in_u16(io_address + ReceiveMissed);
}


/* The big-endian AUTODIN II ethernet CRC calculation.
   N.B. Do not use for bulk data, use a table-based routine instead.
   This is common code and should be moved to net/core/crc.c */
static inline u32 ether_crc(int length, unsigned char *data)
{
  unsigned const ethernet_polynomial = 0x04c11db7U;

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

static void via_rhine_set_receive_mode(net_device *device)
{
  net_device_private_type *np = (net_device_private_type *)device->private;
  long io_address = device->base_address;
  u32 mc_filter[2];			/* Multicast hash filter */
  u8 receive_mode;					/* Note: 0x02=accept runt, 0x01=accept errs */

  /* Set promiscuous. */

  if (device->flags & IFF_PROMISC)
  {
    /* Unconditionally log net taps. */

    printk(KERN_NOTICE "%s: Promiscuous mode enabled.\n", device->name);
    receive_mode = 0x1C;
  }
  else if ((device->mc_count > multicast_filter_limit) ||
           (device->flags & IFF_ALLMULTI))
  {
    /* Too many to match, or accept all multicasts. */

    system_port_out_u32 (io_address + MulticastFilter0, 0xffffffff);
    system_port_out_u32 (io_address + MulticastFilter1, 0xffffffff);
    receive_mode = 0x0C;
  }
  else
  {
    dev_mc_list *mclist;
    int i;
    memset(mc_filter, 0, sizeof(mc_filter));
    for (i = 0, mclist = device->mc_list; mclist && i < device->mc_count;
         i++, mclist = mclist->next)
    {
      set_bit(ether_crc(ETH_ALEN, mclist->dmi_addr) >> 26, mc_filter);
    }
    system_port_out_u32 (io_address + MulticastFilter0, mc_filter[0]);
    system_port_out_u32 (io_address + MulticastFilter1, mc_filter[1]);
    receive_mode = 0x0C;
  }
  system_port_out_u8 (io_address + ReceiveConfig, np->receive_thresh | receive_mode);
}

static int mii_ioctl(net_device *device, ifreq *rq, int cmd)
{
  net_device_private_type *np = (net_device_private_type *)device->private;
  u16 *data = (u16 *)&rq->ifr_data;
  unsigned long flags;
  int retval;

  /* Fixme: Mutex here. */
  //  spin_lock_irqsave(&np->lock, flags);

  retval = 0;

  switch(cmd)
  {
    case SIOCDEVPRIVATE:
    {
      /* Get the address of the PHY in use. */

      data[0] = np->phys[0] & 0x1f;

      /* Fall Through */
    }
    case SIOCDEVPRIVATE+1:
    {
      /* Read the specified MII register. */

      data[3] = mdio_read(device, data[0] & 0x1f, data[1] & 0x1f);
      break;
    }
    case SIOCDEVPRIVATE+2:
    {
      /* Write the specified MII register */

      if (!capable(CAP_NET_ADMIN))
      {
        retval = -EPERM;
        break;
      }
      mdio_write(device, data[0] & 0x1f, data[1] & 0x1f, data[2]);
      break;
    }
    default:
    {
      retval = -EOPNOTSUPP;
    }
  }

  /* Fixme: Mutex here. */
  //  spin_unlock_irqrestore(&np->lock, flags);

  return retval;
}



static void via_rhine_remove_one (pci_device *pci_device)
{
  net_device *device = pci_device->driver_data;
  net_device_private_type *np = (net_device_private_type *)(device->private);
	
  unregister_net_device(device);

  release_region(pci_resource_start (pci_device, 0),
                 pci_resource_len (pci_device, 0));
  release_mem_region(pci_resource_start (pci_device, 1),
                     pci_resource_len (pci_device, 1));

#ifndef USE_IO
  iounmap((char *)(device->base_address));
#endif

  pci_free_consistent(pci_device, 
                      RECEIVE_RING_SIZE * sizeof(receive_desc) +
                      SEND_RING_SIZE * sizeof(send_desc),
                      np->receive_ring, np->receive_ring_dma);

  kfree(device);
}


#if FALSE

static void handle_connection (mailbox_id_type reply_mailbox_id, 
                               via_rhine_device_type *device)
{
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  bool done = FALSE;
  unsigned int data_size = 1024;
  u32 *data;

  memory_allocate ((void **) &data, data_size);

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  message_parameter.data = data;
  message_parameter.block = TRUE;
  message_parameter.protocol = IPC_PROTOCOL_ETHERNET;

  while (!done)
  {
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     &data_size) !=
        STORM_RETURN_SUCCESS)
    {
      continue;
    }

    switch (message_parameter.message_class)
    {
      case IPC_ETHERNET_REGISTER_TARGET:
      {
        /* FIXME: Check if the protocol is already registered */

        device->target[device->number_of_targets].mailbox_id = 
          ipc_structure.output_mailbox_id;
        device->target[device->number_of_targets].protocol_type =
          system_byte_swap_u16 (data[0]);
        device->number_of_targets++;
        break;
      }

      case IPC_ETHERNET_PACKET_SEND:
      {
        if (!via_rhine_start_transmit ((u8 *) data,
                                       message_parameter.length, device))
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Failed to send an ethernet packet.");

          /* FIXME: Do something. */
        }
        break;
      }

      case IPC_ETHERNET_ADDRESS_GET:
      {
        memory_copy (data, &device->ethernet_address, 6);
        message_parameter.length = 6;
        system_call_mailbox_send (ipc_structure.output_mailbox_id,
                                  &message_parameter);
        break;
      }

      default:
      {
        log_print (&log_structure, LOG_URGENCY_ERROR,
                   "Unknown IPC command received.");
        break;
      }
    }
  }
}


int main (void)
{
  pci_device_info_type *device_info;
  unsigned int number_of_devices;
  unsigned int counter;
  unsigned int probe_counter;
  
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

  system_call_process_parent_unblock ();

  for (probe_counter = 0; pci_device_probe[probe_counter].vendor_id !=
         0xFFFF; probe_counter++)
  {
    pci_device_exists (&pci_structure, &pci_device_probe[probe_counter],
                       &device_info, &number_of_devices);
    
    if (number_of_devices != 0)
    {
      for (counter = 0; counter < number_of_devices; counter++)
      {
        if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
        {
          handle_8139 (&device_info[counter]);
        }
      }
    }
  }

  return 0;
}

#endif
