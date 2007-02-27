/* $Id$ */
/* Abstract: Enumerations used by the via-rhine driver seriers. */
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


/* Offsets to the device registers. */

enum register_offsets
{
  StationAddr = 0x00,
  ReceiveConfig = 0x06,
  SendConfig = 0x07,
  ChipCmd = 0x08,
  IntrStatus = 0x0C,
  IntrEnable = 0x0E,
  MulticastFilter0 = 0x10,
  MulticastFilter1 = 0x14,
  ReceiveRingPtr = 0x18,
  SendRingPtr = 0x1C,
  MIIPhyAddr = 0x6C,
  MIIStatus = 0x6D,
  PCIBusConfig = 0x6E,
  MIICmd = 0x70,
  MIIRegAddr = 0x71,
  MIIData = 0x72,
  Config = 0x78,
  ConfigA = 0x7A,
  ReceiveMissed = 0x7C,
  ReceiveCRCErrs = 0x7E,
  StickyHW = 0x83,
  WOLcrClr = 0xA4,
  WOLcgClr = 0xA7,
  PwrcsrClr = 0xAC
};


/* Bits in the interrupt status/mask registers. */

enum intr_status_bits
{
  IntrReceiveDone = 0x0001,
  IntrReceiveErr = 0x0004,
  IntrReceiveEmpty = 0x0020,
  IntrSendDone = 0x0002,
  IntrSendAbort = 0x0008,
  IntrSendUnderrun = 0x0010,
  IntrPCIErr = 0x0040,
  IntrStatsMax = 0x0080,
  IntrReceiveEarly = 0x0100,
  IntrMIIChange = 0x0200,
  IntrReceiveOverflow = 0x0400,
  IntrReceiveDropped = 0x0800,
  IntrReceiveNoBuf = 0x1000,
  IntrSendAborted = 0x2000,
  IntrLinkChange = 0x4000,
  IntrReceiveWakeUp = 0x8000,
  IntrNormalSummary = 0x0003,
  IntrAbnormalSummary = 0xC260
};


/* MII interface, status flags.
   Not to be confused with the MIIStatus register ... */

enum mii_status_bits
{
  MIICap100T4			 =  0x8000,
  MIICap10100HdFd		 =  0x7800,
  MIIPreambleSupr		 =  0x0040,
  MIIAutoNegCompleted	 =  0x0020,
  MIIRemoteFault		 =  0x0010,
  MIICapAutoNeg		 =  0x0008,
  MIILink				 =  0x0004,
  MIIJabber			 =  0x0002,
  MIIExtended			 =  0x0001
};


/* Bits in *_desc.status */

enum receive_status_bits
{
  ReceiveOK = 0x8000,
  ReceiveWholePkt = 0x0300,
  ReceiveErr = 0x008F
};

enum desc_status_bits
{
  DescOwn = 0x80000000,
  DescEndPacket = 0x4000,
  DescIntr = 0x1000
};


/* Bits in ChipCmd. */

enum chip_cmd_bits
{
  CmdInit = 0x0001,
  CmdStart = 0x0002,
  CmdStop = 0x0004,
  CmdReceiveOn = 0x0008,
  CmdSendOn = 0x0010,
  CmdSendDemand = 0x0020,
  CmdReceiveDemand = 0x0040,
  CmdEarlyReceive = 0x0100,
  CmdEarlySend = 0x0200,
  CmdFDuplex = 0x0400,
  CmdNoSendPoll = 0x0800,
  CmdReset = 0x8000
};


enum pci_flags_bit
{
  PCI_USES_IO = 1,
  PCI_USES_MEM = 2,
  PCI_USES_MASTER = 4,
  PCI_ADDR0 = 0x10<<0,
  PCI_ADDR1 = 0x10<<1,
  PCI_ADDR2 = 0x10<<2,
  PCI_ADDR3 = 0x10<<3
};

enum via_rhine_chips
{
  VT86C100A = 0,
  VT6102,
  VT3043
};


enum chip_capability_flags
{
  CanHaveMII = 1,
  HasESIPhy = 2,
  HasDavicomPhy = 4,
  ReqSendAlign = 0x10,
  HasWOL = 0x20
};

#endif /* __ENUMERATIONS_H__ */
