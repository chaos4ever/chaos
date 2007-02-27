/*
  Little John, a NES emulator.
  File : unes_mapper.h
  Authors : Yoyo.
  Version : 0.3
  Last update : 3rd Mai 2000
*/

void MMC3_UpdateROM();
void MMC3_UpdateVROM();
void MMC3_Access(word Addr,byte Value);

void MMC1_Access(word Addr,byte Value);

void MMC2_Access(word Addr,byte Value);
void MMC2_Access_Refresh(word PatternTable,byte TileNum);


byte MMC3_R8000;
byte *MMC3_ROMPage[2];
byte *MMC3_VROMPage[8];
byte IRQ_Counter_Initvalue;
byte IRQ_Reset;
byte IRQ_Counter;
byte IRQ_Enable;

int MMC1_Reg[4];
int MMC1_Cnt;
int MMC1_low,MMC1_high;
int MMC1_RegValue;
int MMC1_PageSelect;

byte MMC2_Latch1_lo,MMC2_Latch1_hi,MMC2_Latch2_lo,MMC2_Latch2_hi,MMC2_Latch1_state,MMC2_Latch2_state;
void *MMC_Latch;

