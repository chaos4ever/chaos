/* $Id$ */
/* Abstract: NE2000 server for chaos. */
/* Author noah williamsson <tm@ns2.crw.se>
          Per Lundberg <perlun@gmail.com> */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

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

#include <config.h>
#include "ne2000.h"

// FIXME: very bad, but added because the code relies on it.
#define ERROR 1

/* An empty tag list. */

tag_type empty_tag =
{
  0, 0, ""
};

log_structure_type log_structure;

u8 ne_reset_chip(device_type *);
void ne_init_chip(device_type *, u8);
void ne_int_handler(device_type *);
void ne_recv(device_type *);
bool ne2000_start_transmit (void *data, u32 length, device_type *device);
void ne_download_buf(device_type *, u16, u8 *, u16);
void ne_handle_overflow(device_type *);
u16 hex2dec(u8 *);

//device_type card;
u32 message[1024];
u32 data_buffer[1518]; /* xxx - was 400 before.. */
struct ne_user_t users[MAX_USERS];
u8 num_users = 0;


u8 ne_reset_chip (device_type *n)
{
  u16 i;

  outb (inb (n->io + NE_RESET), n->io + NE_RESET);

  /* 50 is probably far too much here, that means a delay for up to 50
     seconds at worst. */
  for (i = 0; i < 50; i++)
  {
    if ((inb (n->io + NE_R0_ISR) & BIT_ISR_RESET) != 0)
      break;

    /* Sleep for one second. */

    system_sleep (1);
  }

  outb (BIT_ISR_RESET, n->io + NE_R0_ISR); /* Ack. intrerrupt */
  if (i != 50) {
    return 0;
  }

  log_print_formatted (&log_structure, LOG_URGENCY_ERROR, "-- timeout while waiting for reset ACK.");
  return ERROR;
}


void ne_init_chip (device_type *n, u8 startp)
{
  u16 i;

  /* This is what the datasheet recommends */
  /* Read more in ne2000.h */
  n->status |= NIC_INIT;
  for (i = 0;i < (sizeof (ne_init_program)/sizeof (ne_program));i++)
    outb (ne_init_program[i].value, n->io + ne_init_program[i].offset);

  outb (NE_NODMA + NE_PAGE1 + NE_STOP, n->io); /* Switch to page 1 */

  /* Hmm, can you change your phys addr? ;-) */
  /* XXX - this should be cleaned up! */
#ifdef TEST
  outb (0xc0, n->io+1);
  outb (0xff, n->io+2);
  outb (0xee, n->io+3);
  outb (0xc0, n->io+4);
  outb (0xff, n->io+5);
  outb (0xee, n->io+6);
#else
  for (i = 0;i < 6;i++)
  {
     outb (n->prom[i], n->io + i + 1);
     if (inb (n->io + i + 1) != n->prom[i])
       log_print_formatted (&log_structure, LOG_URGENCY_ERROR, "-- Argh. Cannot set physical address");
  }
#endif

  outb (NE_PG_RX_START, n->io + NE_PG_CURRENT); /* Init. current ptr */
  outb (NE_NODMA+NE_PAGE0+NE_STOP, n->io);

  n->status &= ~NIC_INT;
  n->status &= ~NIC_UP;
  n->status &= ~NIC_DOWN;
  if (startp)
  {
    outb (0xff, n->io + NE_R0_ISR);
    outb (BIT_ISR_ALL, n->io + NE_R0_IMR);

    /* Wheee. Start the card */
    outb (NE_NODMA + NE_START + NE_PAGE0, n->io);
    outb (NE_DEF_TXCFG, n->io + NE_R0_TCR); /* We're ready to transmit */
    outb (NE_DEF_RXCFG, n->io + NE_R0_RCR); /* ..and receive. */
    n->status |= NIC_UP;
  }
  else {
    n->status |= NIC_DOWN;
  }

  n->status &= ~NIC_INIT;
}

/* FIXME: rewrite this! */
void ne_int_handler (device_type *card __attribute__ ((unused)))
{
  int sreg;
  u8 num_handled;

  card->num_interrupts++;
  
  if (debug >= 2)
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Entering interrupt again");
  
  if (card->status & NIC_INT)
    log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "-- What, entering interrupt again???");
  
  card->status |= NIC_INT;
  
  /* Switch to page 0 */
  outb (NE_NODMA + NE_PAGE0, card->io);
  
  num_handled = 0;
  while (!(card->status & NIC_BUSY) && 
         (sreg = inb (card->io + NE_R0_ISR)) != 0 && 
         num_handled++ < MAX_INT_WORK)
  {
    //      sreg = inb (card->io+NE_R0_ISR);
    if (debug == 2)
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** ISR:%2.2x ISM:%2.2x", sreg, inb(card->io + NE_R0_IMR));
    
    if (!(card->status & NIC_UP))
      log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "-- Weird, this nic should be stopped!");
    
    if (sreg & BIT_ISR_OVERFLW)
    {
      if (debug >= 1)
        log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "-- Overflow in rx buffer");
      
      /* This a little more than 10ms. */
#ifdef CAN_WAIT_10_MS
      ne_handle_overflow (card);
      
#else /* XXX - this code probably DONT work */
#error "Tomtevarning; DEN HÄR KODEN FUNKAR INTE"
      {
        u32 tomte;
        
        if (debug >= 1)
          log_print_formatted ("** resetting card->.");
        
        tomte = NE_RST_OVERRUN;
        card->status |= NIC_BUSY;
        syscall_message_send (syscall_process_get_pid_by_name ("ne"), &tomte, 4);
        break;
        /* Maybe we should ack. some interrupts.. */
      }
#endif
    }
    
    if (sreg & (BIT_ISR_RX + BIT_ISR_RX_ERR))
    {
      if (debug >= 2)
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Packet received");

      ne_recv (card);
    }
    
    if (sreg & BIT_ISR_TX)
      if (debug >= 1)
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Transmit");
    
    if (sreg & BIT_ISR_TX_ERR)
      if (debug >= 1)
        log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "-- Transmit error");

    if (sreg & BIT_ISR_CNTRS)
    {
      u8 x,y,z;
      x = inb (card->io+NE_R0_CNTR0);
      y = inb (card->io+NE_R0_CNTR1);
      z = inb (card->io+NE_R0_CNTR2);
      outb (BIT_ISR_CNTRS, card->io+NE_R0_ISR);
      if (debug >= 2)
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "-- COUNTERS: frame:%d crc:%d missed:%d", x,y,z);
    }
  
    if (sreg & BIT_ISR_RDC)
    {
      if (debug >= 2)
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "-- Ignoring RDC interrupt");

      outb (BIT_ISR_RDC, card->io + NE_R0_ISR);

      /* XXX - lock here for debugging purposes */
      //        for(;;);
    }


    /* XXX - varför i helvete sitter det en // här? */
       /* Ack. all interrupts */
       //      outb (0xff, card->io + NE_R0_ISR);


       /* XXX - does this enable the card again?
          does it make the card generate intr.:s. again? */

       outb (NE_NODMA + NE_PAGE0 + NE_START, card->io);
  }

  if (num_handled == MAX_INT_WORK) {
    if (debug >= 1) {
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "-- ** -- Max interrupt work done!!");
    }
  }

  card->status &= ~NIC_INT;
  if (debug >= 2) {
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** ISR:%2.2x ISM:%2.2x device_type status:%.2x (Leaving interrupt handler)", inb(card->io + NE_R0_ISR),
                         inb (card->io + NE_R0_IMR), card->status);
  }
}

void ne_recv (device_type *card)
{
  /* xxx - moved data_buffer to global .. */
  u8 *data = ((u8 *) data_buffer) + ((u8) 8);
  u8 rx_packets = 0;
  u8 rx_page;
  u8 frame, next_frame, pkt_status;
  u16 current_offset, pkt_len;
  struct ne_pkt_hdr pkt_hdr;
  int n;

  frame = 0;
  current_offset = 0;
  for (rx_packets = 0;rx_packets < 10;rx_packets++)
  {
    /* Switch to page 1 */  
    outb (NE_NODMA + NE_PAGE1, card->io);

    /* Get the packet ptr from current page */
    rx_page = inb (card->io + NE_PG_CURRENT);

    /* Switch back to page 0 */
    outb (NE_NODMA + NE_PAGE0, card->io);


    frame = inb (card->io + NE_BOUNDARY) + 1;
    if (frame >= NE_PG_STOP)
    {
      if (debug >= 2) {
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "-- hmm? weirdo. fixing frame: %x => %x", frame, NE_PG_RX_START);
      }

      frame = NE_PG_RX_START;
    }
    
    if (debug >= 2)
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** rxpage:0x%x frame:0x%x startpg:0x%x stoppg:0x%x",
              rx_page, frame, NE_PG_TX_START, NE_PG_RX_START);


    if (frame == rx_page) /* D0h, we're done */
    {
      if (debug >= 2)
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** wow, we're done recv!");

      break;
    }

    current_offset = frame << 8;




    /* We're about to get the header from the chip */

    if (card->status & NIC_DMA)
      log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "Say what, DMA active in recv?");
   
   
    card->status |= NIC_DMA;
    outb (NE_NODMA + NE_PAGE0 + NE_START, card->io);

    /* We're gonna read some stuff soon */
    outb (sizeof (struct ne_pkt_hdr), card->io+NE_R0_RBCR0);
    outb (0, card->io + NE_R0_RBCR1);

    outb (0, card->io + NE_R0_RSAR0);
    outb (frame, card->io + NE_R0_RSAR1);

    /* Ok, let's start */
    outb (NE_RREAD + NE_START, card->io);
    *((u32 *)&pkt_hdr) = 0;
    if (debug >= 2)
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                           "** Before: stat:0x%x next:0x%x len:0x%x",
                           pkt_hdr.status, pkt_hdr.next, pkt_hdr.len);
 
    /* xxx - chnaged void * from u32 *, and till _u16 ist.f. u32 etc.. */
    
    system_port_in_u16_string (card->io + NE_DATAPORT,
                               (void *) &pkt_hdr, sizeof (struct ne_pkt_hdr) >> 1);
       
    /* Ack. dma */
    outb (BIT_ISR_RDC, card->io + NE_R0_ISR);
    card->status &= ~NIC_DMA;
    /* Ok, have the header now */

    pkt_len = pkt_hdr.len-sizeof (struct ne_pkt_hdr);
    pkt_status = pkt_hdr.status;
    next_frame = frame+1+((pkt_len+4)>>8);
    if (debug >= 1)
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Packet: stat:0x%x next:0x%x (nxtfrm:0x%x) len:0x%x",
              pkt_hdr.status, pkt_hdr.next, next_frame, pkt_hdr.len);
      
    if (pkt_hdr.next != next_frame &&
        pkt_hdr.next != next_frame+1 &&
        pkt_hdr.next != next_frame - (0x80-0x4c) &&
        pkt_hdr.next != next_frame + 1 - (0x80-0x4c))
      {
        outb (rx_page-1, card->io + NE_BOUNDARY);
        log_print_formatted (&log_structure, LOG_URGENCY_ERROR, "-- ERROR!!!");
        log_print_formatted (&log_structure, LOG_URGENCY_ERROR, "[%d:%d:%d:%d] hdr.nxt(%d) nxtfrm(%d) frm(%d)", pkt_hdr.next != next_frame, 
                pkt_hdr.next != next_frame+1, pkt_hdr.next != next_frame - (0x80-0x4c), 
                pkt_hdr.next != next_frame + 1 - (0x80-0x4c), pkt_hdr.next, next_frame, frame);

//        continue;
      }

      if (pkt_len < 60 || pkt_len > 1518)   
        log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "-- Oops. Weird packet length, 0x%x", pkt_len);
   
      /* Do something useful */
      if ((pkt_status & 0x0f) == BIT_RSR_RXOK)
      {
        /* Wheee.. We received a good packet */
#define PROTO ((data[12]*256)+data[13])

        if (debug >= 1)
          log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "++ Good packet");

        ne_download_buf (card, pkt_len, data, current_offset+sizeof (pkt_hdr));
        if (debug >= 2)
          log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Download done!");


        /* Check if this packet should be delivered somewhere. */
        
        for (n = 0; n < card->number_of_targets; n++)
        {          
          if (card->target[n].protocol_type ==
              ((ipv4_ethernet_header_type *) data)->protocol_type)
          {
            message_parameter_type message_parameter;
            
            message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
            message_parameter.message_class = IPC_ETHERNET_PACKET_RECEIVED;
            message_parameter.length = pkt_len + 8; /* probably 8 bytes of header. */
            message_parameter.block = FALSE;
            message_parameter.data = data;
            
            if (debug >= 1)
            {
              log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
                                   "Sending to mailbox ID %u", 
                                   card->target[n].mailbox_id);
            }
            
            system_call_mailbox_send (card->target[n].mailbox_id,
                                      &message_parameter);
            break;
          }
        }
      } /* End of RX ok code */
      else
      {
         log_print_formatted (&log_structure, LOG_URGENCY_WARNING, "-- Bogus packet with size(0x%x/%d):status(%x)", pkt_len, pkt_len, pkt_status);
         card->num_dropped++;
      }

      if (debug >= 2) {
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x => %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x]", 
                             data[6], data[7], data[8],
                             data[9], data[10], data[11],
                             data[0], data[1], data[2],
                             data[3], data[4], data[5]);
      }

      next_frame = pkt_hdr.next;
      outb (next_frame-1, card->io+NE_BOUNDARY);
   }

   outb (BIT_ISR_RX + BIT_ISR_RX_ERR, card->io + NE_R0_ISR);
}

#define NE_SANITY_CHECK
bool ne2000_start_transmit (void *data, u32 length, device_type *device)
{
  int nic_base = device->io;
  time_type dma_start;

  int start_page = 64;
#ifdef NE_SANITY_CHECK
  int retries = 0;
#endif
  u8 scratch[60];

  log_print(&log_structure, LOG_URGENCY_DEBUG, "in ne2000_start_transmit");

  /* Turn off interrupts. */
  outb_p(0x00, nic_base + NE_R0_IMR);
  
  /* Round the count up for word writes.  Do we need to do this?
     What effect will an odd byte count have on the 8390?
     I should check someday. */
  
  if (length & 0x01) {
    length++;
  }
  
  /* This *shouldn't* happen. If it does, it's the last thing you'll see */
  if ((device->status & NIC_DMA) == NIC_DMA)
  {
    log_print_formatted(&log_structure, LOG_URGENCY_EMERGENCY,
                        "DMAing conflict in ne_block_output."
                        "[DMAstat:%d][irqlock:%d]\n",
                        device->status & NIC_DMA, device->status & NIC_INT);
    
    /* Turn 8390 interrupts back on. */
    outb_p(BIT_ISR_ALL, nic_base + NE_R0_IMR);

    return FALSE;
  }

  device->status |= NIC_DMA;
  /* We should already be in page 0, but to be safe... */
  outb_p(NE_PAGE0 + NE_START + NE_NODMA, nic_base + NE_R0_CMD);
  
#ifdef NE_SANITY_CHECK
retry:
#endif
  
#ifdef NE8390_RW_BUGFIX
  /* Handle the read-before-write bug the same way as the
     Crynwr packet driver -- the NatSemi method doesn't work.
     Actually this doesn't always work either, but if you have
     problems with your NEx000 this is better than nothing! */
  
  outb_p(0x42, nic_base + NE_R0_RCNT0);
  outb_p(0x00, nic_base + NE_R0_RCNT1);
  outb_p(0x42, nic_base + NE_R0_RSAR0);
  outb_p(0x00, nic_base + NE_R0_RSAR1);
  outb_p(E8390_RREAD + E8390_START, nic_base + NE_R0_CMD);
  /* Make certain that the dummy read has occurred. */
  system_sleep(1);
#endif
  
  outb_p(BIT_ISR_RDC, nic_base + NE_R0_ISR);

  /* The minimum length of a frame that can be successfully sent is 60
     bytes, minus FCS. */

  if (length < 60) {
    memory_copy(&scratch, data, length); // what if it has been word-aligned...?
    memory_set_u8(&scratch[length], 0, 60 - length);
    data = &scratch;
    length = 60;
  }

  /* Now the normal output. */
  outb_p(length & 0xFF, nic_base + NE_R0_RBCR0);
  outb_p(length >> 8,   nic_base + NE_R0_RBCR1);
  
  outb_p(0, nic_base + NE_R0_RSAR0);
  outb_p(start_page, nic_base + NE_R0_RSAR1);
  
  outb_p(NE_RWRITE + NE_START, nic_base + NE_R0_CMD);
  log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE,
                      "Sending %ld bytes...", length);
  outsw(nic_base + NE_DATAPORT, data, length >> 1);
  
  dma_start = time_get ();
  
#ifdef NE_SANITY_CHECK
  /* This was for the ALPHA version only, but enough people have
     been encountering problems so it is still here. */
  
  if (debug > 1)
  {
    /* DMA termination address check... */
    unsigned int addr, tries = 20;
    do {
      int high = inb_p(nic_base + NE_R0_RSAR1);
      int low = inb_p(nic_base + NE_R0_RSAR0);
      addr = (high << 8) + low;
      if ((start_page << 8) + length == addr) {
        break;
      }
    } while (--tries > 0);
    
    if (tries <= 0)
    {
      log_print_formatted(&log_structure, LOG_URGENCY_WARNING, "Tx packet transfer address mismatch,"
             "%#4.4lx (expected) vs. %#4.4x (actual).\n",
                          (start_page << 8) + length, addr);
      if (retries++ == 0)
        goto retry;
    }
  }
#endif
  
  while ((inb_p(nic_base + NE_R0_ISR) & BIT_ISR_RDC) == 0)
    /* Wait for up to 1 second. This is obviously far too long, the
       Linux code uses 20ms but this is much easier to
       implement. :-) */
    if (time_get () - dma_start > 0) { 
      log_print(&log_structure, LOG_URGENCY_WARNING, "timeout waiting for Tx RDC.");
      ne_reset_chip(device);
      ne_init_chip(device, 1);

      /* Turn 8390 interrupts back on. */
      outb_p(BIT_ISR_ALL, nic_base + NE_R0_IMR);
      
      return FALSE;
    }
  
  outb_p(BIT_ISR_RDC, nic_base + NE_R0_ISR);  /* Ack intr. */
  device->status &= ~NIC_DMA;

  /* Okay, let's send this frame. */
  outb_p(NE_NODMA + NE_PAGE0, nic_base + NE_R0_CMD);
  
  if (inb_p(nic_base) & NE_TRANS)
  {
    log_print(&log_structure, LOG_URGENCY_WARNING, "ne2000_start_transmit() called with the transmitter busy");

    /* Turn 8390 interrupts back on. */
    outb_p(BIT_ISR_ALL, nic_base + NE_R0_IMR);
    
    return FALSE;
  }

  outb_p(length & 0xFF, nic_base + NE_R0_TBCR0);
  outb_p(length >> 8, nic_base + NE_R0_TBCR1);
  outb_p(start_page, nic_base + NE_R0_TPSR);
  outb_p(NE_NODMA + NE_TRANS + NE_START, nic_base + NE_R0_CMD);

  /* Turn 8390 interrupts back on. */
  outb_p(BIT_ISR_ALL, nic_base + NE_R0_IMR);

  return TRUE;
}


void ne_download_buf (device_type *n, u16 len, u8 *data, u16 offset)
{
  if (debug >= 2)
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** ne_download_buf(): Gonna read 0x%x bytes", len);

  n->status |= NIC_DMA;
  outb (NE_NODMA + NE_PAGE0 + NE_START, n->io);
  outb (len & 0xFF, n->io + NE_R0_RBCR0);
  outb (len >> 8, n->io + NE_R0_RBCR1);
  outb (offset & 0xFF, n->io + NE_R0_RSAR0);
  outb (offset >> 8, n->io + NE_R0_RSAR1);
  outb (NE_RREAD + NE_START, n->io);
  
  if (debug >= 2)
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** ne_download_buf(): now we're about to read..");

  system_port_in_u16_string (n->io + NE_DATAPORT, (u16 *) data, len >> 1);
  if (len & 0x01)
    data[len-1] = inb (n->io + NE_DATAPORT);

  if (debug >= 2)
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** ne_download_buf(): all read done..");

  outb (BIT_ISR_RDC, n->io + NE_R0_ISR);
  n->status &= ~NIC_DMA;

}




void ne_handle_overflow (device_type *device)
{
 
  u8 txing, resend = 0;
  u16 x;
  
  txing = inb (device->io) & NE_TRANS;

  /* stop the card->. */
  outb (NE_NODMA + NE_PAGE0 + NE_STOP, device->io);
  if (debug >= 1)
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "-- Huh.. Overflow.. Sleeping one second!");

#if 0
  syscall_process_control (syscall_process_get_pid_by_name ("ne"),
                          PROCESS_SLEEP, 1000); /* Hmm, what about 10? */
#endif

  for(x = 1;x != 0;x++); /* XXX - Hmm, this is probably ok for now :) */
  outb (0, device->io + NE_R0_RBCR0);
  outb (0, device->io + NE_R0_RBCR1);

  if (txing)
  {
    if (!(inb (device->io+NE_R0_ISR) & (BIT_ISR_TX+BIT_ISR_TX_ERR)))
      resend++;
  }


  outb (BIT_TCR_LB, device->io + NE_R0_TCR); /* Enter loopback mode */
  outb (NE_NODMA + NE_PAGE0+NE_START, device->io);

  if (debug >= 1)
    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** - Clearing rx buffer ring");
  /* Clear RX ring buffer */
  ne_recv (device);

  /* Ack. overflow interrupt */
  outb (BIT_ISR_OVERFLW, device->io+NE_R0_ISR);

  /* Leave loopback mode & resend any stopped packets */
  outb (0, device->io + NE_R0_TCR);
  if (resend)
    outb (NE_NODMA + NE_PAGE0 + NE_START + NE_TRANS, device->io);

  /* We're finnished */

  log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "** overflow reset done!");
  device->status &= ~NIC_BUSY;
}

u16 hex2dec (u8 *s)
{
  u16 ret = 0;
  u8 shift;
  

  if (s[0] != '0' || s[1] != 'x')
    return 0;

  s += 2;
  for (shift = 0;s[shift];shift++);
  shift = (shift-1) * 4;
  while (*s >= '0')
  {
    if (*s >= '0' && *s <= '9')
      ret += (*s - '0')<<shift;
    else
       ret += (*s - 'a' + 10)<<shift;

    s++;
    shift -= 4;
  }

  return ret;
}

/* Open the ne2000 device. */

static bool ne2000_open (device_type *device)
{
  /* Create a thread for the IRQ handler and set up an IRQ handler loop. */
  if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW) {
    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "in IRQ thread");

    if (system_call_irq_register (device->irq, PACKAGE_NAME) != SYSTEM_RETURN_SUCCESS)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "$@!# Couldn't register irq %d :(", device->irq);
      /* FIXME: Deallocate I/O space. */

      return FALSE;
    }

    system_call_thread_name_set ("IRQ handler");

    while (TRUE) {
      system_call_irq_wait (device->irq);
      ne_int_handler (device);
      system_call_irq_acknowledge (device->irq);
    }
  }

  ne_init_chip (device, 1);
  return TRUE;
}

// FIXME: should have an accompanying ne2000_close ()...

/* Probe for an ne2000 compatible device. */

static bool ne2000_probe (device_type *device)
{
  u8 chk = 0, x;
  u32 i;

  if (!device || !device->io || !device->irq)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "-- Invalid io(0x%x) address or irq(%d)", device->io, device->irq);
    return FALSE;
  }

  if (system_call_port_range_register (device->io, 0x1F, PACKAGE_NAME))
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "-- Cannot allocate 0x%x => 0x%x", device->io, device->io + 0x1f);
    return FALSE;
  }

  chk = system_port_in_u8 (device->io);
  if (chk == 0xff) /* No card found */
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "-- Nothing found on 0x%x", device->io);

    /* FIXME: deallocate I/O space */
    return FALSE;
  }

  /* Check if it really is a NE2k card */
  outb (NE_NODMA + NE_PAGE1 + NE_STOP, device->io); /* Stop the card */
  x = inb (device->io + NE_R0_CNTR0);
  outb (0xff, device->io + NE_R0_CNTR0);
  outb (NE_NODMA + NE_PAGE0, device->io);
  inb (device->io + NE_R0_CNTR0); /* Clear the counter by reading */

  if (inb (device->io + NE_R0_CNTR0) != 0) /* Ooops ;) */
  {
    outb (chk, device->io);
    outb (chk, device->io + NE_R0_CNTR0);
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "Something, but not a NE2k card, found. Aborting");
    return FALSE;
  }

  ne_reset_chip (device);

  /* We don't ack _all_ interrupts in ne_reset_chip(),
     so we do it here instead */
  outb (0xff, device->io + NE_R0_ISR);


  /* Init registers and shit */
  for (i = 0; i < sizeof (ne_preinit_program)/ sizeof (ne_program); i++)
  {
    outb (ne_preinit_program[i].value, device->io + ne_preinit_program[i].offset);
  }

  /* It's time to read the station address prom now */
  for (i = 0, x = 0; i < 32; i += 2)
  {
    device->prom[i] = inb (device->io + NE_DATAPORT);
    device->prom[i+1] = inb (device->io + NE_DATAPORT);
    if (device->prom[i] != device->prom[i+1])
      x = 1;
  }

  if (x) /* This isn't a 16 bit card */
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "Haha! Your card isn't supported. Get a 16 bit card");
    return FALSE;
  }

  for (i = 0;i < 16;i++) {
    device->prom[i] = device->prom[i + i];
  }

  for (i = 0; i < 6; i++) {
    device->ethernet_address[i] = device->prom[i];
  }

  log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "MAC address is %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x"
         " and signature [0x%x 0x%x]",
         device->prom[0], device->prom[1], device->prom[2],
         device->prom[3], device->prom[4], device->prom[5], 
         device->prom[14], device->prom[15]);

  outb (0x49, device->io + NE_R0_DCR); /* Set the card in word-wide mode */

  /* this code should probably wait here until the IRQ handler etc has
     been setup... */
  
  ne_init_chip (device, 0);
  return TRUE;
}

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id, 
                               device_type *device)
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
        if (!ne2000_start_transmit (data, message_parameter.length, device))
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Failed to send an ethernet packet.");

          /* FIXME: Do something useful. We should probably send an
             IPC message back to the sender regardless, to notify that
             the package was sent/not sent. */
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

  if (!ne2000_open (device))
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
  device_type *device;
  bool found;

  /* Set up our name. */

  system_call_process_name_set (PACKAGE_NAME);
  system_call_thread_name_set ("Initialising");

  if (log_init (&log_structure, PACKAGE_NAME, &empty_tag) != LOG_RETURN_SUCCESS)
  {
    return -1;
  }

  /* Give some credits to Noah. :-) */

  log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "NE2000 server by noah williamsson");

  memory_allocate ((void **) &device, sizeof (device_type));
  
  // FIXME: Hardwired for now. These are the values that Bochs use
  // by default, that's why I'm choosing them.
  device->io = 0x300;
  device->irq = 3;

  device->num_dropped = 0;
  device->num_interrupts = 0;
  
  found = ne2000_probe (device);
  
  if (!found)
  {
    log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE, "!found");
    memory_deallocate ((void **) &device);
  }
  else
  {
    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      handle_device (device);
      return 0;
    }
  }

  system_call_process_parent_unblock ();

  return 0;
}

/* Can probably be removed soon... */
#if 0
int main(int argc, char *argv[]) 
{
  device_type *card;
  ipc_structure_type ipc_structure;

  system_call_process_name_set ("ne2000");
  system_call_thread_name_set ("Main thread");

  if (log_init (&log_structure, PACKAGE_NAME, &empty_tag) !=
      LOG_RETURN_SUCCESS)
  {
    return -1;
  }


  // FIXME: we could do some probing here. on port 0x300, 0x280 and so
  // forth...
  if (argc != 3)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "Error: Probing is not implemented, you must specify I/O port/IRQ like this: <io> <irq>\n"
            "Ex: module=ne2000 0x260 3");

    return 0;
  }

  memory_allocate ((void **) &card, sizeof(device_type));

  card->num_dropped = 0;
  card->num_interrupts = 0;


  card->io = hex2dec (argv[1]);
  card->irq = argv[2][0] - '0';

  if (argv[2][1] >= '0')
    card->irq = 10 + argv[2][1] - '0';

  if (card->irq == 2)
    card->irq = 9;

  /* Do some basic sanity checking of the port ranges here. */
  if (card->io < 0x240 || card->io > 0x360 || card->irq <= 1 || card->irq > 15)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY, "Invalid I/O address(0x%x) or IRQ(%d) specified", card->io, card->irq);
    return 0;
  }

  if (card->irq == 2)
    card->irq = 9;

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "Probing for a card at 0x%x with IRQ %d", card->io, card->irq);
  if (ne_detect (card)) /* No card found */
    return 0;

  log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "NE2000 card found at 0x%x using IRQ %d", card->io, card->irq);
  ne_init_chip (card, 1);

  /* Create the service. */

  if (ipc_service_create ("ethernet", &ipc_structure, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create ethernet service.");
    return -1;
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

#if 0
  /* Infinite loop */
  for (;;)
  {
    from = syscall_message_receive (ERROR, &message, 1024);
    if (from <= 0)
    {
      /* XXX - fix this after overflow bug is fixed */
//      if(!x++) log_print_formatted ("Hmm, Damnit, Got from=0x%lx", from);
      continue;
    }


    switch (message[0])
    {
      case NE_STOP_CARD:
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "Stopping card->. (pid %ld wants this..)", from);
        ne_init_chip (&card, 0);
        break;

      case NE_GET_STATUS:
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Status of NE2k card at 0x%x on irq %d:", card->io, card->irq);
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Interrupts processed: %ld", card->num_interrupts);
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Dropped packets: %ld", card->num_dropped);
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "** Status: 0x%x", card->status);
        break;


      case NE_RST_OVERRUN:
        log_print_formatted (&log_structure, LOG_URGENCY_ERROR, "-- RX overrun occured. Stop, reset, start card->.");
        ne_init_chip (&card, 0);


        ne_handle_overflow (&card);
        /* XXX - fix this */

        ne_init_chip (&card, 1);
        break;


      case IPC_ETHERNET_REGISTER_TARGET:
      {
        /* FIXME: Check if the protocol is already registered */
        
        device->target[device->number_of_targets].mailbox_id = 
          ipc_structure.output_mailbox_id;
        device->target[device->number_of_targets].protocol_type =
          host_to_network_u16 (data[0]);
        device->number_of_targets++;
        break;
      }

      case ETHERNET_PACKET_SEND:
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "** Pid %ld requested PACKET_SEND", from);
        break;
	
      case ETHERNET_ADDRESS_GET:
        memcpy (&message[1], card->ethernet_address, 6);
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "** Pid %ld requested ETHERNET_ADDRESS_GET", from);
        syscall_message_send (from, &message, 12);
	break;


      default:
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, "** Pid %ld requested UNKNOWN type 0x%lx", from, message[0]);
        message[0] = 42;
/* If from == 0 (kernel?) wee get a pf here */
//        syscall_message_send (from, &message, 4);
        break;
    }
  }  
#endif
}
#endif
