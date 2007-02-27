/*
  Little John, a NES emulator.
  File : unes_io.h
  Authors : Yoyo.
  Version : 0.3
  Last update : 3rd Apr 2000
*/


byte Op6502(register word A);
void Wr6502(register word Addr,register byte Value);
byte Rd6502(register word Addr);

byte lastPPUread;
