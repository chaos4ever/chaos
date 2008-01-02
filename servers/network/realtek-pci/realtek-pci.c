/* $Id$ */
/* Abstract: Realtek PCI server. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

/* Partially based on the GPL:ed Linux driver, written by Donald Becker. */

#include "config.h"
#include "realtek-pci.h"

#define REALTEK_VENDOR_ID               0x10EC

pci_device_probe_type pci_device_probe[] =
{
  /* Realtek 8139. */

  { REALTEK_VENDOR_ID, 0x8139 },

  /* End of list. */

  { 0xFFFF, 0xFFFF }
};

static log_structure_type log_structure;
static ipc_structure_type pci_structure;

/* An empty tag list. */

static tag_type empty_tag =
{
  0, 0, ""
};

/* User-tunable variables. */

static int realtek_debug = 1;
static int max_interrupt_work = 20;

/* Delay between EEPROM clock transitions.  No extra delay is needed
   with 33Mhz PCI, but 66Mhz may change this. */

#define eeprom_delay() system_port_in_u32 (eeprom_address)

#define mdio_delay()	system_port_in_u8 (mdio_address)

static char mii_2_8139_map[8] = 
{
  MII_BMCR, MII_BMSR, 0, 0, NWayAdvert, NWayLPAR, NWayExpansion, 0 
};

static void set_rx_mode (realtek_device_type *device)
{
  long io_address = device->port_base;
  int rx_mode;
  
  rx_mode = AcceptBroadcast | AcceptMyPhysical;

  /* We can safely update without stopping the chip. */

  system_port_out_u8 (io_address + RxConfig, rx_mode);
}

/* Syncronize the MII management interface by shifting 32 one bits out. */

static void mdio_sync (u16 mdio_address)
{
  int counter;
  
  for (counter = 32; counter >= 0; counter--)
  {
    system_port_out_u8 (mdio_address, MDIO_WRITE1);
    mdio_delay ();
    system_port_out_u8 (mdio_address, MDIO_WRITE1 | MDIO_CLOCK);
    mdio_delay();
  }
}

/* Read from the MII device. */

static int mdio_read (u16 base_address, int physical_id, int location)
{
  long mdio_address = base_address + MII_SMI;
  int mii_command = (0xF6 << 10) | (physical_id << 5) | location;
  int return_value = 0;
  int counter;
  
  /* Really a 8139.  Use internal registers. */

  if (physical_id > 31)
  {
    return (location < 8 && mii_2_8139_map[location] ?
            system_port_in_u16 (base_address + mii_2_8139_map[location]) : 0);
  }

  mdio_sync (mdio_address);

  /* Shift the read command bits out. */
  
  for (counter = 15; counter >= 0; counter--) 
  {
    int data_value = (mii_command & (1 << counter)) ? MDIO_DATA_OUT : 0;
    
    system_port_out_u8 (mdio_address, MDIO_DIR | data_value);
    mdio_delay ();
    system_port_out_u8 (mdio_address, MDIO_DIR | data_value | MDIO_CLOCK);
    mdio_delay ();
  }
  
  /* Read the two transition, 16 data, and wire-idle bits. */

  for (counter = 19; counter > 0; counter--) 
  {
    system_port_out_u8 (mdio_address, 0);
    mdio_delay ();
    return_value = (return_value << 1) | ((system_port_in_u8 (mdio_address) & 
                                           MDIO_DATA_IN) ? 1 : 0);
    system_port_out_u8 (mdio_address, MDIO_CLOCK);
    mdio_delay ();
  }

  return (return_value >> 1) & 0xFFFF;
}

/* Read an EEPROM register. */

static int read_eeprom (u16 io_address, int location)
{
  int counter;
  unsigned return_value = 0;
  long eeprom_address = io_address + Config9346;
  int read_command = location | EEPROM_READ_COMMAND;
  
  system_port_out_u8 (eeprom_address, EEPROM_ENB & ~EEPROM_CHIP_SELECT);
  system_port_out_u8 (eeprom_address, EEPROM_ENB);
  
  /* Shift the read command bits out. */

  for (counter = 10; counter >= 0; counter--)
  {
    int dataval = (read_command & (1 << counter)) ? EEPROM_DATA_WRITE : 0;

    system_port_out_u8 (eeprom_address, EEPROM_ENB | dataval);
    eeprom_delay ();
    system_port_out_u8 (eeprom_address, EEPROM_ENB | dataval | 
                        EEPROM_SHIFT_CLOCK);
    eeprom_delay ();
  }

  system_port_out_u8 (eeprom_address, EEPROM_ENB);
  eeprom_delay ();
  
  for (counter = 16; counter > 0; counter--) 
  {
    system_port_out_u8 (eeprom_address, EEPROM_ENB | EEPROM_SHIFT_CLOCK);
    eeprom_delay ();
    return_value = (return_value << 1) | 
      ((system_port_in_u8 (eeprom_address) & EEPROM_DATA_READ) ? 1 : 0);
    system_port_out_u8 (eeprom_address, EEPROM_ENB);
    eeprom_delay ();
  }
  
  /* Terminate the EEPROM access. */

  system_port_out_u8 (eeprom_address, ~EEPROM_CHIP_SELECT);
  return return_value;
}

/* Receive a packet from the Realtek device. */

static int realtek_receive (realtek_device_type *device)
{
  long io_address = device->port_base;
  unsigned char *rx_ring = device->rx_ring;
  u16 current_rx = device->current_rx;
  unsigned int counter;
  
  if (realtek_debug > 4)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                         "In %s (), current %4.4x BufAddr %4.4x,"
                         " free to %4.4x, Command %2.2x.",
                         __FUNCTION__, current_rx, 
                         system_port_in_u16 (io_address + RxBufferAddress),
                         system_port_in_u16 (io_address + RxBufferPointer),
                         system_port_in_u8 (io_address + ChipCommand));
  }
  
  while ((system_port_in_u8 (io_address + ChipCommand) & 1) == 0) 
  {
    int ring_offset = current_rx % RX_BUFFER_LENGTH;
    u32 rx_status = system_little_endian_to_native_u32 
      (* (u32 *) (rx_ring + ring_offset));
    int rx_size = rx_status >> 16;
    
    if (realtek_debug > 4) 
    {
      // int i;
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                           "%s () status %lx, size %x, current %x.",
                           __FUNCTION__, rx_status, rx_size, current_rx);

      //      printk(KERN_DEBUG"%s: Frame contents ", device->name);
      //      for (i = 0; i < 70; i++)
      //      {
      //        printk(" %2.2x", le32_to_cpu(rx_ring[ring_offset + i]));
      //      printk(".\n");
    }

    if ((rx_status & RxTooLong) != 0)
    {
      if (realtek_debug > 0)
      {
        log_print_formatted (&log_structure, LOG_URGENCY_WARNING,
                             "Oversized Ethernet frame, status %lx!",
                             rx_status);
      }

      // tp->stats.rx_length_errors++;
    }
    else if ((rx_status & (RxBadSymbol | RxRunt | RxTooLong | RxCRCError | 
                           RxBadAlign)) != 0)
    {
      if (realtek_debug > 1)
      {
        log_print_formatted (&log_structure, LOG_URGENCY_WARNING, 
                             "Ethernet frame had errors,"
                             " status %lx.", rx_status);
      }

      // tp->stats.rx_errors++;

      if ((rx_status & (RxBadSymbol | RxBadAlign)) != 0)
      {
        // tp->stats.rx_frame_errors++;
      }

      if ((rx_status & (RxRunt | RxTooLong)) != 0)
      {
        // tp->stats.rx_length_errors++;
      }

      if (rx_status & RxCRCError) 
      {
        // tp->stats.rx_crc_errors++;
      }

      /* Reset the receiver, based on RealTek recommendation. (Bug?) */

      device->current_rx = 0;
      system_port_out_u8 (io_address + ChipCommand, CommandTxEnable);
      system_port_out_u8 (io_address + ChipCommand, CommandRxEnable |
                          CommandTxEnable);
      system_port_out_u32 (io_address + RxConfig, (RX_FIFO_THRESHOLD << 13) |
                           (RX_BUFFER_LENGTH_INDEX << 11) | (RX_DMA_BURST << 8));
    }
    else 
    {
      u8 *data;
      u8 **data_pointer = &data;
      
      memory_allocate ((void **) data_pointer, rx_size);

      if (ring_offset + rx_size + 4 > RX_BUFFER_LENGTH)
      {
        int semi_count = RX_BUFFER_LENGTH - ring_offset - 4;
       
        memory_copy (data, &rx_ring[ring_offset + 4],
                     semi_count);
        memory_copy (&data[semi_count], rx_ring,
                     rx_size - semi_count);

        if (realtek_debug > 4) 
        {
          log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                               "Frame wrap @%d.", semi_count);
          memory_set_u8 (rx_ring, 0xCC, 16);
        }
      } 
      else 
      {
        memory_copy (data, &rx_ring[ring_offset + 4], rx_size);
      }

      /* Check if this packet should be delivered somewhere. */
      
      for (counter = 0; counter < device->number_of_targets; counter++)
      {          
        if (device->target[counter].protocol_type ==
            ((ipv4_ethernet_header_type *) data)->protocol_type)
        {
          message_parameter_type message_parameter;
          
          message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
          message_parameter.message_class = IPC_ETHERNET_PACKET_RECEIVED;
          message_parameter.length = rx_size;
          message_parameter.block = FALSE;
          message_parameter.data = data;
          
          if (realtek_debug > 2)
          {
            log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                                 "Sending to mailbox ID %u", 
                                 device->target[counter].mailbox_id);
          }
          
          ipc_send (device->target[counter].mailbox_id, &message_parameter);
          break;
        }
      }

      // memory_deallocate ((void **) &data);

#if LINUX_VERSION_CODE > 0x20119
      // tp->stats.rx_bytes += rx_size;
#endif
      // tp->stats.rx_packets++;
    }
    
    current_rx = (current_rx + rx_size + 4 + 3) & ~3;
    system_port_out_u16 (io_address + RxBufferPointer, current_rx - 16);
  }

  if (realtek_debug > 4)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                         "Done %s (), current %4.4x BufferAddress %4.4x,"
                         " free to %4.4x, Command %2.2x.\n",
                         __FUNCTION__, current_rx,
                         system_port_in_u16 (io_address + RxBufferAddress),
                         system_port_in_u16 (io_address + RxBufferPointer), 
                         system_port_in_u8 (io_address + ChipCommand));
  }

  device->current_rx = current_rx;

  return 0;
}

/* Start a transmit. */

static bool realtek_start_transmit (u8 *data, unsigned int length,
                                   realtek_device_type *device)
{
  long io_address = device->port_base;
  int entry;
  
  // netif_stop_queue(dev);
  
  /* Calculate the next Tx descriptor entry. */

  entry = device->current_tx % NUMBER_OF_TX_DESCRIPTORS;
  
  // tp->tx_info[entry].skb = skb;

  // tp->tx_info[entry].mapping = 0;
  
  memory_copy (device->tx_buffer[entry], data, length);
  system_port_out_u32 (io_address + TxAddress0 + entry * 4, 
                       (u32) (device->tx_buffers_dma + 
                              (device->tx_buffer[entry] -
                               device->tx_buffers)));
  
  /* Note: the chip doesn't have auto-pad! */
  
  system_port_out_u32 (io_address + TxStatus0 + entry * 4,
                       device->tx_flag | 
                       (length >= IPV4_ETHERNET_MINIMUM_LENGTH ? 
                        length : IPV4_ETHERNET_MINIMUM_LENGTH));
  
  /* Typical path */

  if (++device->current_tx - device->dirty_tx < NUMBER_OF_TX_DESCRIPTORS) 
  {
    // netif_start_queue(dev);
  }
  else 
  {
    device->tx_full = TRUE;
  }
  
  // dev->trans_start = jiffies;

  if (realtek_debug > 4)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Queued Tx packet at %p size %d to slot %d.\n",
                         data, length, entry);
  }
  
  return TRUE;
}

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id, 
                               realtek_device_type *device)
{
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  bool done = FALSE;
  unsigned int data_size = 1024;
  u32 *data;
  u32 **data_pointer = &data;

  memory_allocate ((void **) data_pointer, data_size);

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
        if (!realtek_start_transmit ((u8 *) data,
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

/* Handle a given Realtek 8139 adapter. */

static void handle_8139 (pci_device_info_type *device_info)
{
  unsigned int counter;
  u16 port_base = MAX_U16;
  u16 ports = 0;
  realtek_device_type *device;
  realtek_device_type **device_pointer = &device;
  unsigned int physical, physical_index;
  ipc_structure_type ipc_structure;

  system_thread_name_set ("Initialising");
  
  for (counter = 0; counter < PCI_NUMBER_OF_RESOURCES; counter++)
  {
    if ((device_info->resource[counter].flags & PCI_RESOURCE_IO) != 0)
    {
      port_base = device_info->resource[counter].start;
      ports = (device_info->resource[counter].end -
               device_info->resource[counter].start + 1);
    }
  }

  if (port_base == MAX_U16)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_ERROR, 
                         "No port range found -- hardware possibly broken or incompatible?");
    return;
  }

  system_call_port_range_register (port_base, ports, "Realtek 8139");

  memory_allocate ((void **) device_pointer, sizeof (realtek_device_type));

  device->port_base = port_base;
  device->irq = device_info->irq;

  /* Initialise the adapter. */

  system_port_out_u8 (port_base + Config1, 0x00);
  
  if (read_eeprom (port_base, 0) != 0xFFFF) 
  {
    for (counter = 0; counter < 3; counter++) 
    {
      ((u16 *)(device->ethernet_address))[counter] =
        system_little_endian_to_native_u16 (read_eeprom (port_base, 
                                                        counter + 7));
    }
  }
  else 
  {
    for (counter = 0; counter < 6; counter++)
    {
      device->ethernet_address[counter] = 
        system_port_in_u8 (port_base + MAC0 + counter);
    }
  }

  log_print_formatted 
    (&log_structure, LOG_URGENCY_INFORMATIVE,
     "Realtek 8139 at 0x%X, IRQ %d, ethernet address: %02X:%02X:%02X:%02X:%02X:%02X",
     port_base, device_info->irq, device->ethernet_address[0], 
     device->ethernet_address[1], device->ethernet_address[2], 
     device->ethernet_address[3], device->ethernet_address[4],
     device->ethernet_address[5]);

  /* Find the connected MII transceivers. */

  for (physical = 0, physical_index = 0; physical < 32 &&
         physical_index < sizeof (device->mii_address); physical++)
  {
    int mii_status = mdio_read (port_base, physical, 1);

    if (mii_status != 0xFFFF && mii_status != 0x0000) 
    {
      device->mii_address[physical_index] = physical;
      physical_index++;

      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "MII transceiver found at address %d.",
                           physical);
    }
  }

  if (physical_index == 0) 
  {
    if (realtek_debug > 1)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "No MII transceivers found! Assuming SYM "
                           "transceiver.");
    }

    device->mii_address[0] = -1;
  }

  /* Soft reset the chip. */

  system_port_out_u8 (port_base + ChipCommand, CommandReset);

  u8 **tx_buffers_dma_pointer = &device->tx_buffers_dma;
  u8 **tx_buffers_pointer = &device->tx_buffers;
  u8 **rx_ring_dma_pointer = &device->rx_ring_dma;
  u8 **rx_ring_pointer = &device->rx_ring;

  pci_allocate_buffer ((void **) tx_buffers_dma_pointer,
                       (void **) tx_buffers_pointer,
                       TX_BUFFER_SIZE * NUMBER_OF_TX_DESCRIPTORS);
  pci_allocate_buffer ((void **) rx_ring_dma_pointer,
                       (void **) rx_ring_pointer,
                       RX_BUFFER_LENGTH + 16);

  device->tx_full = FALSE;
  device->current_rx = 0;
  device->dirty_tx = device->current_tx = 0;

  for (counter = 0; counter < NUMBER_OF_TX_DESCRIPTORS; counter++) 
  {
    device->tx_buffer[counter] = &device->tx_buffers[counter * TX_BUFFER_SIZE];
  }
  
  /* Check that the chip has finished the reset. */

  for (counter = 0; counter < 1000; counter++)
  {
    if ((system_port_in_u8 (port_base + ChipCommand) & CommandReset) == 0)
    {
      break;
    }
  }

  /* Must enable Tx/Rx before setting transfer thresholds! */

  system_port_out_u8 (port_base + ChipCommand, 
                      CommandRxEnable | CommandTxEnable);
  system_port_out_u32 (port_base + RxConfig, 
                       (RX_FIFO_THRESHOLD << 13) | 
                       (RX_BUFFER_LENGTH_INDEX << 11) | 
                       (RX_DMA_BURST << 8));

  system_port_out_u32 (port_base + TxConfig, (TX_DMA_BURST << 8) | 0x03000000);

  if (device->mii_address[0] >= 0) 
  {
    u16 mii_reg5 = mdio_read (port_base, device->mii_address[0], 5);

    if (mii_reg5 == 0xffff)
    {
    }
    else if ((mii_reg5 & 0x0100) == 0x0100 || (mii_reg5 & 0x00C0) == 0x0040)
    {
      device->full_duplex = TRUE;
    }

    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                         "Setting %s%s-duplex based on"
                         " auto-negotiated partner ability %4.4x.\n",
                         mii_reg5 == 0 ? "" :
                         ((mii_reg5 & 0x0180) != 0) ? "100 Mbps " : 
                         "10 Mbps ",
                         device->full_duplex ? "full" : "half", mii_reg5);
  }
  
  system_port_out_u8 (port_base + Config9346, 0xC0);
  system_port_out_u8 (port_base + Config1, device->full_duplex ? 0x60 : 0x20);
  system_port_out_u8 (port_base + Config9346, 0x00);

  system_port_out_u32 (port_base + RxBuffer, (u32) device->rx_ring_dma);
  
  /* Start the chip's Tx and Rx process. */
  
  system_port_out_u32 (port_base + RxMissed, 0);
  set_rx_mode (device);
  
  system_port_out_u8 (port_base + ChipCommand,
                      CommandRxEnable | CommandTxEnable);
  
  /* Enable all known interrupts by setting the interrupt mask. */
  
  system_port_out_u16 (port_base + InterruptMask,
                       PCIError | PCSTimeout | RxUnderrun | RxOverflow | 
                       RxFIFOOverrun | TxError | TxOK | RxError | RxOK);


  if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    system_thread_name_set ("IRQ handler");

    if (system_call_irq_register (device_info->irq, "Realtek 8139") != 
        SYSTEM_RETURN_SUCCESS)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, 
                           "Couldn't set up IRQ handler");
      return;
    }

    /* Loop and handle interrupts. */
    
    while (TRUE)
    {
      int bogus_count = max_interrupt_work;
      unsigned status, link_changed = 0;
      
      system_call_irq_wait (device->irq);
      
      do 
      {
        status = system_port_in_u16 (port_base + InterruptStatus);
        
        /* Acknowledge all of the current interrupt sources ASAP, but
           an first get an additional status bit from CSCR. */
        
        if ((status & RxUnderrun) && 
            system_port_in_u16 (port_base + CSCR) & CSCR_LinkChangeBit)
        {
          link_changed = 1;
        }
        
        system_port_out_u16 (port_base + InterruptStatus, status);
        
        if ((status & (PCIError | PCSTimeout | RxUnderrun | RxOverflow | 
                       RxFIFOOverrun | TxError | TxOK | RxError | RxOK)) == 0)
        {
          break;
        }
        
        /* Rx interrupt. */
        
        if ((status & (RxOK | RxUnderrun | RxOverflow | RxFIFOOverrun)) != 0)
        {
          realtek_receive (device);
        }
        
        if ((status & (TxOK | TxError)) != 0) 
        {
          unsigned int dirty_tx = device->dirty_tx;
          
          while (device->current_tx - dirty_tx > 0) 
          {
            int entry = dirty_tx % NUMBER_OF_TX_DESCRIPTORS;
            int txstatus = system_port_in_u32 (port_base + TxStatus0 +
                                               entry * 4);
            
            if ((txstatus & (TxStatusOK | TxUnderrun | TxAborted)) == 0)
            {
              /* It still hasn't been transmitted. */
              
              break;
            }
            
            /* Note: TxCarrierLost is always asserted at 100 Mbps. */
            
            if ((txstatus & (TxOutOfWindow | TxAborted)) != 0)
            {
              /* There was an major error, log it. */
              
              // device->stats.tx_errors++;

              if ((txstatus & TxAborted) != 0)
              {
                // tp->stats.tx_aborted_errors++;

                system_port_out_u32 (port_base + TxConfig, 
                                     (TX_DMA_BURST << 8) | 0x03000001);
              }
              
              if ((txstatus & TxCarrierLost) != 0)
              {
                // tp->stats.tx_carrier_errors++;
              }
              
              if ((txstatus & TxOutOfWindow) != 0)
              {
                // tp->stats.tx_window_errors++;
              }
            }
            else 
            {
              if ((txstatus & TxUnderrun) != 0)
              {
                /* Add 64 to the Tx FIFO threshold. */
                
                if (device->tx_flag < 0x00300000)
                {
                  device->tx_flag += 0x00020000;
                }
                
                // tp->stats.tx_fifo_errors++;
              }
              
              // tp->stats.collisions += (txstatus >> 24) & 15;
              // tp->stats.tx_bytes += txstatus & 0x7ff;
              // tp->stats.tx_packets++;
            }
            
            // if (tp->tx_info[entry].mapping != 0)
            // {
            //   pci_unmap_single (tp->pdev,
            //                     tp->tx_info[entry].mapping,
            //                     tp->tx_info[entry].skb->len,
            //                     PCI_DMA_TODEVICE);
            // tp->tx_info[entry].mapping = 0;
            // }
          
            /* Free the original skb. */
            
            // dev_kfree_skb_irq (tp->tx_info[entry].skb);
            //          tp->tx_info[entry].skb = NULL;
            
            if (device->tx_full) 
            {
              /* The ring is no longer full, wake the queue. */
              
              device->tx_full = FALSE;
              
              // netif_wake_queue(dev);
            }
            
            dirty_tx++;
          }
          
          device->dirty_tx = dirty_tx;
        }
        
        /* Check uncommon events with one test. */
        
        if ((status & (PCIError | PCSTimeout | RxUnderrun | RxOverflow | 
                       RxFIFOOverrun | TxError | RxError)) != 0)
        {
          if (realtek_debug > 2)
          {
            log_print_formatted (&log_structure, LOG_URGENCY_WARNING,
                                 "Abnormal interrupt, status %8.8x.\n",
                                 status);
          }
          
          if (status == 0xFFFFFFFF)
          {
            break;
          }
          
          /* Update the error count. */
          
          // tp->stats.rx_missed_errors += inl(port_base + RxMissed);
          system_port_out_u32 (port_base + RxMissed, 0);
          
          if ((status & RxUnderrun) != 0 && link_changed)
          {
            /* Really link-change on new chips. */
            
            int lpar = system_port_in_u16 (port_base + NWayLPAR);
            int duplex = (lpar & 0x0100) || (lpar & 0x01C0) == 0x0040; 
            
            if (device->full_duplex != duplex) 
            {
              device->full_duplex = duplex;
              
              system_port_out_u8 (port_base + Config9346, 0xC0);
              system_port_out_u8 (port_base + Config1, 
                                  device->full_duplex ? 0x60 : 0x20);
              system_port_out_u8 (port_base + Config9346, 0x00);
            }
            status &= ~RxUnderrun;
          }
          
          if ((status & (RxUnderrun | RxOverflow | RxError |
                         RxFIFOOverrun)) != 0)
          {
            // tp->stats.rx_errors++;
          }
          
          if ((status & (PCSTimeout)) != 0)
          {
            // tp->stats.rx_length_errors++;
          }
          
          if ((status & (RxUnderrun | RxFIFOOverrun)) != 0)
          {
            //  tp->stats.rx_fifo_errors++;
          }
          
          if ((status & RxOverflow) != 0)
          {
            // tp->stats.rx_over_errors++;
            device->current_rx =
              (system_port_in_u16 (port_base + RxBufferAddress) % 
               RX_BUFFER_LENGTH);
            system_port_out_u16 (port_base + RxBufferPointer, device->current_rx - 16);
          }
          
          if ((status & PCIError) != 0) 
          {
            log_print (&log_structure, LOG_URGENCY_WARNING, "PCI Bus error.");
          }
        }
        
        if (--bogus_count < 0) 
        {
          log_print_formatted (&log_structure, LOG_URGENCY_WARNING,
                               "Too much work at interrupt, "
                               "InterruptStatus = 0x%4.4x.",
                               status);
          
          /* Clear all interrupt sources. */
          
          system_port_out_u16 (port_base + InterruptStatus, 0xFFFF);
          break;
        }
      } while (TRUE);
      
      system_call_irq_acknowledge (device->irq);
    }
  }

  /* Now, use the remaining thread for the service handler. */

  system_thread_name_set ("Service handler");

  /* Create the service. */

  if (ipc_service_create ("ethernet", &ipc_structure, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create ethernet service.");
    return;
  }

  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;

    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;

    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_call_thread_name_set ("Handling connection");
      handle_connection (reply_mailbox_id, device);
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
