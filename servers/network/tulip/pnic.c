/* $Id$ */
/* Abstract: PNIC (?) funktions for the tulip driver series. */
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


void pnic_do_nway(net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  u32 phy_reg = system_port_in_u32 (ioaddr + 0xB8);
  u32 new_csr6 = tp->csr6 & ~0x40C40200;

  if (phy_reg & 0x78000000)
  {
    /* Ignore baseT4 */

    if (phy_reg & 0x20000000)
    {
      device->if_port = 5;
    }
    else if (phy_reg & 0x40000000)
    {
      device->if_port = 3;
    }
    else if (phy_reg & 0x10000000)
    {
      device->if_port = 4;
    }
    else if (phy_reg & 0x08000000)
    {
      device->if_port = 0;
    }

    tp->nwayset = 1;
    new_csr6 = (device->if_port & 1) ? 0x01860000 : 0x00420000;
    system_port_out_u32 (ioaddr + CSR12, 0x32 | (device->if_port & 1));

    if (device->if_port & 1)
    {
      system_port_out_u32 (ioaddr + 0xB8, 0x1F868);
    }

    if (phy_reg & 0x30000000)
    {
      tp->full_duplex = 1;
      new_csr6 |= 0x00000200;
    }

    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_DEBUG,
         "%s: PNIC autonegotiated status %8.8lx, %s.\n",
         device->name, phy_reg, medianame[device->if_port]);
    }

    if (tp->csr6 != new_csr6)
    {
      tp->csr6 = new_csr6;

      /* Restart Tx */

      tulip_restart_rxtx(tp, tp->csr6);
      device->transmission_start = jiffies;
    }
  }
}


void pnic_link_change(net_device *device, u32 csr5)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int phy_reg = system_port_in_u32 (ioaddr + 0xB8);
  u32 tmp;

  if (tulip_debug > 1)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: PNIC link changed state %8.8x, CSR5 %8.8lx.\n",
       device->name, phy_reg, csr5);
  }

  if (system_port_in_u32 (ioaddr + CSR5) & TPLinkFail)
  {
    tmp = (system_port_in_u32 (ioaddr + CSR7) & ~TPLinkFail) | TPLinkPass;
    system_port_out_u32 (ioaddr + CSR7, tmp);

    if (! tp->nwayset  ||  jiffies - device->transmission_start > 1*HZ)
    {
      tp->csr6 = 0x00420000 | (tp->csr6 & 0x0000fdff);
      tulip_outl_csr(tp, tp->csr6, CSR6);
      system_port_out_u32 (ioaddr + CSR12, 0x30);

      /* Turn on autonegotiation. */

      system_port_out_u32 (ioaddr + 0xB8, 0x0201F078);
      device->transmission_start = jiffies;
    }
  }
  else if (system_port_in_u32 (ioaddr + CSR5) & TPLinkPass)
  {
    pnic_do_nway(device);
    tmp = (system_port_in_u32 (ioaddr + CSR7) & ~TPLinkPass) | TPLinkFail;
    system_port_out_u32 (ioaddr + CSR7, tmp);
  }
}


void pnic_timer(unsigned long data)
{
  net_device *device = (net_device *)data;
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int next_tick = 60*HZ;

  if (tulip_media_cap[device->if_port] & MediaIsMII)
  {
    if (tulip_check_duplex(device) > 0)
    {
      next_tick = 3*HZ;
    }
  }
  else
  {
    u32 csr12 = system_port_in_u32 (ioaddr + CSR12);
    u32 new_csr6 = tp->csr6 & ~0x40C40200;
    int phy_reg = system_port_in_u32 (ioaddr + 0xB8);
    u32 csr5 = system_port_in_u32 (ioaddr + CSR5);

    if (tulip_debug > 1)
    {
      log_print_formatted 
        (&log_structure, LOG_URGENCY_DEBUG,
         "%s: PNIC timer PHY status %8.8x, %s "
         "CSR5 %8.8lx.\n",
         device->name, phy_reg, medianame[device->if_port], csr5);
    }
    if (phy_reg & 0x04000000)
    {
      /* Remote link fault */

      system_port_out_u32 (ioaddr + 0xB8, 0x0201F078);
      next_tick = 1*HZ;
      tp->nwayset = 0;
    
    }
    else if (phy_reg & 0x78000000)
    {
      /* Ignore baseT4 */

      pnic_do_nway(device);
      next_tick = 60*HZ;
    
    }
    else if (csr5 & TPLinkFail)
    {
      /* 100baseTx link beat */

      if (tulip_debug > 1)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_DEBUG,
           "%s: %s link beat failed, CSR12 %4.4lx, "
           "CSR5 %8.8lx, PHY %3.3lx.\n",
           device->name, medianame[device->if_port], csr12,
           system_port_in_u32 (ioaddr + CSR5),
           system_port_in_u32 (ioaddr + 0xB8));
      }

      next_tick = 3*HZ;

      if (tp->medialock)
      {      
      }
      else if (tp->nwayset && (device->if_port & 1))
      {
        next_tick = 1*HZ;
      }
      else if (device->if_port == 0)
      {
        device->if_port = 3;
        system_port_out_u32 (ioaddr + CSR12, 0x33);
        new_csr6 = 0x01860000;
        system_port_out_u32 (ioaddr + 0xB8, 0x1F868);
      }
      else
      {
        device->if_port = 0;
        system_port_out_u32 (ioaddr + CSR12, 0x32);
        new_csr6 = 0x00420000;
        system_port_out_u32 (ioaddr + 0xB8, 0x1F078);
      }

      if (tp->csr6 != new_csr6)
      {
        tp->csr6 = new_csr6;

        /* Restart Tx */


        device->transmission_start = jiffies;
        if (tulip_debug > 1)
        {
          log_print_formatted
            (&log_structure, LOG_URGENCY_INFORMATIVE,
             "%s: Changing PNIC configuration to %s "
             "%s-duplex, CSR6 %8.8lx.\n",
             device->name, medianame[device->if_port],
             tp->full_duplex ? "full" : "half", new_csr6);
        }
      }
    }
  }

  tp->timer.expires = RUN_AT(next_tick);
  add_timer(&tp->timer);
}

