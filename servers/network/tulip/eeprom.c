/* $Id$ */
/* Abstract: EEPROM support for the tulip driver series. */
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


/* Serial EEPROM section. */
/* The main routine to parse the very complicated SROM structure.
   Search www.digital.com for "21X4 SROM" to get details.
   This code is very complex, and will require changes to support
   additional cards, so I'll be verbose about what is going on.
*/

/* Known cards that have old-style EEPROMs. */

#include "config.h"
#include "types.h"
#include "functions.h"

static eeprom_fixup eeprom_fixups[] =
{
  {
    "Asante", 0, 0, 0x94,
    {
      0x1e00, 0x0000, 0x0800, 0x0100, 0x018c,
      0x0000, 0x0000, 0xe078, 0x0001, 0x0050, 0x0018
    }
  },
  {
    "SMC9332DST", 0, 0, 0xC0, { 0x1e00, 0x0000, 0x0800, 0x041f,
                                0x0000, 0x009E, /* 10baseT */
                                0x0004, 0x009E, /* 10baseT-FD */
                                0x0903, 0x006D, /* 100baseTx */
                                0x0905, 0x006D, /* 100baseTx-FD */ 
    }
  },
  {
    "Cogent EM100", 0, 0, 0x92, 
    {
      0x1e00, 0x0000, 0x0800, 0x063f,
      0x0107, 0x8021, /* 100baseFx */
      0x0108, 0x8021, /* 100baseFx-FD */
      0x0100, 0x009E, /* 10baseT */
      0x0104, 0x009E, /* 10baseT-FD */
      0x0103, 0x006D, /* 100baseTx */
      0x0105, 0x006D, /* 100baseTx-FD */ 
    }
  },
  {
    "Maxtech NX-110", 0, 0, 0xE8, 
    {
      0x1e00, 0x0000, 0x0800, 0x0513,
      0x1001, 0x009E, /* 10base2, CSR12 0x10*/
      0x0000, 0x009E, /* 10baseT */
      0x0004, 0x009E, /* 10baseT-FD */
      0x0303, 0x006D, /* 100baseTx, CSR12 0x03 */
      0x0305, 0x006D, /* 100baseTx-FD CSR12 0x03 */
    }
  },
  {
    "Accton EN1207", 0, 0, 0xE8, 
    {
      0x1e00, 0x0000, 0x0800, 0x051F,
      0x1B01, 0x0000, /* 10base2,   CSR12 0x1B */
      0x0B00, 0x009E, /* 10baseT,   CSR12 0x0B */
      0x0B04, 0x009E, /* 10baseT-FD,CSR12 0x0B */
      0x1B03, 0x006D, /* 100baseTx, CSR12 0x1B */
      0x1B05, 0x006D, /* 100baseTx-FD CSR12 0x1B */
    }
  },
  {
    "NetWinder", 0x00, 0x10, 0x57,
    /* Default media = MII
     * MII block, reset sequence (3) = 0x0821 0x0000 0x0001, capabilities 0x01e1
     */
    {
      0x1e00, 0x0000, 0x000b, 0x8f01, 0x0103, 0x0300, 0x0821, 0x000, 0x0001, 0x0000, 0x01e1
    }
  },
  {
    0, 0, 0, 0, 
    {
    }
  }
};


static const char *block_name[] =
{
	"21140 non-MII",
	"21140 MII PHY",
	"21142 Serial PHY",
	"21142 MII PHY",
	"21143 SYM PHY",
	"21143 reset method"
};


void tulip_parse_eeprom (net_device *device)
{
  /* The last media info list parsed, for multiport boards.  */

  static mediatable *last_mediatable = NULL;
  static unsigned char *last_ee_data = NULL;
  static int controller_index = 0;
  tulip_private *tp = (tulip_private *)device->private;
  unsigned char *ee_data = tp->eeprom;
  bool subsequent_board = FALSE;
  int i;

  tp->mtable = NULL;

  /* Detect an old-style (SA only) EEPROM layout:
     memcmp(eedata, eedata+16, 8). */

  for (i = 0; i < 8; i ++)
  {
    if (ee_data[i] != ee_data[16+i])
      break;
  }

  if (i >= 8)
  {
    if (ee_data[0] == 0xff)
    {
      if (last_mediatable)
      {
        controller_index++;
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s:  Controller %d of multiport board.\n",
           device->name, controller_index);
        tp->mtable = last_mediatable;
        ee_data = last_ee_data;
        subsequent_board = TRUE;
        // goto subsequent_board
      }
      else
      {
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s:  Missing EEPROM, this interface may "
           "not work correctly!\n",
           device->name);
        return;
      }
      //      return;
    }

    /* Do a fix-up based on the vendor half of the station address
       prefix. */

    if (!subsequent_board)
    {
      for (i = 0; eeprom_fixups[i].name; i++)
      {
        if (device->address[0] == eeprom_fixups[i].address0 &&
            device->address[1] == eeprom_fixups[i].address1 &&
            device->address[2] == eeprom_fixups[i].address2)
        {
          if (device->address[2] == 0xE8 &&
              ee_data[0x1a] == 0x55)
          {
            /* An Accton EN1207, not an outlaw Maxtech. */

            i++;
          }

          memory_copy (ee_data + 26, eeprom_fixups[i].new_table,
                       sizeof (eeprom_fixups[i].new_table));
          log_print_formatted
            (&log_structure, LOG_URGENCY_INFORMATIVE,
             "%s: Old format EEPROM on '%s' board.  Using"
             " substitute media control info.\n",
             device->name, eeprom_fixups[i].name);
          break;
        }
      }
      if (eeprom_fixups[i].name == NULL)
      { /* No fixup found. */
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s: Old style EEPROM with no media selection "
           "information.\n",
           device->name);
        return;
      }
    }
  }

  if (!subsequent_board)
  {
    controller_index = 0;
    if (ee_data[19] > 1)
    {		/* Multiport board. */
      last_ee_data = ee_data;
    }
  }
  
  // subsequent_board:

  if (ee_data[27] == 0)
  {
    /* No valid media table. */
  }
  else if (tp->chip_id == DC21041)
  {
    unsigned char *p;
    int media;
    int count;

    p = &ee_data[ee_data[27 + controller_index*3]];

    media = system_get_unaligned_u16 (p);
    count = p[2];
    p += 3;

    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE,
       "%s: 21041 Media table, default media %4.4x (%s).\n",
       device->name, media,
       media & 0x0800 ?"Autosense" : medianame[media & 15]);
    for (i = 0; i < count; i++)
    {
      unsigned char media_code = *p++;
      if (media_code & 0x40)
      {
        p += 6;
      }

      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s:  21041 media #%d, %s.\n",
         device->name, media_code & 15,
         medianame[media_code & 15]);
    }
  }
  else
  {
    unsigned char *p;
    unsigned char csr12_direction = 0;
    int count, new_advertise = 0;
    mediatable *mtable;
    u16 media;

    p = &ee_data[ee_data[27]];
    media = system_get_unaligned_u16 (p);

    p += 2;
    if (tp->flags & CSR12_IN_SROM)
      csr12_direction = *p++;
    count = *p++;

    /* there is no phy information, don't even try to build mtable */
    if (count == 0)
    {
      DPRINTK("no phy info, aborting mtable build\n");
      return;
    }

    /* Fixme: Must be mapped non-cachable, or something...  (kmalloc
     * (,,GPF_KERNEL) in original)*/

    memory_allocate ((void **) &mtable,
                     sizeof (mediatable) + count*sizeof (medialeaf));

    if (mtable == NULL)
    {
      return;				/* Horrible, impossible failure. */
    }

    last_mediatable = tp->mtable = mtable;
    mtable->default_media = media;
    mtable->leaf_count = count;
    mtable->csr12_direction = csr12_direction;
    mtable->has_nonmii = mtable->has_mii = mtable->has_reset = 0;
    mtable->csr15_direction = mtable->csr15_value = 0;

    log_print_formatted
      (&log_structure, LOG_URGENCY_INFORMATIVE,
       "%s:  EEPROM default media type %s.\n", device->name,
       media & 0x0800 ? "Autosense" : medianame[media & 15]);
    for (i = 0; i < count; i++)
    {
      medialeaf *leaf = &mtable->mleaf[i];

      if ((p[0] & 0x80) == 0)
      { /* 21140 Compact block. */
        leaf->type = 0;
        leaf->media = p[0] & 0x3f;
        leaf->leaf_data = p;
        if ((p[2] & 0x61) == 0x01)	/* Bogus, but Znyx boards do it. */
          mtable->has_mii = 1;
        p += 4;
      }
      else
      {
        leaf->type = p[1];
        if (p[1] == 0x05)
        {
          mtable->has_reset = i;
          leaf->media = p[2] & 0x0f;
        }
        else if (p[1] & 1)
        {
          mtable->has_mii = 1;
          leaf->media = 11;
        }
        else
        {
          mtable->has_nonmii = 1;
          leaf->media = p[2] & 0x0f;
          switch (leaf->media)
          {
            case 0:
            {
              new_advertise |= 0x0020;
              break;
            }
            case 4:
            {
              new_advertise |= 0x0040;
              break;
            }
            case 3:
            {
              new_advertise |= 0x0080;
              break;
            }
            case 5:
            {
              new_advertise |= 0x0100;
              break;
            }
            case 6:
            {
              new_advertise |= 0x0200;
              break;
            }
          }
          if (p[1] == 2 && leaf->media == 0)
          {
            if (p[2] & 0x40)
            {
              u32 base15 = system_get_unaligned_u16 ((u16 *) &p[7]);
              mtable->csr15_direction =
                (system_get_unaligned_u16 ((u16 *) &p[9]) << 16) + base15;
              mtable->csr15_value =
                (system_get_unaligned_u16 ((u16 *) &p[11]) << 16) + base15;
            }
            else
            {
              mtable->csr15_direction =
                system_get_unaligned_u16 ((u16 *) &p[3]) << 16;
              mtable->csr15_value =
                system_get_unaligned_u16 ((u16 *) &p[5]) << 16;
            }
          }
        }
        leaf->leaf_data = p + 2;
        p += (p[0] & 0x3f) + 1;
      }
      if (tulip_debug > 1 && leaf->media == 11)
      {
        unsigned char *bp = leaf->leaf_data;
        log_print_formatted
          (&log_structure, LOG_URGENCY_INFORMATIVE,
           "%s:  MII interface PHY %d, setup/reset "
           "sequences %d/%d long, "
           "capabilities %2.2x %2.2x.\n",
           device->name, bp[0], bp[1], bp[2 + bp[1]*2],
           bp[5 + bp[2 + bp[1]*2]*2],
           bp[4 + bp[2 + bp[1]*2]*2]);
      }
      log_print_formatted
        (&log_structure, LOG_URGENCY_INFORMATIVE,
         "%s:  Index #%d - Media %s (#%d) described "
         "by a %s (%d) block.\n",
         device->name, i, medianame[leaf->media], leaf->media,
         block_name[leaf->type], leaf->type);
    }
    if (new_advertise)
    {
      tp->to_advertise = new_advertise;
    }
  }
}

/* Reading a serial EEPROM is a "bit" grungy, but we work our way
 * through:->.*/

/* Note: this routine returns extra data bits for size detection. */

int tulip_read_eeprom (int ioaddress, int location, int address_length)
{
  int i;
  unsigned retval = 0;
  long ee_address = ioaddress + CSR9;
  int read_cmd = location | (EE_READ_CMD << address_length);
  short dataval;

  system_port_out_u32 (ee_address, EE_ENB & ~EE_CS);
  system_port_out_u32 (ee_address, EE_ENB);

  /* Shift the read command bits out. */
  for (i = 4 + address_length; i >= 0; i--)
  {
    dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
    system_port_out_u32 (ee_address, EE_ENB | dataval);
    eeprom_delay ();
    system_port_out_u32 (ee_address, EE_ENB | dataval | EE_SHIFT_CLK);
    eeprom_delay ();
    retval = (retval << 1) | ((system_port_in_u32 (ee_address) & EE_DATA_READ) ? 1 : 0);
  }
  system_port_out_u32 (ee_address, EE_ENB);

  for (i = 16; i > 0; i--)
  {
    system_port_out_u32 (ee_address, EE_ENB | EE_SHIFT_CLK);
    eeprom_delay ();
    retval = (retval << 1) | ((system_port_in_u32 (ee_address) & EE_DATA_READ) ? 1 : 0);
    system_port_out_u32 (ee_address, EE_ENB);
    eeprom_delay ();
  }

  /* Terminate the EEPROM access. */
  system_port_out_u32 (ee_address, EE_ENB & ~EE_CS);

  return retval;
}

