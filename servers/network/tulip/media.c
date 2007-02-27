/* $Id$ */
/* Abstract: Media access funktions for the tulip driver series. */
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
#include "defines.h"
#include "types.h"
#include "functions.h"


/* This is a mysterious value that can be written to CSR11 in the
   21040 (only) to support a pre-NWay full-duplex signaling mechanism
   using short frames.  No one knows what it should be, but if left at
   its default value some 10base2(!) packets trigger a
   full-duplex-request interrupt. */

#define FULL_DUPLEX_MAGIC	0x6969


/* MII transceiver control section.
   Read and write the MII registers using software-generated serial
   MDIO protocol.  See the MII specifications or DP83840A data sheet
   for details. */

int tulip_mdio_read (net_device *device, int phy_id, int location)
{
  tulip_private *tp = (tulip_private *)device->private;
  int read_cmd = (0xf6 << 10) | (phy_id << 5) | location;
  long ioaddr = device->base_address;
  long mdio_addr = ioaddr + CSR9;
  int return_value = 0;
  int i;

  if (tp->chip_id == LC82C168)
  {
    i = 1000;
    system_port_out_u32 (ioaddr + 0xA0,
                         0x60020000 + (phy_id << 23) + (location << 18));
    system_port_in_u32 (ioaddr + 0xA0);
    system_port_in_u32 (ioaddr + 0xA0);
    while (--i > 0)
    {
      if (!((return_value = system_port_in_u32 (ioaddr + 0xA0)) & 0x80000000))
      {
        return return_value & 0xffff;
      }
    }
    return 0xffff;
  }

  if (tp->chip_id == COMET)
  {
    if (phy_id == 1)
    {
      if (location < 7)
      {
        return system_port_in_u32 (ioaddr + 0xB4 + (location << 2));
      }
      else if (location == 17)
      {
        return system_port_in_u32 (ioaddr + 0xD0);
      }
      else if (location >= 29 && location <= 31)
      {
        return system_port_in_u32 (ioaddr + 0xD4 + ((location-29) << 2));
      }
    }
    return 0xffff;
  }

  /* Establish sync by sending at least 32 logic ones. */

  for (i = 32; i >= 0; i--)
  {
    system_port_out_u32 (mdio_addr, MDIO_ENB | MDIO_DATA_WRITE1);
    mdio_delay();
    system_port_out_u32 (mdio_addr, MDIO_ENB | MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK);
    mdio_delay();
  }

  /* Shift the read command bits out. */

  for (i = 15; i >= 0; i--)
  {
    int dataval = (read_cmd & (1 << i)) ? MDIO_DATA_WRITE1 : 0;

    system_port_out_u32 (mdio_addr, MDIO_ENB | dataval);
    mdio_delay();
    system_port_out_u32 (mdio_addr, MDIO_ENB | dataval | MDIO_SHIFT_CLK);
    mdio_delay();
  }

  /* Read the two transition, 16 data, and wire-idle bits. */

  for (i = 19; i > 0; i--)
  {
    system_port_out_u32 (mdio_addr, MDIO_ENB_IN);
    mdio_delay();
    return_value = (return_value << 1) | ((system_port_in_u32 (mdio_addr) & MDIO_DATA_READ) ? 1 : 0);
    system_port_out_u32 (mdio_addr, MDIO_ENB_IN | MDIO_SHIFT_CLK);
    mdio_delay();
  }
  return (return_value >> 1) & 0xffff;
}

void tulip_mdio_write(net_device *device, int phy_id, int location, int value)
{
  tulip_private *tp = (tulip_private *)device->private;
  int i;
  int cmd = (0x5002 << 16) | (phy_id << 23) | (location<<18) | value;
  long ioaddr = device->base_address;
  long mdio_addr = ioaddr + CSR9;

  if (tp->chip_id == LC82C168)
  {
    i = 1000;
    system_port_out_u32 (ioaddr + 0xA0, cmd);
    do
    {
      if (!(system_port_in_u32 (ioaddr + 0xA0) & 0x80000000))
      {
        break;
      }
    }
    while (--i > 0);

    return;
  }

  if (tp->chip_id == COMET)
  {
    if (phy_id != 1)
    {
      return;
    }
    if (location < 7)
    {
      system_port_out_u32 (ioaddr + 0xB4 + (location << 2), value);
    }
    else if (location == 17)
    {
      system_port_out_u32 (ioaddr + 0xD0, value);
    }
    else if (location >= 29 && location <= 31)
    {
      system_port_out_u32 (ioaddr + 0xD4 + ((location-29) << 2), value);
    }
    return;
  }

  /* Establish sync by sending 32 logic ones. */

  for (i = 32; i >= 0; i--)
  {
    system_port_out_u32 (mdio_addr, MDIO_ENB | MDIO_DATA_WRITE1);
    mdio_delay();
    system_port_out_u32 (mdio_addr, MDIO_ENB | MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK);
    mdio_delay();
  }

  /* Shift the command bits out. */

  for (i = 31; i >= 0; i--)
  {
    int dataval = (cmd & (1 << i)) ? MDIO_DATA_WRITE1 : 0;
    system_port_out_u32 (mdio_addr, MDIO_ENB | dataval);
    mdio_delay();
    system_port_out_u32 (mdio_addr, MDIO_ENB | dataval | MDIO_SHIFT_CLK);
    mdio_delay();
  }

  /* Clear out extra bits. */

  for (i = 2; i > 0; i--)
  {
    system_port_out_u32 (mdio_addr, MDIO_ENB_IN);
    mdio_delay();
    system_port_out_u32 (mdio_addr, MDIO_ENB_IN | MDIO_SHIFT_CLK);
    mdio_delay();
  }
}


/* Set up the transceiver control registers for the selected media type. */

void tulip_select_media(net_device *device, int startup)
{
  long ioaddr = device->base_address;
  tulip_private *tp = (tulip_private *)device->private;
  mediatable *mtable = tp->mtable;
  u32 new_csr6;
  int i;

  if (mtable)
  {
    medialeaf *mleaf = &mtable->mleaf[tp->cur_index];
    unsigned char *p = mleaf->leaf_data;
    switch (mleaf->type)
    {
      case 0:
      {
        /* 21140 non-MII xcvr. */

        if (tulip_debug > 1)
        {
          log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                               "%s: Using a 21140 non-MII transceiver"
                               " with control setting %2.2x.\n",
                               device->name, p[1]);
        }
        device->if_port = p[0];
        if (startup)
        {
          system_port_out_u32 (ioaddr + CSR12, mtable->csr12_direction | 0x100);
        }
        system_port_out_u32 (ioaddr + CSR12, p[1]);
        new_csr6 = 0x02000000 | ((p[2] & 0x71) << 18);
        break;
      }

      case 2:
      case 4:
      {
        u16 setup[5];
        u32 csr13_value, csr14_value, csr15_direction, csr15_value;
        for (i = 0; i < 5; i++)
          setup[i] = system_get_unaligned_u16(&p[i*2 + 1]);

        device->if_port = p[0] & 15;
        if (tulip_media_cap[device->if_port] & MediaAlwaysFD)
        {
          tp->full_duplex = 1;
        }

        if (startup && mtable->has_reset)
        {
          medialeaf *rleaf = &mtable->mleaf[mtable->has_reset];
          unsigned char *rst = rleaf->leaf_data;
          if (tulip_debug > 1)
          {
            log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                                 "%s: Resetting the transceiver.\n",
                                 device->name);
          }
          for (i = 0; i < rst[0]; i++)
          {
            system_port_out_u32 (ioaddr + CSR15,
                                 system_get_unaligned_u16(rst + 1 + (i << 1)) << 16);
          }
        }
        if (tulip_debug > 1)
        {
          log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                               "%s: 21143 non-MII %s transceiver control "
                               "%4.4x/%4.4x.\n",
                               device->name, medianame[device->if_port],
                               setup[0], setup[1]);
        }
        if (p[0] & 0x40)
        {
          /* SIA (CSR13-15) setup values are provided. */

          csr13_value = setup[0];
          csr14_value = setup[1];
          csr15_direction = (setup[3] << 16) | setup[2];
          csr15_value = (setup[4] << 16) | setup[2];

          system_port_out_u32 (ioaddr + CSR13, 0);

          system_port_out_u32 (ioaddr + CSR14, csr14_value);

          system_port_out_u32 (ioaddr + CSR15, csr15_direction);

          system_port_out_u32 (ioaddr + CSR15, csr15_value);

          system_port_out_u32 (ioaddr + CSR13, csr13_value);
        }
        else
        {
          csr13_value = 1;
          csr14_value = 0x0003FF7F;
          csr15_direction = (setup[0] << 16) | 0x0008;
          csr15_value = (setup[1] << 16) | 0x0008;
          if (device->if_port <= 4)
          {
            csr14_value = t21142_csr14[device->if_port];
          }
          if (startup)
          {
            system_port_out_u32 (ioaddr + CSR13, 0);
            system_port_out_u32 (ioaddr + CSR14, csr14_value);
          }
          system_port_out_u32 (ioaddr + CSR15, csr15_direction);
          system_port_out_u32 (ioaddr + CSR15, csr15_value);

          if (startup)
          {
            system_port_out_u32 (ioaddr + CSR13, csr13_value);
          }
        }
        if (tulip_debug > 1)
        {
          log_print_formatted
            (&log_structure, LOG_URGENCY_INFORMATIVE,
             "%s:  Setting CSR15 to %8.8lx/%8.8lx.\n",
             device->name, csr15_direction, csr15_value);
        }
        if (mleaf->type == 4)
        {
          new_csr6 = 0x82020000 | ((setup[2] & 0x71) << 18);
        }
        else
        {
          new_csr6 = 0x82420000;
        }
        break;
      }

      case 1:
      case 3:
      {
        int phy_num = p[0];
        int init_length = p[1];
        u16 *misc_info;
        u16 to_advertise;

        device->if_port = 11;
        new_csr6 = 0x020E0000;
        if (mleaf->type == 3)
        {
          /* 21142 */

          u16 *init_sequence = (u16*)(p+2);
          u16 *reset_sequence = &((u16*)(p+3))[init_length];
          int reset_length = p[2 + init_length*2];
          misc_info = reset_sequence + reset_length;
          if (startup)
          {
            for (i = 0; i < reset_length; i++)
            {
              system_port_out_u32 (ioaddr + CSR15, 
                                   system_get_unaligned_u16(&reset_sequence[i]) << 16);
            }
          }
          for (i = 0; i < init_length; i++)
          {
            system_port_out_u32 (ioaddr + CSR15, 
                                 system_get_unaligned_u16(&init_sequence[i]) << 16);
          }
        }
        else
        {
          u8 *init_sequence = p + 2;
          u8 *reset_sequence = p + 3 + init_length;
          int reset_length = p[2 + init_length];
          misc_info = (u16*)(reset_sequence + reset_length);
          if (startup)
          {
            system_port_out_u32 (ioaddr + CSR12, mtable->csr12_direction | 0x100);
            for (i = 0; i < reset_length; i++)
            {
              system_port_out_u32 (ioaddr + CSR12, reset_sequence[i]);
            }
          }
          for (i = 0; i < init_length; i++)
          {
            system_port_out_u32 (ioaddr + CSR12, init_sequence[i]);
          }
        }
        to_advertise = (system_get_unaligned_u16(&misc_info[1]) & tp->to_advertise) | 1;
        tp->advertising[phy_num] = to_advertise;
        if (tulip_debug > 1)
        {
          log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                               "%s:  Advertising %4.4x on PHY %d (%d).\n",
                               device->name, to_advertise, phy_num,
                               tp->phys[phy_num]);
        }

        /* Bogus: put in by a committee?  */

        tulip_mdio_write(device, tp->phys[phy_num], 4, to_advertise);
        break;
      }
      default:
      {
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "%s:  Invalid media table selection %d.\n",
                             device->name, mleaf->type);
        new_csr6 = 0x020E0000;
        break;
      }
    }
    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s: Using media type %s, CSR12 is %2.2lx.\n",
         device->name, medianame[device->if_port],
         system_port_in_u32 (ioaddr + CSR12) & 0xff);
    }
  }
  else if (tp->chip_id == DC21041)
  {
    int port = device->if_port <= 4 ? device->if_port : 0;
    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s: 21041 using media %s, CSR12 is %4.4lx.\n",
         device->name, medianame[port == 3 ? 12: port],
         system_port_in_u32 (ioaddr + CSR12));
    }

    /* Reset the serial interface */

    system_port_out_u32 (ioaddr + CSR13, 0x00000000);
    system_port_out_u32 (ioaddr + CSR14, t21041_csr14[port]);
    system_port_out_u32 (ioaddr + CSR15, t21041_csr15[port]);
    system_port_out_u32 (ioaddr + CSR13, t21041_csr13[port]);
    new_csr6 = 0x80020000;
  }
  else if (tp->chip_id == LC82C168)
  {
    if (startup && !tp->medialock)
    {
      device->if_port = tp->mii_cnt ? 11 : 0;
    }
    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s: PNIC PHY status is %3.3lx, media %s.\n",
         device->name, system_port_in_u32 (ioaddr + 0xB8),
         medianame[device->if_port]);
    }
    if (tp->mii_cnt)
    {
      new_csr6 = 0x810C0000;
      system_port_out_u32 (ioaddr + CSR15, 0x0001);
      system_port_out_u32 (ioaddr + 0xB8, 0x0201B07A);
    }
    else if (startup)
    {
      /* Start with 10mbps to do autonegotiation. */

      system_port_out_u32 (ioaddr + CSR12, 0x32);
      new_csr6 = 0x00420000;
      system_port_out_u32 (ioaddr + 0xB8, 0x0001B078);
      system_port_out_u32 (ioaddr + 0xB8, 0x0201B078);
    }
    else if (device->if_port == 3  ||  device->if_port == 5)
    {
      system_port_out_u32 (ioaddr + CSR12, 0x33);
      new_csr6 = 0x01860000;

      /* Trigger autonegotiation. */

      system_port_out_u32 (ioaddr + 0xB8, startup ? 0x0201F868 : 0x0001F868);
    }
    else
    {
      system_port_out_u32 (ioaddr + CSR12, 0x32);
      new_csr6 = 0x00420000;
      system_port_out_u32 (ioaddr + 0xB8, 0x1F078);
    }
  }
  else if (tp->chip_id == DC21040)
  {
    /* 21040 */

    /* Turn on the xcvr interface. */

    int csr12 = system_port_in_u32 (ioaddr + CSR12);
    if (tulip_debug > 1)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%s: 21040 media type is %s, CSR12 is %2.2x.\n",
                           device->name, medianame[device->if_port], csr12);
    }
    if (tulip_media_cap[device->if_port] & MediaAlwaysFD)
    {
      tp->full_duplex = 1;
    }
    new_csr6 = 0x20000;

    /* Set the full duplux match frame. */

    system_port_out_u32 (ioaddr + CSR11, FULL_DUPLEX_MAGIC);

    /* Reset the serial interface */

    system_port_out_u32 (ioaddr + CSR13, 0x00000000);

    if (t21040_csr13[device->if_port] & 8)
    {
      system_port_out_u32 (ioaddr + CSR14, 0x0705);
      system_port_out_u32 (ioaddr + CSR15, 0x0006);
    }
    else
    {
      system_port_out_u32 (ioaddr + CSR14, 0xffff);
      system_port_out_u32 (ioaddr + CSR15, 0x0000);
    }
    system_port_out_u32 (ioaddr + CSR13, 0x8f01 | t21040_csr13[device->if_port]);
  }
  else
  {
    /* Unknown chip type with no media table. */

    if (tp->default_port == 0)
    {
      device->if_port = tp->mii_cnt ? 11 : 3;
    }
    if (tulip_media_cap[device->if_port] & MediaIsMII)
    {
      new_csr6 = 0x020E0000;
    }
    else if (tulip_media_cap[device->if_port] & MediaIsFx)
    {
      new_csr6 = 0x028600000;
    }
    else
    {
      new_csr6 = 0x038600000;
    }
    if (tulip_debug > 1)
    {
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s: No media description table, assuming "
         "%s transceiver, CSR12 %2.2lx.\n",
         device->name, medianame[device->if_port],
         system_port_in_u32 (ioaddr + CSR12));
    }
  }

  tp->csr6 = new_csr6 | (tp->csr6 & 0xfdff) | (tp->full_duplex ? 0x0200 : 0);
  return;
}

/*
  Check the MII negotiated duplex, and change the CSR6 setting if
  required.
  Return 0 if everything is OK.
  Return < 0 if the transceiver is missing or has no link beat.
  */

int tulip_check_duplex(net_device *device)
{
  tulip_private *tp = (tulip_private *)device->private;
  int mii_reg1, mii_reg5, negotiated, duplex;

  if (tp->full_duplex_lock)
  {
    return 0;
  }
  mii_reg1 = tulip_mdio_read(device, tp->phys[0], 1);
  mii_reg5 = tulip_mdio_read(device, tp->phys[0], 5);
  if (tulip_debug > 1)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                         "%s: MII status %4.4x, Link partner report "
                         "%4.4x.\n", device->name, mii_reg1, mii_reg5);
  }
  if (mii_reg1 == 0xffff)
  {
    return -2;
  }
  if ((mii_reg1 & 0x0004) == 0)
  {
    int new_reg1 = tulip_mdio_read(device, tp->phys[0], 1);
    if ((new_reg1 & 0x0004) == 0)
    {
      if (tulip_debug  > 1)
      {
    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                         "%s: No link beat on the MII interface,"
                         " status %4.4x.\n", device->name, new_reg1);
      }
      return -1;
    }
  }
  negotiated = mii_reg5 & tp->advertising[0];
  duplex = ((negotiated & 0x0300) == 0x0100
            || (negotiated & 0x00C0) == 0x0040);

  /* 100baseTx-FD  or  10T-FD, but not 100-HD */

  if (tp->full_duplex != duplex)
  {
    tp->full_duplex = duplex;
    if (negotiated & 0x038)
    {
      /* 100mbps. */

      tp->csr6 &= ~0x00400000;
    }

    if (tp->full_duplex)
    {
      tp->csr6 |= 0x0200;
    }
    else
    {
      tp->csr6 &= ~0x0200;
    }
    tulip_restart_rxtx(tp, tp->csr6);
    if (tulip_debug > 0)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                           "%s: Setting %s-duplex based on MII"
                           "#%d link partner capability of %4.4x.\n",
                           device->name, tp->full_duplex ? "full" : "half",
                           tp->phys[0], mii_reg5);
    }
    return 1;
  }
  return 0;
}

