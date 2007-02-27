/* $Id$ */
/* Abstract: 21142 support for the tulip driver series. */
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
#include "functions.h"

/*
#include "types.h"
#include "defines.h"
*/

static u16 t21142_csr13[] =
{
  0x0001, 0x0009, 0x0009, 0x0000, 0x0001
};

u16 t21142_csr14[] =
{
  0xFFFF, 0x0705, 0x0705, 0x0000, 0x7F3D
};

static u16 t21142_csr15[] =
{
  0x0008, 0x0006, 0x000E, 0x0008, 0x0008
};

/* Handle the 21143 uniquely: do autoselect with NWay, not the EEPROM
   list of available transceivers. */

void t21142_timer (unsigned long data)
{
  net_device *device = (net_device *) data;
  tulip_private *tp = (tulip_private *) device->private;
  long ioaddr = device->base_address;
  int csr12;
  int next_tick = 60*HZ;
  int new_csr6 = 0;

  csr12 = system_port_in_u32 (ioaddr + CSR12);

  if (tulip_debug > 2)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE,
       ": %s: 21143 negotiation status %8.8x, %s.\n",
       device->name, csr12, medianame[device->if_port]);
  }

  if (tulip_media_cap[device->if_port] & MediaIsMII)
  {
    tulip_check_duplex (device);
    next_tick = 60*HZ;
  }
  else if (tp->nwayset)
  {
    /* Don't screw up a negotiated session! */
    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: Using NWay-set %s media, csr12 %8.8x.\n",
         device->name, medianame[device->if_port], csr12);
    }
  }
  else if (tp->medialock)
  {
    ;
  }
  else if (device->if_port == 3)
  {
    if (csr12 & 2)
    {
      /* No 100mbps link beat, revert to 10mbps. */

      if (tulip_debug > 1)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           ": %s: No 21143 100baseTx link beat, %8.8x, "
           "trying NWay.\n", device->name, csr12);
      }

      t21142_start_nway (device);
      next_tick = 3*HZ;
    }
  }
  else if ((csr12 & 0x7000) != 0x5000)
  {
    /* Negotiation failed.  Search media types. */

    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: 21143 negotiation failed, status %8.8x.\n",
         device->name, csr12);
    }

    if (!(csr12 & 4))
    {
      /* 10mbps link beat good. */

      new_csr6 = 0x82420000;
      device->if_port = 0;
      system_port_out_u32 (ioaddr + CSR13, 0);
      system_port_out_u32 (ioaddr + CSR14, 0x0003FFFF);
      system_port_out_u16 (ioaddr + CSR15, t21142_csr15[device->if_port]);
      system_port_out_u32 (ioaddr + CSR13, t21142_csr13[device->if_port]);
    }
    else
    {
      /* Select 100mbps port to check for link beat. */
      new_csr6 = 0x83860000;
      device->if_port = 3;
      system_port_out_u32 (ioaddr + CSR13, 0);
      system_port_out_u32 (ioaddr + CSR14, 0x0003FF7F);
      system_port_out_u16 (ioaddr + CSR15, 8);
      system_port_out_u32 (ioaddr + CSR13, 1);
    }

    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: Testing new 21143 media %s.\n",
         device->name, medianame[device->if_port]);
    }

    if ((unsigned) new_csr6 != (tp->csr6 & ~0x00D5))
    {
      tp->csr6 &= 0x00D5;
      tp->csr6 |= new_csr6;
      system_port_out_u32 (ioaddr + CSR12, 0x0301);
      tulip_restart_rxtx (tp, tp->csr6);
    }
    next_tick = 3*HZ;
  }

  /* mod_timer synchronizes us with potential add_timer calls
   * from interrupts.
   */
  mod_timer (&tp->timer, RUN_AT (next_tick));
}


void t21142_start_nway (net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int csr14 = ((tp->to_advertise & 0x0780) << 9)  |
    ((tp->to_advertise&0x0020)<<1) | 0xffbf;

  DPRINTK ("ENTER\n");

  device->if_port = 0;
  tp->nway = tp->mediasense = 1;
  tp->nwayset = tp->lpar = 0;

  if (tulip_debug > 1)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: Restarting 21143 autonegotiation, "
       "csr14=%8.8x.\n",
       device->name, csr14);
  }

  system_port_out_u32 (ioaddr + CSR13, 0x0001);
  udelay (100);
  system_port_out_u32 (ioaddr + CSR14, csr14);

  if (tp->chip_id == PNIC2)
  {
    tp->csr6 = 0x01a80000 | (tp->to_advertise & 0x0040 ? 0x0200 : 0);
  }
  else
  {
    tp->csr6 = 0x82420000 | (tp->to_advertise & 0x0040 ? 0x0200 : 0);
  }

  tulip_outl_csr (tp, tp->csr6, CSR6);

  if (tp->mtable &&
      tp->mtable->csr15_direction)
  {
    system_port_out_u32 (ioaddr + CSR15, tp->mtable->csr15_direction);
    system_port_out_u32 (ioaddr + CSR15, tp->mtable->csr15_value);
  }
  else
  {
    system_port_out_u16 (ioaddr + CSR15, 0x0008);
  }

  /* Trigger NWAY. */

  system_port_out_u32 (ioaddr + CSR12, 0x1301);
}


void t21142_link_change (net_device *device, u32 csr5)
{
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int csr12;
  int setup_done;
  int negotiated;

  csr12 = system_port_in_u32 (ioaddr + CSR12);

  if (tulip_debug > 1)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE,
       ": %s: 21143 link status interrupt %8.8x, CSR5 %lx, "
       "%8.8lx.\n", device->name, csr12, csr5,  
       system_port_in_u32 (ioaddr + CSR14));
  }

  /* If NWay finished and we have a negotiated partner capability. */

  if (tp->nway &&
      !tp->nwayset &&
      (csr12 & 0x7000) == 0x5000)
  {
    setup_done = 0;
    negotiated = tp->to_advertise & (csr12 >> 16);
    tp->lpar = csr12 >> 16;
    tp->nwayset = 1;

    if (negotiated & 0x0100)
    {
      device->if_port = 5;
    }
    else if (negotiated & 0x0080)
    {
      device->if_port = 3;
    }
    else if (negotiated & 0x0040)
    {
      device->if_port = 4;
    }
    else if (negotiated & 0x0020)
    {
      device->if_port = 0;
    }
    else
    {
      tp->nwayset = 0;
      if ((csr12 & 2) == 0 &&
          (tp->to_advertise & 0x0180))
      {
        device->if_port = 3;
      }
    }
    tp->full_duplex =
      (tulip_media_cap[device->if_port] & MediaAlwaysFD) ? 1 : 0;

    if (tulip_debug > 1)
    {
      if (tp->nwayset)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: Switching to %s based on link "
           "negotiation %4.4x & %4.4x = %4.4x.\n",
           device->name, medianame[device->if_port],
           tp->to_advertise, tp->lpar, negotiated);
      }
      else
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: Autonegotiation failed, using %s,"
           " link beat status %4.4x.\n",
           device->name, medianame[device->if_port], csr12);
      }
    }

    if (tp->mtable)
    {
      int i;
      for (i = 0; i < tp->mtable->leaf_count; i++)
      {
        if (tp->mtable->mleaf[i].media == device->if_port)
        {
          tp->cur_index = i;
          tulip_select_media (device, 0);
          setup_done = 1;
          break;
        }
      }
    }
    if (!setup_done)
    {
      tp->csr6 = device->if_port & 1 ? 0x83860000 : 0x82420000;
      if (tp->full_duplex)
      {
        tp->csr6 |= 0x0200;
      }
      system_port_out_u32 (ioaddr + CSR13, 1);
    }

    /* Restart shouldn't be needed. */

#if FALSE

    tulip_outl_csr (tp, tp->csr6 | csr6_sr, CSR6);
    if (tulip_debug > 2)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_DEBUG,
         "%s:  Restarting Tx and Rx, CSR5 is %8.8x.\n",
         device->name, system_port_in_u32 (ioaddr + CSR5));
    }

#endif

    tulip_outl_csr (tp, tp->csr6 | csr6_st | csr6_sr, CSR6);
    if (tulip_debug > 2)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_DEBUG,
         "%s:  Setting CSR6 %8.8lx/%lx CSR12 %8.8lx.\n",
         device->name, tp->csr6, system_port_in_u32 (ioaddr + CSR6),
         system_port_in_u32 (ioaddr + CSR12));
    }
  }
  else if ((tp->nwayset && (csr5 & 0x08000000) &&
            (device->if_port == 3  ||  device->if_port == 5) &&
            (csr12 & 2) == 2) ||
           (tp->nway && (csr5 & (TPLinkFail))))
  {
    /* Link blew? Maybe restart NWay. */

    del_timer_sync (&tp->timer);
    t21142_start_nway (device);
    tp->timer.expires = RUN_AT (3*HZ);
    add_timer (&tp->timer);
  }
  else if (device->if_port == 3 || device->if_port == 5)
  {
    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: 21143 %s link beat %s.\n",
         device->name, medianame[device->if_port],
         (csr12 & 2) ? "failed" : "good");
    }
    if ((csr12 & 2) &&
        !tp->medialock)
    {
      del_timer_sync (&tp->timer);
      t21142_start_nway (device);
      tp->timer.expires = RUN_AT (3*HZ);
      add_timer (&tp->timer);
    }
  }
  else if (device->if_port == 0 || device->if_port == 4)
  {
    if ((csr12 & 4) == 0)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: 21143 10baseT link beat good.\n",
         device->name);
    }
  }
  else if (!(csr12 & 4))
  {
    /* 10 Mbps link beat good. */

    if (tulip_debug)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: 21143 10mbps sensed media.\n",
         device->name);
    }
    device->if_port = 0;
  }
  else if (tp->nwayset)
  {
    if (tulip_debug)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: 21143 using NWay-set %s, csr6 %8.8lx.\n",
         device->name, medianame[device->if_port], tp->csr6);
    }
  }
  else
  {
    /* 100 Mbps link beat good. */

    if (tulip_debug)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         ": %s: 21143 100baseTx sensed media.\n",
         device->name);
    }
    device->if_port = 3;
    tp->csr6 = 0x83860000;
    system_port_out_u32 (ioaddr + CSR14, 0x0003FF7F);
    system_port_out_u32 (ioaddr + CSR12, 0x0301);
    tulip_restart_rxtx (tp, tp->csr6);
  }
}


