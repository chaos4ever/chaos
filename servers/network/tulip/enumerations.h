/* $Id$ */
/* Abstract: Enumerations for the tulip driver series. */
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

#ifndef __ENUMERATIONS_H__
#define __ENUMERATIONS_H__


enum tbl_flag
{
  HAS_MII = 0x01,
  HAS_MEDIA_TABLE = 0x02,
  CSR12_IN_SROM = 0x04,
  ALWAYS_CHECK_MII = 0x08,
  HAS_ACPI = 0x10,

  /* Hash-only multicast filter. */

  MULTICAST_HASH_ONLY = 0x20,

  /* Uses internal NWay xcvr. */

  HAS_NWAY = 0x40,

  HAS_PNICNWAY = 0x80,
  HAS_INTR_MITIGATION = 0x100,
  IS_ASIX = 0x200,
  HAS_8023X = 0x400,
};


/* Chip types. Careful! Order is VERY IMPORTANT here, as these are
   used throughout the driver as indices into arrays. */

/* Note: 21142 == 21143. */

enum chips
{
  DC21040 = 0,
  DC21041 = 1,
  DC21140 = 2,
  DC21142 = 3, DC21143 = 3,
  LC82C168,
  MX98713,
  MX98715,
  MX98725,
  AX88140,
  PNIC2,
  COMET,
  COMPEX9881,
  I21145,
};


enum MediaIs
{
  MediaIsFD = 1,
  MediaAlwaysFD = 2,
  MediaIsMII = 4,
  MediaIsFx = 8,
  MediaIs100 = 16
};


/* Offsets to the Command and Status Registers, "CSRs".  All accesses
   must be longword instructions and quadword aligned. */

enum tulip_offsets
{
  CSR0 = 0x00,
  CSR1 = 0x08,
  CSR2 = 0x10,
  CSR3 = 0x18,
  CSR4 = 0x20,
  CSR5 = 0x28,
  CSR6 = 0x30,
  CSR7 = 0x38,
  CSR8 = 0x40,
  CSR9 = 0x48,
  CSR10 = 0x50,
  CSR11 = 0x58,
  CSR12 = 0x60,
  CSR13 = 0x68,
  CSR14 = 0x70,
  CSR15 = 0x78
};


/* The bits in the CSR5 status registers, mostly interrupt sources. */

enum status_bits
{
  TxInterrupt = 0x0001,
  TxDied = 0x0002,
  TxNoBuffer = 0x0004,
  TxJabber = 0x0008,
  TPLinkPass = 0x0010,
  TxFIFOUnderflow = 0x0020,
  RxInterrupt = 0x0040,
  RxNoBuffer = 0x0080,
  RxDied = 0x0100,
  RxJabber = 0x0200,
  TimerInterrupt = 0x0800,
  TPLinkFail = 0x1000,
  SytemError = 0x2000,
  AbnormalIntr = 0x8000,
  NormalInterrupt = 0x10000,
};


/* The Tulip Rx and Tx buffer descriptors. */

enum desc_status_bits
{
  RxWholePacket = 0x0300,
  RxDescFatalError = 0x8000,
  DescOwned = 0x80000000,
};


enum t21041_csr13_bits
{
  /* When reset, resets all SIA functions, machines. */

  csr13_srl = (1 << 0),

  /* CSR13/14/15 autoconfiguration. */

  csr13_cac = (1 << 2),

  /* Clear to force 10bT, set to force AUI/BNC. */

  csr13_aui = (1 << 3),

  /* For eng. purposes only, hardcode at EF0h. */

  csr13_eng = (0xEF0 << 4),


  csr13_mask_auibnc = (csr13_eng | csr13_aui | csr13_cac | csr13_srl),
  csr13_mask_10bt = (csr13_eng | csr13_cac | csr13_srl),
};



/***************************************************************
 * This table shows transmit threshold values based on media   *
 * and these two registers (from PNIC1 & 2 docs) Note: this is *
 * all meaningless if sf is set.                               *
 ***************************************************************/

/***********************************
 * (trh,trl) * 100BaseTX * 10BaseT *
 ***********************************
 *   (0,0)   *     128   *    72   *
 *   (0,1)   *     256   *    96   *
 *   (1,0)   *     512   *   128   *
 *   (1,1)   *    1024   *   160   *
 ***********************************/

enum t21143_csr6_bits
{
  /* Hash/Perfect Receive Filtering Mode: can't be set. */

  csr6_hp = (1 << 0),

  /* Start(1)/Stop(0) Receive. */

  csr6_sr = (1 << 1),

  /* Hash-only filtering mode: can't be set. */

  csr6_ho = (1 << 2),

  /* Pass Bad Frames, (1) causes even bad frames to be passed on. */

  csr6_pb = (1 << 3),

  /* Inverse Filtering, rejects only addresses in address table: can't be set. */

  csr6_if = (1 << 4),

  /* Start(1)/Stop(0) backoff counter. */

  csr6_sb = (1 << 5),

  /* Promiscuous mode. */

  csr6_pr = (1 << 6),

  /* Pass All Multicast. */

  csr6_pm = (1 << 7),

  /* Full duplex mode, disables hearbeat, no loopback. */

  csr6_fd = (1 << 9),

  /* external (PMD) and internal (FIFO) loopback flag. Set both and
     you get (PHY) loopback. */

  csr6_om_ext_loop = (1 << 11),
  csr6_om_int_loop = (1 << 10),

  /* Forces a collision in next transmission (for testing in loopback
     mode). */

  csr6_fc = (1 << 12),

  /* Transmit control: 1 = transmit, 0 = stop. */

  csr6_st = (1 << 13),

  /* Transmit Threshold low bit. */

  csr6_trl = (1 << 14),

  /* Transmit Threshold high bit. */

  csr6_trh = (1 << 15),

  /* Collision Offset Enable. If set uses special algorithm in low
     collision situations. */

  csr6_ca = (1 << 17),

  /* Port Select. 0 (defualt) = 10baseT, 1 = 100baseTX: can't be set. */

  csr6_ps = (1 << 18),

  /* Heart beat disable. Disables SQE function in 10baseT. */

  csr6_hbd = (1 << 19),

  /* Store and forward. If set ignores TR bits. */

  csr6_sf = (1 << 21),

  /* Transmit Threshold Mode, set for 10baseT, 0 for 100BaseTX. */

  csr6_ttm = (1 << 22),

  /* Enables PCS functions (symbol mode requires csr6_ps be set) default is set. */

  csr6_pcs = (1 << 23),

  /* scramble mode flag: can't be set. */

  csr6_scr = (1 << 24),
  csr6_mbo = (1 << 25),
  csr6_ign_dest_msb = (1 << 26),
  csr6_ra = (1 << 30),
  csr6_sc = (1 << 31),
	
  csr6_mask_capture = (csr6_sc | csr6_ca),
  csr6_mask_defstate = (csr6_mask_capture | csr6_mbo),
  csr6_mask_hdcap = (csr6_mask_defstate | csr6_hbd | csr6_ps),
  csr6_mask_hdcaptt = (csr6_mask_hdcap  | csr6_trh | csr6_trl),
  csr6_mask_fullcap = (csr6_mask_hdcaptt | csr6_fd),
  csr6_mask_fullpromisc = (csr6_pr | csr6_pm),
  csr6_mask_filters = (csr6_hp | csr6_ho | csr6_if),
  csr6_mask_100bt = (csr6_scr | csr6_pcs | csr6_hbd),
};


#endif /* !__ENUMERATIONS_H__ */
