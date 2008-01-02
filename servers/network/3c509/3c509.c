/* $Id$ */
/* Abstract: 3Com Etherlink III server for chaos. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

/* Partially based on 3c509.c: A 3c509 EtherLink3 ethernet driver for
   Linux, Written 1993-1998 by Donald Becker. (Licensed under the GNU 
   GPL) */

/* TODO:
   
  * Rewrite the Linux-stuff. (outw, etc)
  * Use the 3Com provided specification and learn how this driver and
    card works.

*/

#include "config.h"
#include "3c509.h"

#define outb(a,b) system_port_out_u8 (b, a)
#define outw(a,b) system_port_out_u16 (b, a)
#define outsb(a,b,c) system_port_out_u8_string (a, b, c)
#define outsl(a,b,c) system_port_out_u32_string (a, b, c)
#define inw(a) system_port_in_u16 (a)
#define insl(a,b,c) system_port_in_u32_string (a, b, c)

static log_structure_type log_structure;

/* Start with 0x110 to avoid new sound cards. (?) */

static int id_port = 0x110;
static u16 id_read_eeprom (int index);

/* Maximum events (Rx packets, etc.) to handle at each interrupt. */

static int max_interrupt_work = 25;

/* system_call_process_control \
                    (syscall_process_get_pid_by_name ("3c509"), \
                    PROCESS_SLEEP, a) */

static inline int test_and_set_bit (int nr, volatile void * addr)
{
  int oldbit;
  
  asm volatile ("btsl %2,%1\n"
                "sbbl %0,%0"
                : "=r" (oldbit), "=m" (addr)
                : "Ir" (nr));
  return oldbit;
}

//#define ETHERLINK3_DEBUG 100 /* lots of debugging information */

#ifdef ETHERLINK3_DEBUG
int etherlink3_debug = ETHERLINK3_DEBUG;
#else
int etherlink3_debug = 2;
#endif

const char *interface_names[] = 
{
  "10baseT", "AUI", "undefined", "BNC"
};

/* An empty tag list. */

tag_type empty_tag =
{
  0, 0, ""
};

/* Probe for a free EtherLink III adapter. */

static bool etherlink3_probe (device_type *device)
{
  short lrs_state = 0xFF, i;
  int io_address, irq, interface_port;
  u16 physical_address[3];
  static int current_tag = 0;
  unsigned int io_base;

#ifdef LINUX
  
  /* Reset the ISA PnP mechanism on 3c509b. */
  /* Select PnP config control register. */

  system_port_out_u8 (0x279, 0x02);

  /* Return to WaitForKey state. */

  system_port_out_u8 (0xA79, 0x02);

#endif

  /* Select an open I/O location at 0x1*0 to do contention select. */

  for ( ; id_port < 0x200; id_port += 0x10) 
  {
    if (system_call_port_range_register (id_port, 1,
                                         "3Com Etherlink III ID port") !=
        STORM_RETURN_SUCCESS)
    {
      continue;
    }

    system_port_out_u8 (id_port, 0x00);
    system_port_out_u8 (id_port, 0xFF);

    if (system_port_in_u8 (id_port) & 0x01)
    {
      break;
    }
  }

  if (id_port >= 0x200) 
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "No I/O port available for 3c509 activation.");

    return FALSE;
  }

  /* Next check for all ISA bus boards by sending the ID sequence to
     the ID_PORT. We find cards past the first by setting the
     'current_tag' on cards as they are found. Cards with their tag
     set will not respond to subsequent ID sequences. */
  
  system_port_out_u8 (id_port, 0x00);
  system_port_out_u8 (id_port, 0x00);

  for (i = 0; i < 255; i++) 
  {
    system_port_out_u8 (id_port, lrs_state);
    lrs_state <<= 1;
    lrs_state = lrs_state & 0x100 ? lrs_state ^ 0xcf : lrs_state;
  }
  
  /* For the first probe, clear all board's tag registers. Otherwise
     kill off already-found boards. */
  
  if (current_tag == 0)
  {
    system_port_out_u8 (id_port, 0xD0);
  }
  else                          
  {
    system_port_out_u8 (id_port, 0xD8);
  }
  
  if (id_read_eeprom (7) != 0x6D50) 
  {
    system_call_port_range_unregister (id_port);
    return FALSE;
  }

  /* Read in EEPROM data, which does contention-select. Only the
     lowest address board will stay "on-line". 3Com got the byte order
     backwards. */

  for (i = 0; i < 3; i++) 
  {
    physical_address[i] = system_byte_swap_u16 (id_read_eeprom(i));
  }   

  io_base = id_read_eeprom (8);
  interface_port = io_base >> 14;
  io_address = 0x200 + ((io_base & 0x1F) << 4);
  irq = id_read_eeprom (9) >> 12;

  /* Register the port range so that we can access the card */

  if (system_call_port_range_register (io_address, ETHERLINK3_IO_EXTENT,
                                       "3Com Etherlink III") !=
      STORM_RETURN_SUCCESS)
  {
    system_call_port_range_unregister (id_port);
    return FALSE;
  }

  if (device != NULL)
  {
    /* Set passed-in IRQ or I/O Addr. */
        
    if (device->irq > 1 &&
        device->irq < 16)
    {
      irq = device->irq;
    }

    if (device->base_addr) 
    {
      /* Magic key */

      if (device->mem_end == 0x3c509 &&
          device->base_addr >= 0x200 &&
          device->base_addr <= 0x3E0)
      {
        io_address = device->base_addr & 0x3F0;
      }
      else if (device->base_addr != io_address)
      {
        system_call_port_range_unregister (id_port);
        return FALSE;
      }
    }
  }

  /* Set the adapter tag so that the next card can be found. */

  system_port_out_u8 (id_port, 0xD0 + ++current_tag);

  /* Activate the adapter at the EEPROM location. */

  system_port_out_u8 (id_port, (io_address >> 4) | 0xE0);

  ETHERLINK3WINDOW(0);
  if (system_port_in_u16 (io_address) != 0x6D50)
  {
    system_call_port_range_unregister (id_port);
    return FALSE;
  }

  /* Free the internet so that some other guy can use it. */

  system_port_out_u16 (io_address + WINDOW0_IRQ, 0x0F00);

#ifdef LINUX
  if (dev == NULL) 
  {
    dev = init_etherdev(dev, sizeof(struct etherlink3_private));
  }
#endif
  memory_copy (device->ethernet_address, physical_address,
               sizeof (physical_address));
  device->base_addr = io_address;
  device->irq = irq;
  device->interface_port = (device->mem_start & 0x1F) ? device->mem_start & 3 : interface_port;

  system_call_port_range_unregister (id_port);

  log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                       "Adapter found at 0x%04lX tag %d, %s port, address "
                       "%02X:%02X:%02X:%02X:%02X:%02X, IRQ %u.",
                       device->base_addr, current_tag,
                       interface_names[device->interface_port],
                       device->ethernet_address[0],
                       device->ethernet_address[1],
                       device->ethernet_address[2],
                       device->ethernet_address[3],
                       device->ethernet_address[4],
                       device->ethernet_address[5],
                       device->irq);

  return TRUE;
}

/* Read a word from the EEPROM when in the ISA ID probe state. */

static u16 id_read_eeprom (int index)
{
  int bit, word = 0;

  /* Issue read command, and pause for at least 162 us. for it to
     complete. Assume extra-fast 16 MHz bus. */

  system_port_out_u8 (id_port, EEPROM_READ + index);

  /* Pause for at least 162 us. for the read to take place. */

  system_sleep (1);
        
  for (bit = 15; bit >= 0; bit--)
  {
    word = (word << 1) + (system_port_in_u8 (id_port) & 0x01);
  }

  if (etherlink3_debug > 3)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                         "EEPROM word %d %x.", index, word);
  }

  return word;
}

/* Open the etherlink 3 device. */

static bool etherlink3_open (device_type *device)
{
  int io_address = device->base_addr;
  int i;

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, TxReset);
  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, RxReset);
  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, SetStatusEnb | 0x00);
  
  /* Handle the interrupt. */

  if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    /* Allocate memory for the input buffer. */
    
    u32 **buffer = &device->input_buffer;
    memory_allocate ((void **) buffer, 2048);
    
    if (system_call_irq_register (device->irq, "3Com Etherlink III") != 
        STORM_RETURN_SUCCESS)
    {
      log_print (&log_structure, LOG_URGENCY_EMERGENCY,
                 "Couldn't register IRQ handler.");
      return FALSE;
    }
    etherlink3_interrupt (device);
  }
  
  ETHERLINK3WINDOW(0);
  if (etherlink3_debug > 3)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Opening, IRQ %d status@%x %x.", 
                         device->irq, io_address + ETHERLINK3_STATUS,
                         system_port_in_u16 (io_address + ETHERLINK3_STATUS));
  }

  /* Activate board: this is probably unnecessary. */

  system_port_out_u16 (io_address + 4, 0x0001);

  /* Set the IRQ line. */

  system_port_out_u16 (io_address + WINDOW0_IRQ, (device->irq << 12) | 0x0F00);

  /* Set the station address in window 2 each time opened. */

  ETHERLINK3WINDOW(2);

  for (i = 0; i < 6; i++)
  {
    system_port_out_u8 (io_address + i, device->ethernet_address[i]);
  }

  if (device->interface_port == 3)
  {
    /* Start the thinnet transceiver. We should really wait 50ms...*/

    system_port_out_u16 (io_address + ETHERLINK3_COMMAND, StartCoax);
  }
  else if (device->interface_port == 0) 
  {
    /* 10baseT interface, enabled link beat and jabber check. */

    ETHERLINK3WINDOW (4);
    system_port_out_u16 (io_address + WINDOW4_MEDIA,
                         system_port_in_u16 (io_address + WINDOW4_MEDIA) | 
                         MEDIA_TP);
  }

  /* Switch to the stats window, and clear all stats by reading. */

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, StatsDisable);
  ETHERLINK3WINDOW (6);
  for (i = 0; i < 9; i++)
  {
    system_port_in_u8 (io_address + i);
  }
  system_port_in_u16 (io_address + 10);
  system_port_in_u16 (io_address + 12);

  /* Switch to register set 1 for normal use. */

  ETHERLINK3WINDOW(1);

  /* Accept broadcast and phys addr only. */

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND,
                       SetRxFilter | RxStation | RxBroadcast);

  //  outw(StatsEnable, io_address + ETHERLINK3_COMMAND); /* Turn on statistics. */

  device->tbusy = 0;
  device->start = 1;

  /* Enable the receiver. */

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, RxEnable);

  /* Enable transmitter. */

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, TxEnable);

  /* Allow status bits to be seen. */
  
  system_port_out_u16 (io_address + ETHERLINK3_COMMAND, SetStatusEnb | 0xFF);

  /* Ack all pending events, and set active indicator mask. */

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND,
                       AckIntr | InterruptLatch | TxAvailable | RxEarly |
                       IntReq);

  system_port_out_u16 (io_address + ETHERLINK3_COMMAND,
                       SetIntrEnb | InterruptLatch | TxAvailable | 
                       TxComplete | RxComplete /*|StatsFull*/);

  if (etherlink3_debug > 3)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                         "Opened, IRQ %d status %x.", device->irq,
                         system_port_in_u16 (io_address + ETHERLINK3_STATUS));
  }

  return TRUE;
}

/* Start a transmission on the given device. */

static bool etherlink3_start_transmit (void *data, u32 length,
                                       device_type *device)
{
#ifdef LINUX
  struct etherlink3_private *lp = (struct etherlink3_private *)device->priv;
#endif
  int io_address = device->base_addr;

  /* Transmitter timeout, serious problems. */

  if (device->tbusy) 
  {
    log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                         "Transmit timed out, Tx_status %x status %x "
                         "Tx FIFO room %d.\n",
                         system_port_in_u8 (io_address + TX_STATUS),
                         system_port_in_u16 (io_address + ETHERLINK3_STATUS),
                         system_port_in_u16 (io_address + TX_FREE));
    
    /* Issue TX_RESET and TX_START commands. */

    outw (TxReset, io_address + ETHERLINK3_COMMAND);
    outw (TxEnable, io_address + ETHERLINK3_COMMAND);
    device->tbusy = 0;
  }

#ifdef LINUX
  lp->stats.tx_bytes += skb->len;
#endif  

  if (etherlink3_debug > 4) 
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "etherlink3_start_transmit (length = %lu) called, "
                         "status %x.", length,
                         system_port_in_u16 (io_address + ETHERLINK3_STATUS));
  }

  /* Avoid timer-based retransmission conflicts. */

  if (test_and_set_bit (0, (void *) &device->tbusy) != 0)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                         "Transmitter access conflict.");
  }
  else 
  {
    device->no_irq = TRUE;
            
    /* Put out the doubleword header. 1 >> 13 means set DCG (generate
       CRC). We didn't do this earlier, which made some ethernet cards
       go crazy... */

    system_port_out_u16 (io_address + TX_FIFO, length | (1 >> 13));
    system_port_out_u16 (io_address + TX_FIFO, 0);

    /* ... and the packet rounded to a doubleword. */

#ifdef  __powerpc__
    outsl_unswapped(io_address + TX_FIFO, skb->data, (skb->len + 3) >> 2);
#else
    outsl (io_address + TX_FIFO, (u32 *) data, (length + 3) >> 2);
#endif

    if (system_port_in_u16(io_address + TX_FREE) > 1536) 
    {
      device->tbusy = 0;
    }
    else
    {
      /* Interrupt us when the FIFO has room for max-sized packet. */

      outw (SetTxThreshold + 1536, io_address + ETHERLINK3_COMMAND);
    }
    
    device->no_irq = FALSE;
  }

  /* Clear the Tx status stack. */

  {
    short tx_status;
    int i = 4;

    while (--i > 0 &&
           (tx_status = system_port_in_u8(io_address + TX_STATUS)) > 0) 
    {
#ifdef LINUX
      if (tx_status & 0x38) lp->stats.tx_aborted_errors++;
#endif
      if (tx_status & 0x30) outw(TxReset, io_address + ETHERLINK3_COMMAND);
      if (tx_status & 0x3C) outw(TxEnable, io_address + ETHERLINK3_COMMAND);
      outb (0x00, io_address + TX_STATUS); /* Pop the status stack. */
    }
  }
  
  return TRUE;
}

/* The ETHERLINK3 interrupt handler. */

static void etherlink3_interrupt (device_type *device)
{
  int io_address, status;
  int i;

  system_call_thread_name_set ("IRQ handler");
  device->no_irq = FALSE;

  while (TRUE)
  {
    system_call_irq_wait (device->irq);

    while (device->no_irq);
    i = max_interrupt_work;

    io_address = device->base_addr;
    
    if (etherlink3_debug > 4) 
    {
      status = system_port_in_u16 (io_address + ETHERLINK3_STATUS);
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "interrupt, status %x.", status);
    }
    
    while ((status = system_port_in_u16 (io_address + ETHERLINK3_STATUS)) &
           (InterruptLatch | RxComplete | StatsFull)) 
    {
      if ((status & RxComplete) == RxComplete)
      {
        etherlink3_receive (device);
      }
      
      if ((status & TxAvailable)  == TxAvailable)
      {
        if (etherlink3_debug > 5)
        {
          log_print (&log_structure, LOG_URGENCY_DEBUG, 
                     "TX room bit was handled.");
        }        

        /* There's room in the FIFO for a full-sized packet. */
        
        outw (AckIntr | TxAvailable, io_address + ETHERLINK3_COMMAND);
        device->tbusy = 0;
#ifdef LINUX
        mark_bh(NET_BH);
#endif
      }

      if ((status & (AdapterFailure | RxEarly | StatsFull | TxComplete)) != 0) 
      {
        /* Handle all uncommon interrupts. */

        /* Empty statistics. */

        if ((status & StatsFull) == StatsFull)
        {
          update_stats (device);
        }

        /* Rx early is unused. */

        if ((status & RxEarly) == RxEarly)
        {       
          etherlink3_receive (device);
          outw (AckIntr | RxEarly, io_address + ETHERLINK3_COMMAND);
        }
        if (status & TxComplete)
        {       
                /* Really Tx error. */
          
#ifdef LINUX
          struct etherlink3_private *lp = (struct etherlink3_private *)device->priv;
#endif
          short tx_status;
          int retries = 4;
          
          while (--retries > 0 &&
                 (tx_status = system_port_in_u8 (io_address + TX_STATUS)) > 0) 
          {
#ifdef LINUX
            if (tx_status & 0x38)
            {
              lp->stats.tx_aborted_errors++;
            }
#endif
            if (tx_status & 0x30)
            {
              outw (TxReset, io_address + ETHERLINK3_COMMAND);
            }
            if (tx_status & 0x3C)
            {
              outw (TxEnable, io_address + ETHERLINK3_COMMAND);
            }
            outb (0x00, io_address + TX_STATUS); /* Pop the status stack. */
          }
        }

        if (status & AdapterFailure) 
        {
          /* Adapter failure requires Rx reset and reinit. */
          
          outw (RxReset, io_address + ETHERLINK3_COMMAND);
          
          /* Set the Rx filter to the current state. */
          
          outw (SetRxFilter | RxStation | RxBroadcast
               /*             | (dev.flags & IFF_ALLMULTI ? RxMulticast : 0)
                              | (dev.flags & IFF_PROMISC ? RxProm : 0)*/,
               io_address + ETHERLINK3_COMMAND);

          /* Re-enable the receiver. */

          outw (RxEnable, io_address + ETHERLINK3_COMMAND);
          outw (AckIntr | AdapterFailure, io_address + ETHERLINK3_COMMAND);
        }
      }
      
      if (--i < 0) 
      {
        if (etherlink3_debug > 2)
        {
          log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                               "Infinite loop in interrupt, status %x.",
                               status);
        }        

        /* Clear all interrupts. */
        
        outw (AckIntr | 0xFF, io_address + ETHERLINK3_COMMAND);
        break;
      }
      
      /* Acknowledge the IRQ. */

      outw (AckIntr | IntReq | InterruptLatch, io_address + ETHERLINK3_COMMAND);
    }
    
    if (etherlink3_debug > 4) 
    {
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Exiting interrupt, status %x.",
                           system_port_in_u16(io_address + ETHERLINK3_STATUS));
    }

    system_call_irq_acknowledge (device->irq);
  }
}

#ifdef LINUX

static struct enet_statistics *etherlink3_get_stats (struct device *dev)
{
  struct etherlink3_private *lp = (struct etherlink3_private *)device->priv;
  unsigned long flags;

  /* This is fast enough not to bother with disable IRQ
     stuff. */
         
  spin_lock_irqsave(&lp->lock, flags);
  update_stats(dev);
  spin_unlock_irqrestore(&lp->lock, flags);
  return &lp->stats;
}

#endif

/*  Update statistics.  We change to register window 6, so this should
    be run single-threaded if the device is active. This is expected
    to be a rare operation, and it's simpler for the rest of the
    driver to assume that window 1 is always valid rather than use a
    special window-state variable. */

static void update_stats (device_type *device)
{
#ifdef LINUX
  struct etherlink3_private *lp = (struct etherlink3_private *)device->priv;
#endif
  int io_address = device->base_addr;

  if (etherlink3_debug > 5)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Updating the statistics.");
  }

  /* Turn off statistics updates while reading. */

  outw (StatsDisable, io_address + ETHERLINK3_COMMAND);

  /* Switch to the stats window, and read everything. */

  ETHERLINK3WINDOW (6);
#ifdef LINUX
  lp->stats.tx_carrier_errors   += system_port_in_u8 (io_address + 0);
  lp->stats.tx_heartbeat_errors += system_port_in_u8 (io_address + 1);

  /* Multiple collisions. */       system_port_in_u8 (io_address + 2);

  lp->stats.collisions          += system_port_in_u8 (io_address + 3);
  lp->stats.tx_window_errors    += system_port_in_u8 (io_address + 4);
  lp->stats.rx_fifo_errors      += system_port_in_u8 (io_address + 5);
  lp->stats.tx_packets          += system_port_in_u8 (io_address + 6);
#endif

  /* Rx packets */              

  system_port_in_u8 (io_address + 7);

  /* Tx deferrals */

  system_port_in_u8 (io_address + 8);

  system_port_in_u16 (io_address + 10);    /* Total Rx and Tx octets. */
  system_port_in_u16 (io_address + 12);

  /* Back to window 1, and turn statistics back on. */

  ETHERLINK3WINDOW (1);
  outw (StatsEnable, io_address + ETHERLINK3_COMMAND);
  return;
}

/* Receive a packet from the given ethernet card. */

static int etherlink3_receive (device_type *device)
{
  int n;
#ifdef LINUX
  struct etherlink3_private *lp = (struct etherlink3_private *) device->priv;
#endif
  int io_address = device->base_addr;
  short rx_status;

  if (etherlink3_debug > 5)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "In etherlink3_receive (), status %x, rx_status %x.",
                         system_port_in_u16 (io_address + ETHERLINK3_STATUS),
                         system_port_in_u16 (io_address + RX_STATUS));
  }

  while ((rx_status = system_port_in_u16(io_address + RX_STATUS)) > 0) 
  {
    if (rx_status & 0x4000)  /* Error, update stats. */
    {
#ifdef LINUX
      short error = rx_status & 0x3800;
#endif

      outw (RxDiscard, io_address + ETHERLINK3_COMMAND);
#ifdef LINUX
      lp->stats.rx_errors++;
      switch (error) 
      {
        case 0x0000: lp->stats.rx_over_errors++; break;
        case 0x0800: lp->stats.rx_length_errors++; break;
        case 0x1000: lp->stats.rx_frame_errors++; break;
        case 0x1800: lp->stats.rx_length_errors++; break;
        case 0x2000: lp->stats.rx_frame_errors++; break;
        case 0x2800: lp->stats.rx_crc_errors++; break;
      }
#endif
    }
    else 
    {
      short packet_length = rx_status & 0x7FF;
#ifdef LINUX
      struct sk_buff *skb;

      skb = dev_alloc_skb (packet_length + 5);
      lp->stats.rx_bytes += packet_length;
#endif
      if (etherlink3_debug > 4)
      {
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                             "Receiving packet size %d status %x.",
                             packet_length, rx_status);
      }

#ifdef LINUX
      if (skb != NULL) 
#endif
      {
#ifdef LINUX
        skb->dev = dev;
        skb_reserve(skb, 2);     /* Align IP on 16 byte */
#endif

        insl (io_address + RX_FIFO, device->input_buffer,
              (packet_length + 3) >> 2);

        if (etherlink3_debug > 4)
        {
          log_print_formatted 
            (&log_structure, LOG_URGENCY_DEBUG, 
             "Packet from %X:%X:%X:%X:%X:%X to %X:%X:%X:%X:%X:%X of type 0x%X.",
             ((ipv4_ethernet_header_type *) device->input_buffer)->source_address[0],
             ((ipv4_ethernet_header_type *) device->input_buffer)->source_address[1],
             ((ipv4_ethernet_header_type *) device->input_buffer)->source_address[2],
             ((ipv4_ethernet_header_type *) device->input_buffer)->source_address[3],
             ((ipv4_ethernet_header_type *) device->input_buffer)->source_address[4],
             ((ipv4_ethernet_header_type *) device->input_buffer)->source_address[5],
             ((ipv4_ethernet_header_type *) device->input_buffer)->destination_address[0],
             ((ipv4_ethernet_header_type *) device->input_buffer)->destination_address[1],
             ((ipv4_ethernet_header_type *) device->input_buffer)->destination_address[2],
             ((ipv4_ethernet_header_type *) device->input_buffer)->destination_address[3],
             ((ipv4_ethernet_header_type *) device->input_buffer)->destination_address[4],
             ((ipv4_ethernet_header_type *) device->input_buffer)->destination_address[5],
             system_byte_swap_u16 (((ipv4_ethernet_header_type *) device->input_buffer)->protocol_type));
        }

        outw (RxDiscard, io_address + ETHERLINK3_COMMAND); /* Pop top Rx packet. */
#ifdef LINUX
        skb->protocol = eth_type_trans(skb,dev);
        netif_rx(skb);
        lp->stats.rx_packets++;
#endif
        
        /* Check if this packet should be delivered somewhere. */

        for (n = 0; n < device->number_of_targets; n++)
        {          
          if (device->target[n].protocol_type ==
              ((ipv4_ethernet_header_type *) device->input_buffer)->protocol_type)
          {
            message_parameter_type message_parameter;

            message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
            message_parameter.message_class = IPC_ETHERNET_PACKET_RECEIVED;
            message_parameter.length = packet_length;
            message_parameter.block = FALSE;
            message_parameter.data = device->input_buffer;

            if (etherlink3_debug > 2)
            {
              log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                                   "Sending to mailbox ID %u", 
                                   device->target[n].mailbox_id);
            }

            system_call_mailbox_send (device->target[n].mailbox_id,
                                      &message_parameter);
            break;
          }
        }
        continue;
      }
      outw (RxDiscard, io_address + ETHERLINK3_COMMAND);
#ifdef LINUX
      lp->stats.rx_dropped++;
#endif
    }
    system_port_in_u16 (io_address + ETHERLINK3_STATUS);                /* Delay. */
    while (system_port_in_u16(io_address + ETHERLINK3_STATUS) & 0x1000)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Waiting for 3c509 to discard packet, status %x.",
                           system_port_in_u16 (io_address + ETHERLINK3_STATUS));
    }
  }
  return 0;
}

#ifdef LINUX

/* Set or clear the multicast filter for this adapter. */

static void set_multicast_list (struct device *dev)
{
  unsigned long flags;
  struct etherlink3_private *lp = (struct etherlink3_private *)device->priv;
  int io_address = device->base_addr;

  if (etherlink3_debug > 1) 
  {
    static int old = 0;
    if (old != device->mc_count) 
    {
      old = device->mc_count;
      log_print_formatted (0, "3c509", 
                           "%s: Setting Rx mode to %d addresses.\n",
                           device->name, device->mc_count);
    }
  }
  spin_lock_irqsave(&lp->lock, flags);
  if (device->flags&IFF_PROMISC) 
  {
    outw(SetRxFilter | RxStation | RxMulticast | RxBroadcast | RxProm,
         io_address + ETHERLINK3_COMMAND);
  }
  else if (device->mc_count || (device->flags & IFF_ALLMULTI)) 
  {
    outw (SetRxFilter | RxStation | RxMulticast | RxBroadcast, 
          io_address + ETHERLINK3_COMMAND);
  }
  else
  {
    outw (SetRxFilter | RxStation | RxBroadcast, io_address + 
          ETHERLINK3_COMMAND);
  }
  spin_unlock_irqrestore(&lp->lock, flags);
}

#endif

#ifdef LINUX

static int etherlink3_close (device_type *device)
{
  int io_address = device->base_addr;

  if (etherlink3_debug > 2)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                         "Shutting down ethercard.");
  }

  device->tbusy = 1;
  device->start = 0;

  /* Turn off statistics ASAP.  We update lp->stats below. */

  outw (StatsDisable, io_address + ETHERLINK3_COMMAND);

  /* Disable the receiver and transmitter. */

  outw (RxDisable, io_address + ETHERLINK3_COMMAND);
  outw (TxDisable, io_address + ETHERLINK3_COMMAND);

  if (device->interface_port == 3)
  {
    /* Turn off thinnet power.  Green! */

    outw (StopCoax, io_address + ETHERLINK3_COMMAND);
  }
  else if (device->interface_port == 0) 
  {
    /* Disable link beat and jabber, interface_port may change ere
       next open(). */

    ETHERLINK3WINDOW (4);
    outw (system_port_in_u16 (io_address + WINDOW4_MEDIA) & ~MEDIA_TP,
          io_address + WINDOW4_MEDIA);
  }

#ifdef LINUX
  free_irq(device->irq, dev);
#endif

  /* Switching back to window 0 disables the IRQ. */

  ETHERLINK3WINDOW (0);

  /* But we explicitly zero the IRQ line select anyway. */

  system_port_out_u16 (io_address + WINDOW0_IRQ, 0x0F00);

  update_stats(dev);
  return 0;
}

#endif

ipv4_ethernet_header_type ethernet_header;

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id, 
                               device_type *device)
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
        if (!etherlink3_start_transmit (data, message_parameter.length, device))
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

/* Handle this ethernet device. */

static void handle_device (device_type *device)
{
  ipc_structure_type ipc_structure;

  if (!etherlink3_open (device))
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY, 
               "Couldn't open ethernet device.");
    return;
  }

  /* Create the service. */

  if (ipc_service_create ("ethernet", &ipc_structure, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create ethernet service.");
    return;
  }

  /* Main loop. */

  system_call_thread_name_set ("Service handler");

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

/* Main function. */

int main (void)
{
  bool found;

  /* Set up our name. */

  system_call_process_name_set (PACKAGE_NAME);
  system_call_thread_name_set ("Initialising");

  if (log_init (&log_structure, PACKAGE_NAME, &empty_tag) != LOG_RETURN_SUCCESS)
  {
    return -1;
  }

  do
  {
    device_type *device;
    device_type **device_pointer = &device;
    memory_allocate ((void **) device_pointer, sizeof (device_type));
    found = etherlink3_probe (device);

    if (!found)
    {
      memory_deallocate ((void **) device_pointer);
    }
    else
    {
      if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
      {
        handle_device (device);
        return 0;
      }
    }
  } while (found);

  system_call_process_parent_unblock ();

  return 0;
}
