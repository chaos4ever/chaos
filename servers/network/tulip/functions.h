/* $Id$. */
/* Abstract: Functions and some globals used by the tulip driver
   series. */
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


#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "config.h"
#include "enumerations.h"
#include "types.h"
#include "defines.h"

/* Fixme: Fake functions. */

int mod_timer (timer_list *t, int b);
#define udelay(t)
#define del_timer_sync(t)
#define add_timer(t)
#define pci_write_config_dword(a, b, c);
u32 cpu_to_le32 (u32);
u16 le16_to_cpu (u16);
dma_addr_t pci_map_single(pci_dev *, u16 *, int, int);
void pci_unmap_single(pci_dev *, dma_addr_t, int, int);
#define init_timer(t)
int request_irq (int, void *, int, char *, void *);
#define netif_start_queue(x)
#define netif_stop_queue(x)
#define netif_running(x) TRUE
#define spin_lock_irqsave(a, b) {}
#define spin_unlock_irqrestore(a, b) {}
#define spin_lock_irq(a) {}
#define spin_unlock_irq(a) {}
#define device_alloc_skb(PKT_BUF_SZ) NULL
#define device_kfree_skb(PKT_BUF_SZ)
#define free_irq(a, b) 
#define capable(x) TRUE
#define set_bit(x,y)
#define pci_resource_start(a,b) 0
#define init_etherdev(a,b) 0
#define request_region(a,b,c) 0
#define request_mem_region(a,b,c) 0
#define pci_enable_device(a) 0
void pci_set_master(void *);
void pci_read_config_byte(void *a, int b, void *);
#define pci_alloc_consistent(a,b,c) NULL
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT


/* Undefine, or define to various debugging levels (>4 == obscene levels). */

#undef TULIP_DEBUG


#ifdef TULIP_DEBUG
/* Note: prints function name for you. */
#define DPRINTK(fmt, args...) printk(KERN_DEBUG "%s: " fmt, __FUNCTION__ , ## args)
#else
#define DPRINTK(fmt, args...)
#endif


/* 21142.c */

extern u16 t21142_csr14[];
void t21142_timer (unsigned long data);
void t21142_start_nway (net_device *device);
void t21142_link_change (net_device *device, u32 csr5);

/* eeprom.c */

void tulip_parse_eeprom (net_device *device);
int tulip_read_eeprom (int ioaddr, int location, int addr_len);

/* interrupt.c */

extern int tulip_max_interrupt_work;
extern int tulip_rx_copybreak;
void tulip_interrupt (int irq, void *device_instance, pt_regs *regs);

/* media.c */

int tulip_mdio_read (net_device *device, int phy_id, int location);
void tulip_mdio_write (net_device *device, int phy_id,
                       int location, int value);
void tulip_select_media (net_device *device, int startup);
int tulip_check_duplex (net_device *device);

/* pnic.c */

void pnic_do_nway (net_device *device);
void pnic_link_change (net_device *device, u32 csr5);
void pnic_timer (unsigned long data);

/* timer.c */

void tulip_timer (unsigned long data);
void mxic_timer (unsigned long data);
void comet_timer (unsigned long data);

/* tulip_core.c */

extern int tulip_debug;
extern const char *const medianame[];
extern const char tulip_media_cap[];
extern tulip_chip_table tulip_table[];
extern u8 t21040_csr13[];
extern u16 t21041_csr13[];
extern u16 t21041_csr14[];
extern u16 t21041_csr15[];
static log_structure_type log_structure;
//static ipc_structure_type pci_structure;

static inline void tulip_outl_csr (tulip_private *tp, u32 new_value,
                                   enum tulip_offsets offset)
{
  system_port_out_u32 (tp->base_address + offset, new_value);
}

static inline void tulip_stop_rxtx(tulip_private *tp, u32 csr6mask)
{
  tulip_outl_csr(tp, csr6mask & ~(csr6_st | csr6_sr), CSR6);
}

static inline void tulip_restart_rxtx(tulip_private *tp, u32 csr6mask)
{
  tulip_outl_csr(tp, csr6mask | csr6_sr, CSR6);
  tulip_outl_csr(tp, csr6mask | csr6_st | csr6_sr, CSR6);
}


#endif /* __FUNCTIONS_H__ */
