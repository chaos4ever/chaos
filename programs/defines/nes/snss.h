/**************************************************************************/
/*
    snss.h

    (C) 2000 The SNSS Group
    See README.TXT file for license and terms of use.
*/
/**************************************************************************/

#ifndef _SNSS_H_
#define _SNSS_H_

#include <file/file.h>

/**************************************************************************/
/* endian customization */
/**************************************************************************/
/* 
  Endian-ness quick reference:
  the number is:
    $12345678
  the little-endian representation (e.g.: 6502, Intel x86) is:
    78 56 34 12
  the big-endian representation (e.g.: Motorola 68000) is:
    12 34 56 78
  the SNSS file format uses big-endian representation
*/

/* comment/uncomment depending on your processor architecture */
#define USE_LITTLE_ENDIAN
/* #define USE_BIG_ENDIAN */

/**************************************************************************/
/* SNSS constants */
/**************************************************************************/

typedef enum _SNSS_OPEN_MODES
{
  SNSS_OPEN_READ,
  SNSS_OPEN_WRITE
} SNSS_OPEN_MODE;

/* block types */
typedef enum _SNSS_BLOCK_TYPES
{
  SNSS_BASR,
  SNSS_VRAM,
  SNSS_SRAM,
  SNSS_MPRD,
  SNSS_CNTR,
  SNSS_SOUN,
  SNSS_BAD_BLOCK,
  SNSS_READ_ERROR,
} SNSS_BLOCK_TYPE;

/* function return types */
typedef enum _SNSS_RETURN_CODES
{
  SNSS_OK,
  SNSS_BAD_FILE_TAG,
  SNSS_OPEN_FAILED,
  SNSS_CLOSE_FAILED,
  SNSS_READ_FAILED,
  SNSS_WRITE_FAILED,
  SNSS_OUT_OF_MEMORY,
} SNSS_RETURN_CODE;

typedef file_handle_type SNSS_FILE;

#define TAG_LENGTH 4
#define SNSS_BLOCK_VERSION 1

/**************************************************************************/
/* SNSS data structures */
/**************************************************************************/

struct mapper1Data
{
  unsigned char registers[4];
  unsigned char latch;
  unsigned char numberOfBits;
};

struct mapper4Data
{
  unsigned char irqCounter;
  unsigned char irqLatchCounter;
  unsigned char irqCounterEnabled;
  unsigned char last8000Write;
};

struct mapper5Data
{
};

struct mapper6Data
{
  unsigned char irqCounter;
  unsigned char irqLatchCounter;
  unsigned char irqCounterEnabled;
  unsigned char last43FEWrite;
  unsigned char last4500Write;
};

struct mapper9Data
{
  unsigned char latch[2];
  unsigned char lastB000Write;
  unsigned char lastC000Write;
  unsigned char lastD000Write;
  unsigned char lastE000Write;
};

struct mapper10Data
{
  unsigned char latch[2];
  unsigned char lastB000Write;
  unsigned char lastC000Write;
  unsigned char lastD000Write;
  unsigned char lastE000Write;
};

struct mapper16Data
{
  unsigned char irqCounterLowByte;
  unsigned char irqLatchCounterHighByte;
  unsigned char irqCounterEnabled;
};

struct mapper17Data
{
  unsigned char irqCounterLowByte;
  unsigned char irqLatchCounterHighByte;
  unsigned char irqCounterEnabled;
};

struct mapper18Data
{
  unsigned char irqCounterLowByte;
  unsigned char irqLatchCounterHighByte;
  unsigned char irqCounterEnabled;
};

struct mapper19Data
{
  unsigned char irqCounterLowByte;
  unsigned char irqLatchCounterHighByte;
  unsigned char irqCounterEnabled;
};

struct mapper21Data
{
  unsigned char irqCounter;
  unsigned char irqCounterEnabled;
};

struct mapper24Data
{
  unsigned char irqCounter;
  unsigned char irqCounterEnabled;
};

struct mapper69Data
{
  unsigned char irqCounterLowByte;
  unsigned char irqLatchCounterHighByte;
  unsigned char irqCounterEnabled;
};

struct mapper90Data
{
  unsigned char irqCounter;
  unsigned char irqLatchCounter;
  unsigned char irqCounterEnabled;
};

struct mapper224Data
{
  unsigned char chrRamWriteable;
};

struct mapper225Data
{
  unsigned char prgSize;
  unsigned char registers[4];
};

struct mapper226Data
{
  unsigned char chrRamWriteable;
};

struct mapper228Data
{
  unsigned char prgChipSelected;
};

struct mapper230Data
{
  unsigned char numberOfResets;
};

struct SnssFileHeader
{
  char tag[TAG_LENGTH + 1];
  unsigned int numberOfBlocks;
};

/* this block appears before every block in the SNSS file */
struct SnssBlockHeader
{
  char tag[TAG_LENGTH + 1];
  unsigned int blockVersion;
  unsigned int blockLength;
};

#define BASE_BLOCK_LENGTH 0x1931
struct SnssBaseBlock
{
  unsigned char regA;
  unsigned char regX;
  unsigned char regY;
  unsigned char regFlags;
  unsigned char regStack;
  unsigned short regPc;
  unsigned char reg2000;
  unsigned char reg2001;
  unsigned char cpuRam[0x800];
  unsigned char spriteRam[0x100];
  unsigned char ppuRam[0x1000];
  unsigned char palette[0x20];
  unsigned char mirrorState[4];
  unsigned short vramAddress;
  unsigned char spriteRamAddress;
  unsigned char tileXOffset;
};

#define VRAM_16K 0x4000
#define VRAM_8K 0x2000
struct SnssVramBlock
{
  unsigned short vramSize;
  unsigned char vram[0x8000];
};

#define SRAM_1K 0x0400
#define SRAM_2K 0x0800
#define SRAM_3K 0x0C00
#define SRAM_4K 0x1000
#define SRAM_5K 0x1400
#define SRAM_6K 0x1800
#define SRAM_7K 0x1C00
#define SRAM_8K 0x2000
struct SnssSramBlock
{
  unsigned short sramSize;
  unsigned char sramEnabled;
  unsigned char sram[0x2000];
};

#define MAPPER_BLOCK_LENGTH 0x98
struct SnssMapperBlock
{
  unsigned short prgPages[4];
  unsigned short chrPages[8];

  union _extraData
    {
      unsigned char mapperData[128];
      struct mapper1Data mapper1;
      struct mapper4Data mapper4;
      struct mapper5Data mapper5;
      struct mapper6Data mapper6;
      struct mapper9Data mapper9;
      struct mapper10Data mapper10;
      struct mapper16Data mapper16;
      struct mapper17Data mapper17;
      struct mapper18Data mapper18;
      struct mapper19Data mapper19;
      struct mapper21Data mapper21;
      struct mapper24Data mapper24;
      struct mapper69Data mapper69;
      struct mapper90Data mapper90;
      struct mapper224Data mapper224;
      struct mapper225Data mapper225;
      struct mapper226Data mapper226;
      struct mapper228Data mapper228;
      struct mapper230Data mapper230;
    } extraData;
};

struct SnssControllersBlock
{
};

#define SOUND_BLOCK_LENGTH 0x16
struct SnssSoundBlock
{
  unsigned char soundRegisters[0x16];
};

/**************************************************************************/
/* SNSS file manipulation functions */
/**************************************************************************/

/* general file manipulation routines */
SNSS_FILE *openSnssFile (char *filename, SNSS_OPEN_MODE mode);
SNSS_RETURN_CODE getNextBlockType (SNSS_BLOCK_TYPE *blockType,
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE skipNextBlock (SNSS_FILE *snssFile);
SNSS_RETURN_CODE closeSnssFile (SNSS_FILE *snssFile);

/* functions to manipulate the file header */
SNSS_RETURN_CODE readFileHeader (struct SnssFileHeader *fileHeader, 
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeFileHeader (struct SnssFileHeader *fileHeader, 
  SNSS_FILE *snssFile);

/* functions to manipulate the base block */
SNSS_RETURN_CODE readBaseBlock (struct SnssBaseBlock *baseBlock, 
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeBaseBlock (struct SnssBaseBlock *baseBlock, 
  SNSS_FILE *snssFile);

/* functions to manipulate the VRAM block */
SNSS_RETURN_CODE readVramBlock (struct SnssVramBlock *vramBlock, 
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeVramBlock (struct SnssVramBlock *vramBlock, 
  SNSS_FILE *snssFile);

/* functions to manipulate the SRAM block */
SNSS_RETURN_CODE readSramBlock (struct SnssSramBlock *sramBlock, 
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeSramBlock (struct SnssSramBlock *sramBlock, 
  SNSS_FILE *snssFile);

/* functions to manipulate the Mapper block */
SNSS_RETURN_CODE readMapperBlock (struct SnssMapperBlock *mapperBlock, 
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeMapperBlock (struct SnssMapperBlock *mapperBlock, 
  SNSS_FILE *snssFile);

/* functions to manipulate the Controllers block */
SNSS_RETURN_CODE readControllersBlock 
(struct SnssControllersBlock *controllersBlock, SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeControllersBlock 
  (struct SnssControllersBlock *controllersBlock, SNSS_FILE *snssFile);

/* functions to manipulate the Sound block */
SNSS_RETURN_CODE readSoundBlock (struct SnssSoundBlock *soundBlock, 
  SNSS_FILE *snssFile);
SNSS_RETURN_CODE writeSoundBlock (struct SnssSoundBlock *soundBlock, 
  SNSS_FILE *snssFile);

/* support functions */
char *getSnssErrorString (SNSS_RETURN_CODE code);

#endif /* _SNSS_H_ */ 
