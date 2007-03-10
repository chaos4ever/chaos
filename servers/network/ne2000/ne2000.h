/* Abstract: NE2000 header file. */
/* Author: noah williamsson <tm@ns2.crw.se> */

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

const int debug = 1;

/* 0: no debug at all
   1: normal debug
   2: debug everything */

/* FIXME: the target stuff should be replaced by a linked list */

#define MAX_NUMBER_OF_TARGETS 10

typedef struct
{
  u16 protocol_type;
  mailbox_id_type mailbox_id;
} target_type;

typedef struct
{
  u8 irq;
  u8 status;
  u8 prom[32];
  u8 mac[6];
  u16 io;
  u32 num_interrupts;
  u32 num_dropped;

  target_type target[MAX_NUMBER_OF_TARGETS];
  int number_of_targets;
} device_type;

/* Packet header. */

struct ne_pkt_hdr 
{
  u8 status;
  u8 next;
  u16 len;
};

/* Donald Becker's setup sequence, ripped from linux/drivers/net/ne.c */

struct ne_program_struct 
{
  unsigned char value;
  unsigned char offset;
};

typedef struct ne_program_struct ne_program;

struct ne_user_t 
{
  process_id_type pid;
  u16 proto;
};

/* Ripped stuff from linux's .h files */

struct ethhdr
{
  u8 h_dest[6];       /* destination eth addr */
  u8 h_source[6];     /* source ether addr    */
  u16 h_proto;                /* packet type ID field */
};
  
struct iphdr {
        u8    version:4,
              ihl:4;
        u8    tos;
        u16   tot_len;
        u16   id;
        u16   frag_off;
        u8    ttl;
        u8    protocol;
        u16   check;
	u32   saddr;
        u32   daddr;
};


struct tcphdr {
        u16   source;
        u16   dest;
        u32   seq;
        u32   ack_seq;   
        u16   res1:4,
              doff:4,
              fin:1,
              syn:1,
              rst:1,
              psh:1,
              ack:1,
              urg:1,
              res2:2;
        u16   window;
        u16   check;
        u16   urg_ptr;
};




#define CAN_WAIT_10_MS /* Can we wait 10ms in our interrupt handler? */
#define MAX_INT_WORK 5 /* Maximum packets to handle at each intr. */
#define MAX_USERS 10 /* Maximum users of the card */

/* To avoid rewriting a lot of code... */
#define outb(a,b) system_port_out_u8 (b, a)
#define inb(a) system_port_in_u8 (a)

/* FIXME: this should be placed in the ipv4 library or similar. */
#define host_to_network_u16(a) system_byte_swap_u16(a)


/* Message stuff */
#define NE_START_CARD	0x02
#define NE_STOP_CARD	0x04
#define NE_GET_STATUS	0x08
#define NE_RST_OVERRUN	0x10


/* For the status byte in NIC { } */
#define NIC_DOWN	0x01	/* Interface is down */
#define NIC_UP          0x02    /* Interface is up */
#define NIC_INT         0x04    /* We're processing an interrupt */
#define NIC_DMA         0x08    /* We're doing dma xfers */
#define NIC_BUSY	0x10	/* We're resetting the nic? */
#define NIC_INIT	0x20	/* We're initializing the card (debug) */


/* Pages for 16bits cards */
#define NUM_TX_PAGES	0xc	/* Numbers of TX pages */	
#define NE_PG_START	0x40
#define NE_PG_STOP	0x80
#define NE_PG_TX_START	NE_PG_START
#define NE_PG_RX_START	NE_PG_TX_START+NUM_TX_PAGES
#define NE_PG_RNGBUF	0x01	/* Start of recv. ring buffer */
#define NE_PG_CURRENT	0x07	/* Current page */


/* This is commands, written at <io>+NE_R0_CMD */
#define NE_PAGE0	0x00	/* First page */
#define NE_STOP		0x01	/* Stop command */
#define NE_START	0x02	/* Start command */
#define NE_TRANS	0x04	/* Transmit a frame */
#define NE_RREAD	0x08	/* Remote read */
#define NE_RWRITE	0x10	/* Remote write */
#define NE_RESET	0x1f	/* Reset command */
#define NE_NODMA	0x20	/* Remote DMA */
#define NE_PAGE1	0x40	/* Second page */
#define NE_PAGE2	0x80	/* Third page */


/* Default configurations */
#define NE_DEF_TXCFG	0x00	/* This is normal */
#define NE_DEF_RXCFG	0x4	/* XXX - Fix this. Should be 4
                                   Broadcast, promisc, errors
                                   no multicast */


/* Register offsets (R0 == reg in page0) */
#define NE_R0_CMD	0x00	/* Command register */
#define NE_R0_PSTA	0x01	/* Page start register [write] */
#define NE_R0_PSTO	0x02	/* Page stop register [write] */
#define NE_R0_BNRY	0x03	/* Boundary pointer [read/write] */
#define NE_R0_TSR	0x04	/* Transmit status register [read] */
#define NE_R0_TPSR	0x04	/* Transmit page start address [write] */
#define NE_R0_NCR	0x05	/* Number of collision register [read] */
#define NE_R0_TBCR0	0x06	/* Transmit byte count register 0 [write] */
#define NE_R0_TBCR1	0x07	/* Transmit byte count register 1 [write] */
#define NE_R0_FIFO	0x06	/* Fifo [read] */
#define NE_R0_ISR	0x07	/* Interrupt status register [read/write] */
#define NE_R0_CRDA0	0x08	/* Current remote DMA address register 0 [read] */
#define NE_R0_RSAR0	0x08	/* Remote start address register 0 [write] */
#define NE_R0_CRDA1	0x09	/* Current remote DMA address register 1 [read] */
#define NE_R0_RSAR1	0x09	/* Remote start address register 1 [write] */
#define NE_R0_RBCR0	0x0a	/* Remote byte count register 0 [write] */
#define NE_R0_RBCR1	0x0b	/* Remote byte count register 1 [write] */
#define NE_R0_RSR	0x0c	/* Receive status register [read] */
#define NE_R0_RCR	0x0c	/* Receive configuration register [write] */
#define NE_R0_CNTR0	0x0d	/* Tally counter 0 / Rcv alignment counter [read] */
#define NE_R0_TCR	0x0d	/* Transmit configuration register [write] */
#define NE_R0_CNTR1	0x0e	/* Tally counter 1 / CRC errors [read] */
#define NE_R0_DCR	0x0e	/* Data configuration register [write] */
#define NE_R0_CNTR2	0x0f	/* Tally counter 2 / Missed packets [read] */
#define NE_R0_IMR	0x0f	/* Interrupt mask register [read/write] */
#define NE_DATAPORT	0x10	/* We read cool stuff here */


/* Bits in the receive configuration register */
#define BIT_RCR_SEP	0x01	/* Save errored packets */
#define BIT_RCR_AR	0x02	/* Accept runt (size < 64) packets */
#define BIT_RCR_AB	0x04	/* Accept broadcast */
#define BIT_RCR_AM	0x08	/* Accept multicast */
#define BIT_RCR_PRO	0x10	/* Set promisc mode $!@# */
#define BIT_RCR_MON	0x20	/* Set monitor mode */


/* Bits in the transmit configuration register */
#define BIT_TCR_LB	0x02	/* Internal loopback (mode 1) */


/* Bits in interrupt status register */
#define BIT_ISR_RX	0x01	/* Rx, no error */
#define BIT_ISR_TX	0x02	/* Tx, no error */
#define BIT_ISR_RX_ERR	0x04	/* Rx, error */
#define BIT_ISR_TX_ERR	0x08	/* Tx, error */
#define BIT_ISR_OVERFLW	0x10	/* Overflow in buffer ring */
#define BIT_ISR_CNTRS	0x20	/* Counters need clearing */
#define BIT_ISR_ALL	0x3f	/* We write this to IMR to enbl all intrs. */
#define BIT_ISR_RDC	0x40	/* Remote dma complete */
#define BIT_ISR_RESET	0x80	/* Reset completed */


/* Bits in received packet status register*/
#define BIT_RSR_RXOK	0x01	/* Received a good packet */
#define BIT_RSR_CRC	0x02	/* CRC error */
#define BIT_RSR_FAE	0x04	/* Frame alignment error */
#define BIT_RSR_FO	0x08	/* Fifo overrun */
#define BIT_RSR_MP	0x10	/* Missed packet */
#define BIT_RSR_PHY	0x20	/* Phys/Multicast addres */
#define BIT_RSR_DIS	0x40	/* Disable rx, go into monitor mode */
#define BIT_RSR_DEF	0x80	/* Deferring */


/* Bits in transmitted status register */
#define BIT_TSR_PTX	0x01	/* Packet transmitted */
#define BIT_TSR_ND	0x02	/* Not deferred */
#define BIT_TSR_COL	0x04	/* Collision */
#define BIT_TSR_CRS	0x10	/* Carrier sence lost */
#define BIT_TSR_FU	0x20	/* FIFO underrun occured */


/* ... */
#define NE_BOUNDARY	0x03	/* Boundary page of ring buffer */
#define NE_CURPG	0x07	/* Current mem page */


ne_program ne_preinit_program[] = {
  { NE_NODMA + NE_PAGE0 + NE_STOP, NE_R0_CMD },  /* Select page 0 */
  { 0x48, NE_R0_DCR },			/* Set byte-wide (0x48) access. */
  { 0x00, NE_R0_RBCR0 },		/* Clear the count regs. */
  { 0x00, NE_R0_RBCR1 },
  { 0x00, NE_R0_IMR },			/* Mask completion irq. */
  { 0xFF, NE_R0_ISR },			/* Ack all interrupts */
  { 0x20, NE_R0_RCR },			/* 0x20 == Set he monitor bit */
  { 0x02, NE_R0_TCR },			/* 0x02 == Internal loopback */
  { 0x20, NE_R0_RBCR0 },		/* We should DMA 32 bytes */
  { 0x00, NE_R0_RBCR1 },
  { 0x00, NE_R0_RSAR0 },		/* DMA starting at 0x0000. */
  { 0x00, NE_R0_RSAR1 },
  { NE_RREAD + NE_START, NE_R0_CMD }	/* Guess what :-) */
};


ne_program ne_init_program[] = {
  { NE_NODMA + NE_PAGE0 + NE_STOP, NE_R0_CMD },  /* Select page 0 */
  { 0x49, NE_R0_DCR },			/* Set word-wide access */
  { 0x00, NE_R0_RBCR0 }, 		/* Clear remote byte counter */
  { 0x00, NE_R0_RBCR1 },
  { BIT_RCR_MON, NE_R0_RCR },		/* Go into monitor mode */
  { BIT_TCR_LB, NE_R0_TCR }, 		/* Internal loopback */
  { NE_PG_TX_START, NE_R0_TPSR },	/* Setup first transmit page (0x40) */
  { NE_PG_RX_START, NE_R0_PSTA },	/* Receive buffer ring page at tx page+12 = 0x4c */
  { NE_PG_STOP-1, NE_R0_BNRY },		/* Init. boundary pointer */
  { NE_PG_STOP, NE_R0_PSTO },		/* Set stop page */
  { 0xff, NE_R0_ISR },			/* Clear all interrupts */
  { 0x00, NE_R0_IMR }			/* .. and mask */
};


/*
 * About pages:
 * Start page is at 0x40 on 16bit cards
 * Stop page is at 0x80 on 16bit cards 
 *
 * Tx start page is at 0x40
 * Rx start page is at 0x40+NUM_TX_PAGES = 0x4c
 *
 * Starting page of ring buffer is at 0x01
 * 
 *
 */
