/* $Id$ */
/* Abstract: Timer funktions for the tulip driver series. */
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


void tulip_timer(unsigned long data)
{
  net_device *device = (net_device *)data;
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  u32 csr12 = system_port_in_u32 (ioaddr + CSR12);
  int next_tick = 2*HZ;

  if (tulip_debug > 2)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: Media selection tick, %s, status %8.8lx mode"
       " %8.8lx SIA %8.8lx %8.8lx %8.8lx %8.8lx.\n",
       device->name, medianame[device->if_port],
       system_port_in_u32 (ioaddr + CSR5), system_port_in_u32 (ioaddr + CSR6),
       csr12, system_port_in_u32 (ioaddr + CSR13),
       system_port_in_u32 (ioaddr + CSR14), system_port_in_u32 (ioaddr + CSR15));
  }
  switch (tp->chip_id)
  {
    case DC21040:
    {
      if (!tp->medialock  &&  csr12 & 0x0002)
      {
        /* Network error */

        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: No link beat found.\n",
           device->name);
        device->if_port = (device->if_port == 2 ? 0 : 2);
        tulip_select_media(device, 0);
        device->transmission_start = jiffies;
      }
      break;
    }
    case DC21041:
    {
      if (tulip_debug > 2)
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_DEBUG,
           "%s: 21041 media tick  CSR12 %8.8lx.\n",
           device->name, csr12);
      }

      if (tp->medialock)
      {
        break;
      }
      
      switch (device->if_port)
      {
        case 0:
        case 3: 
        case 4:
        {
          if (csr12 & 0x0004)
          {
            /*LinkFail */

            /* 10baseT is dead.  Check for activity on alternate port. */

            tp->mediasense = 1;
            if (csr12 & 0x0200)
            {
              device->if_port = 2;
            }
            else
            {
              device->if_port = 1;
            }
            log_print_formatted
              (&log_structure, LOG_URGENCY_INFORMATIVE,
               "%s: No 21041 10baseT link beat, Media switched to %s.\n",
               device->name, medianame[device->if_port]);

            /* Reset */

            system_port_out_u32 (ioaddr + CSR13, 0);

            system_port_out_u32 (ioaddr + CSR14,
                                 t21041_csr14[device->if_port]);

            system_port_out_u32 (ioaddr + CSR15,
                                 t21041_csr15[device->if_port]);

            system_port_out_u32 (ioaddr + CSR13,
                                 t21041_csr13[device->if_port]);

            /* 2.4 sec. */

            next_tick = 10*HZ;
          }
          else
          {
            next_tick = 30*HZ;
          }
        }
        break;

        /* 10base2 */

        case 1:
          
        /* AUI */

        case 2:
        {
          if (csr12 & 0x0100)
          {
            next_tick = (30*HZ);			/* 30 sec. */
            tp->mediasense = 0;
          }
          else if ((csr12 & 0x0004) == 0)
          {
            log_print_formatted
              (&log_structure, LOG_URGENCY_INFORMATIVE,
               "%s: 21041 media switched to 10baseT.\n",
               device->name);
            device->if_port = 0;
            tulip_select_media(device, 0);
            next_tick = (24*HZ)/10;				/* 2.4 sec. */
          }
          else if (tp->mediasense || (csr12 & 0x0002))
          {
            device->if_port = 3 - device->if_port; /* Swap ports. */
            tulip_select_media(device, 0);
            next_tick = 20*HZ;
          }
          else
          {
            next_tick = 20*HZ;
          }
          break;
        }
      }
      break;
    }

    case DC21140:
    case DC21142:
    case MX98713:
    case COMPEX9881:
    default:
    {
      medialeaf *mleaf;
      unsigned char *p;

      if (tp->mtable == NULL)
      {
	/* No EEPROM info, use generic code. */
        /* Not much that can be done.
           Assume this a generic MII or SYM transceiver. */

        next_tick = 60*HZ;
        if (tulip_debug > 2)
        {
          log_print_formatted
            (&log_structure, LOG_URGENCY_DEBUG,
             "%s: network media monitor CSR6 %8.8lx "
             "CSR12 0x%2.2lx.\n",
             device->name, system_port_in_u32 (ioaddr + CSR6),
             csr12 & 0xff);
        }
        break;
      }
      mleaf = &tp->mtable->mleaf[tp->cur_index];
      p = mleaf->leaf_data;
      switch (mleaf->type)
      {
        case 0:
        case 4:
        {
          /* Type 0 serial or 4 SYM transceiver.  Check the link beat bit. */
          int offset = mleaf->type == 4 ? 5 : 2;
          s8 bitnum = p[offset];
          if (p[offset+1] & 0x80)
          {
            if (tulip_debug > 1)
            {
              log_print_formatted
                (&log_structure, LOG_URGENCY_DEBUG,
                 "%s: Transceiver monitor tick "
                 "CSR12=%#2.2lx, no media sense.\n",
                 device->name, csr12);
            }
            if (mleaf->type == 4)
            {
              if (mleaf->media == 3 && (csr12 & 0x02))
              {
                goto select_next_media;
              }
            }
            break;
          }

          if (tulip_debug > 2)
          {
            log_print_formatted
              (&log_structure, LOG_URGENCY_DEBUG,
               "%s: Transceiver monitor tick: CSR12=%#2.2lx"
               " bit %d is %d, expecting %d.\n",
               device->name, csr12, (bitnum >> 1) & 7,
               (csr12 & (1 << ((bitnum >> 1) & 7))) != 0, (bitnum >= 0));
          }

          /* Check that the specified bit has the proper value. */

          if ((bitnum < 0) !=
              ((csr12 & (1 << ((bitnum >> 1) & 7))) != 0))
          {
            if (tulip_debug > 1)
            {
              log_print_formatted
                (&log_structure, LOG_URGENCY_DEBUG,
                 "%s: Link beat detected for %s.\n", device->name,
                 medianame[mleaf->media]);
            }
            if ((p[2] & 0x61) == 0x01)
            {
              /* Bogus Znyx board. */

              goto actually_mii;
            }
            break;
          }
          if (tp->medialock)
          {
            break;
          }
        select_next_media:
          if (--tp->cur_index < 0)
          {
            /* We start again, but should instead look for default. */

            tp->cur_index = tp->mtable->leaf_count - 1;
          }
          device->if_port = tp->mtable->mleaf[tp->cur_index].media;
          if (tulip_media_cap[device->if_port] & MediaIsFD)
          {
            goto select_next_media; /* Skip FD entries. */
          }
          if (tulip_debug > 1)
          {
            log_print_formatted
              (&log_structure, LOG_URGENCY_DEBUG,
               "%s: No link beat on media %s,"
               " trying transceiver type %s.\n",
               device->name, medianame[mleaf->media & 15],
               medianame[tp->mtable->mleaf[tp->cur_index].media]);
          }
          tulip_select_media(device, 0);

          /* Restart the transmit process. */

          tulip_restart_rxtx(tp, tp->csr6);
          next_tick = (24*HZ)/10;
          break;
        }

        /* 21140, 21142 MII */

        case 1:
        case 3:
        {
        actually_mii:
          tulip_check_duplex(device);
          next_tick = 60*HZ;
          break;
        }

        /* 21142 serial block has no link beat. */

        case 2:
        default:
        {
          break;
        }
      }
    }
    break;
  }
  /* mod_timer synchronizes us with potential add_timer calls
   * from interrupts.
   */
  mod_timer(&tp->timer, RUN_AT(next_tick));
}


void mxic_timer(unsigned long data)
{
  net_device *device = (net_device *)data;
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int next_tick = 60*HZ;

  if (tulip_debug > 3)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE,
       "%s: MXIC negotiation status %8.8lx.\n", device->name,
       system_port_in_u32 (ioaddr + CSR12));
  }
  if (next_tick)
  {
    mod_timer(&tp->timer, RUN_AT(next_tick));
  }
}


void comet_timer(unsigned long data)
{
  net_device *device = (net_device *)data;
  tulip_private *tp = (tulip_private *)device->private;
  long ioaddr = device->base_address;
  int next_tick = 60*HZ;

  if (tulip_debug > 1)
  {
    log_print_formatted
      (&log_structure, LOG_URGENCY_DEBUG,
       "%s: Comet link status %4.4lx partner capability "
       "%4.4lx.\n", device->name,
       system_port_in_u32 (ioaddr + 0xB8),
       system_port_in_u32 (ioaddr + 0xC8));
  }

  /* mod_timer synchronizes us with potential add_timer calls
     from interrupts. */

  mod_timer(&tp->timer, RUN_AT(next_tick));
}

