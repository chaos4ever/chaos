/*
  Little John, a NES emulator.
  File : unes_ppu.h
  Authors : Yoyo.
  Version : 0.3
  Last update : 3rd Mai 2000
*/



byte ReadPPU(register word Addr);
void WritePPU(register byte Value);
void refresh(long scanline);
void InitMirroring();

byte Spr2draw[64];
byte SprCnt;

//byte VRAM_Buffer[256*64*2];

//byte VROM_Updated;
