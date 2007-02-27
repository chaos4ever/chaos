/*
  Little John, a NES emulator.
  File : unes.h
  Authors : Yoyo.
  Version : 0.4
  Last update : 5th May 2000
*/


#ifndef _UNES_H_
#define _UNES_H_

#include "6502.h"
#define INT_NONE 0
#define INT_IRQ 1
#define INT_NMI 2
#define INT_QUIT 3

#define Rd6502 get6502memory

typedef unsigned char byte;
typedef unsigned short word;
typedef signed char offset;

extern byte a_reg,x_reg,y_reg,flag_reg,s_reg;
extern word pc_reg;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


byte ShowBackSprite;
byte ShowBackGround;
byte ShowFrontSprite;

#define AUTO_FSKIP 255
static int skipframe;
static int oldfskiprate;

long framecounter;

// .NES Rom file header structure



typedef struct 
{ 
	char id[4]; //should be a 'NES'+#0x1A  here
        char prg_rom_pages_nb;char chr_rom_pages_nb;
	char rom_ctrl1,rom_ctrl2;
	char dummy;  //should be a 0x00 here
} NES_file_header;

#define MaxScanLine 232
#define MinScanLine 8
#define VblankScanLine 243
#define VblankScanLineB 245
#define LastScanLine 262

byte vbl;

int iperiodadj;

char *gamename;

int select_nt;
int select_bpt;
int select_spt;

int fskiprate;
int frameskipped;

int tamyo;
char emupause;

long starttime;
   

typedef struct {

	byte Reg2005_FirstAccess;  
	word Reg2006_VRAMAddress;
	byte Reg2006_FirstAccess;
	byte BGDScroll_H;
	byte BGDScroll_V;
	word CurrentScanLine;
	byte hscroll,vscroll;
	
	byte SubScanLine;

	byte JoyPad1_BitIndex;
	byte JoyPad2_BitIndex;

	byte hitsprite0;
	byte vrom;
	byte mirroring;
	
	
	byte spr0ok;
  byte DrawCframe;
	int  cart_size;
} NesVar;

byte SpriteDrawn[64];
byte BgndColor;

typedef struct { 
	NES_file_header ROM_Header;
        char Mapper_used;
		
	byte *CPUMemory; //64k
	byte *CPUPageIndex[8];  //8k pages pointers
	
	byte *PPUMemory;  //16K
	byte *PPUPageIndex[16];  //1k pages pointers

	byte *SPRMemory;
	byte *rom;
#ifdef __marat__
	M6502 *CPU;
#endif
	long romsize;
	NesVar var;
} VirtualNES;

VirtualNES Vnes;

byte *Vbuffer;
byte *chr_end,*chr_beg;
byte SaveRAM;



byte cpt;
byte tmp;

int forcemirror;
int forcemapper;
int refreshaddress2000;
int refreshaddress2000tmp;

long CurrentCycle;
byte SoundMode;

int romsize;

/*****************************************************
  Send an error msg
*****************************************************/
void SendErrorMsg(char *errormsg);

/*****************************************************
  Open, check and return memorycopy of a ROM file
*****************************************************/

char Load_ROM(byte *RomName);

char Open_ROM();

char Init_NES(char *RomName);

/*****************************************************
  Close the ROM
*****************************************************/

char Close_ROM();


//Palette from Fem

word MyPal16bbp[64];

typedef struct __pal {
	int r;
	int g;
	int b;
} pal;


/******************************
  Needed for VRAM/VROM caching
*******************************/

byte *PPUConvertedMemory;
byte *PPUConvertedMemoryIndex[8];  //4k pages pointers



//
byte viewsample;
long lastsamplepos;

int toggles;

#endif
