/**************************************************************************/
/*
    snss.c

    (C) 2000 The SNSS Group
    See README.TXT file for license and terms of use.
*/
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snss.h"

/**************************************************************************/
/* This section deals with endian-specific code. */
/**************************************************************************/

#ifdef USE_LITTLE_ENDIAN

unsigned int
swap32 (unsigned int source)
{
  char buffer[4];
  
  buffer[0] = ((char *) &source)[3];
  buffer[1] = ((char *) &source)[2];
  buffer[2] = ((char *) &source)[1];
  buffer[3] = ((char *) &source)[0];

  return *((unsigned int *) buffer);
}

unsigned short
swap16 (unsigned short source)
{
  char buffer[2];
  
  buffer[0] = ((char *) &source)[1];
  buffer[1] = ((char *) &source)[0];

  return *((unsigned short *) buffer);
}

#endif /* USE_LITTLE_ENDIAN */

#ifdef USE_BIG_ENDIAN

unsigned int
swap32 (unsigned int source)
{
  return source;
}

unsigned short
swap16 (unsigned short source)
{
  return source;
}

#endif /* USE_BIG_ENDIAN */

/**************************************************************************/
/* support functions */
/**************************************************************************/

#define MIN(a,b) (a<=b)?a:b

/**************************************************************************/

SNSS_RETURN_CODE
readSnssBlockHeader (struct SnssBlockHeader *header, SNSS_FILE *snssFile)
{
  char headerBytes[12];

  if (fread (headerBytes, 12, 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  strncpy (header->tag, &headerBytes[0], TAG_LENGTH);
  header->tag[4] = '\0';
  header->blockVersion = *((unsigned int *) &headerBytes[4]);
  header->blockVersion = swap32 (header->blockVersion);
  header->blockLength = *((unsigned int *) &headerBytes[8]);
  header->blockLength = swap32 (header->blockLength);

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE
writeSnssBlockHeader (struct SnssBlockHeader *header, SNSS_FILE *snssFile)
{
  char headerBytes[12];
  unsigned int tempInt;

  strncpy (&headerBytes[0], header->tag, TAG_LENGTH);

  tempInt = swap32 (header->blockVersion);
  headerBytes[4] = ((char *) &tempInt)[0];
  headerBytes[5] = ((char *) &tempInt)[1];
  headerBytes[6] = ((char *) &tempInt)[2];
  headerBytes[7] = ((char *) &tempInt)[3];

  tempInt = swap32 (header->blockLength);
  headerBytes[8] = ((char *) &tempInt)[0];
  headerBytes[9] = ((char *) &tempInt)[1];
  headerBytes[10] = ((char *) &tempInt)[2];
  headerBytes[11] = ((char *) &tempInt)[3];

  if (fwrite (headerBytes, 12, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/

char *getSnssErrorString (SNSS_RETURN_CODE code)
{
  switch (code)
    {
    case SNSS_OK:
      return strdup ("no error");
      break;
    case SNSS_BAD_FILE_TAG:
      return strdup ("the SNSS file is invalid");
      break;
    case SNSS_OPEN_FAILED:
      return strdup ("there was a problem opening the SNSS file");
      break;
    case SNSS_CLOSE_FAILED:
      return strdup ("there was a problem closing the SNSS file");
      break;
    case SNSS_READ_FAILED:
      return strdup ("there was a problem reading from the SNSS file");
      break;
    case SNSS_WRITE_FAILED:
      return strdup ("there was a problem writing to SNSS file");
      break;
    case SNSS_OUT_OF_MEMORY:
      return strdup ("could not allocate enough memory");
      break;
    default:
      return strdup ("unknown error");
      break;
    }
}

/**************************************************************************/
/* general file manipulation functions */
/**************************************************************************/

SNSS_FILE *
openSnssFile (char *filename, SNSS_OPEN_MODE mode)
{
  if (SNSS_OPEN_READ == mode)
    return fopen (filename, "rb");
  else
    return fopen (filename, "wb");
}

/**************************************************************************/

SNSS_RETURN_CODE 
getNextBlockType (SNSS_BLOCK_TYPE *blockType, SNSS_FILE *snssFile)
{
  char tagBuffer[TAG_LENGTH + 1];

  if (fread (tagBuffer, TAG_LENGTH, 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }
  tagBuffer[TAG_LENGTH] = '\0';

  /* reset the file pointer to the start of the block */
  if (fseek (snssFile, -TAG_LENGTH, SEEK_CUR) != 0)
    {
      return SNSS_READ_FAILED;
    }

  /* figure out which type of block it is */
  if (strcmp (tagBuffer, "BASR") == 0)
    {
      *blockType = SNSS_BASR;
      return SNSS_OK;
    }
  else if (strcmp (tagBuffer, "VRAM") == 0)
    {
      *blockType = SNSS_VRAM;
      return SNSS_OK;
    }
  else if (strcmp (tagBuffer, "SRAM") == 0)
    {
      *blockType = SNSS_SRAM;
      return SNSS_OK;
    }
  else if (strcmp (tagBuffer, "MPRD") == 0)
    {
      *blockType = SNSS_MPRD;
      return SNSS_OK;
    }
  else if (strcmp (tagBuffer, "CNTR") == 0)
    {
      *blockType = SNSS_CNTR;
      return SNSS_OK;
    }
  else if (strcmp (tagBuffer, "SOUN") == 0)
    {
      *blockType = SNSS_SOUN;
      return SNSS_OK;
    }
  else
    {
      *blockType = SNSS_BAD_BLOCK;
      return SNSS_OK;
    }
}

/**************************************************************************/

SNSS_RETURN_CODE 
skipNextBlock (SNSS_FILE *snssFile)
{
  unsigned int blockLength;

  /* skip the block's tag and version */
  if (fseek (snssFile, TAG_LENGTH + sizeof (unsigned int), SEEK_CUR) != 0)
    {
      return SNSS_READ_FAILED;
    }

  /* get the block data length */
  if (fread (&blockLength, sizeof (unsigned int), 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }
  blockLength = swap32 (blockLength);

  /* skip over the block data */
  if (fseek (snssFile, blockLength, SEEK_CUR) != 0)
    {
      return SNSS_READ_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
closeSnssFile (SNSS_FILE *snssFile)
{
  if (fclose (snssFile) != 0)
    {
      return SNSS_CLOSE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing SNSS headers */
/**************************************************************************/

SNSS_RETURN_CODE 
readFileHeader (struct SnssFileHeader *fileHeader, SNSS_FILE *snssFile)
{
  if (fread (fileHeader->tag, 4, 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }
  fileHeader->tag[4] = '\0';

  if (fread (&fileHeader->numberOfBlocks, sizeof (unsigned int), 1, 
    snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }
  fileHeader->numberOfBlocks = swap32 (fileHeader->numberOfBlocks);

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeFileHeader (struct SnssFileHeader *fileHeader, SNSS_FILE *snssFile)
{
  unsigned int tempInt;
  char writeBuffer[8];

  /* always place the SNSS tag in this field */
  strncpy (&writeBuffer[0], "SNSS", 4);
  tempInt = swap32 (fileHeader->numberOfBlocks);
  writeBuffer[4] = ((char *) &tempInt)[0];
  writeBuffer[5] = ((char *) &tempInt)[1];
  writeBuffer[6] = ((char *) &tempInt)[2];
  writeBuffer[7] = ((char *) &tempInt)[3];

  if (fwrite (writeBuffer, 8, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing base register blocks */
/**************************************************************************/

SNSS_RETURN_CODE 
readBaseBlock (struct SnssBaseBlock *baseBlock, SNSS_FILE *snssFile)
{
  char blockBytes[BASE_BLOCK_LENGTH];
  struct SnssBlockHeader header;

  if (readSnssBlockHeader (&header, snssFile) != SNSS_OK)
    {
      return SNSS_READ_FAILED;
    }

  if (fread (blockBytes, MIN (header.blockLength, BASE_BLOCK_LENGTH), 1, 
    snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  baseBlock->regA = blockBytes[0x0];
  baseBlock->regX = blockBytes[0x1];
  baseBlock->regY = blockBytes[0x2];
  baseBlock->regFlags = blockBytes[0x3];
  baseBlock->regStack = blockBytes[0x4];
  baseBlock->regPc = *((unsigned short *) &blockBytes[0x5]);
  baseBlock->regPc = swap16 (baseBlock->regPc);
  baseBlock->reg2000 = blockBytes[0x7];
  baseBlock->reg2001 = blockBytes[0x8];
  memcpy (&baseBlock->cpuRam, &blockBytes[0x9], 0x800);
  memcpy (&baseBlock->spriteRam, &blockBytes[0x809], 0x100);
  memcpy (&baseBlock->ppuRam, &blockBytes[0x909], 0x1000);
  memcpy (&baseBlock->palette, &blockBytes[0x1909], 0x20);
  memcpy (&baseBlock->mirrorState, &blockBytes[0x1929], 0x4);
  baseBlock->vramAddress = *((unsigned short *) &blockBytes[0x192D]);
  baseBlock->vramAddress = swap16 (baseBlock->vramAddress);
  baseBlock->spriteRamAddress = blockBytes[0x192F];
  baseBlock->tileXOffset = blockBytes[0x1930];

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeBaseBlock (struct SnssBaseBlock *baseBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;
  SNSS_RETURN_CODE returnCode;
  char blockBytes[BASE_BLOCK_LENGTH];
  unsigned short tempShort;

  strcpy (header.tag, "BASR");
  header.blockVersion = SNSS_BLOCK_VERSION;
  header.blockLength = BASE_BLOCK_LENGTH;
  if ((returnCode = writeSnssBlockHeader (&header, snssFile)) != SNSS_OK)
    {
      return returnCode;
    }

  blockBytes[0x0] = baseBlock->regA;
  blockBytes[0x1] = baseBlock->regX;
  blockBytes[0x2] = baseBlock->regY;
  blockBytes[0x3] = baseBlock->regFlags;
  blockBytes[0x4] = baseBlock->regStack;
  tempShort = swap16 (baseBlock->regPc);
  blockBytes[0x5] = ((char *) &tempShort)[0];
  blockBytes[0x6] = ((char *) &tempShort)[1];
  blockBytes[0x7] = baseBlock->reg2000;
  blockBytes[0x8] = baseBlock->reg2001;
  memcpy (&blockBytes[0x9], &baseBlock->cpuRam, 0x800);
  memcpy (&blockBytes[0x809], &baseBlock->spriteRam, 0x100);
  memcpy (&blockBytes[0x909], &baseBlock->ppuRam, 0x1000);
  memcpy (&blockBytes[0x1909], &baseBlock->palette, 0x20);
  memcpy (&blockBytes[0x1929], &baseBlock->mirrorState, 0x4);
  tempShort = swap16 (baseBlock->vramAddress);
  blockBytes[0x192D] = ((char *) &tempShort)[0];
  blockBytes[0x192E] = ((char *) &tempShort)[1];
  blockBytes[0x192F] = baseBlock->spriteRamAddress;
  blockBytes[0x1930] = baseBlock->tileXOffset;

  if (fwrite (blockBytes, BASE_BLOCK_LENGTH, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing VRAM blocks */
/**************************************************************************/

SNSS_RETURN_CODE 
readVramBlock (struct SnssVramBlock *vramBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;

  if (readSnssBlockHeader (&header, snssFile) != SNSS_OK)
    {
      return SNSS_READ_FAILED;
    }

  if (fread (vramBlock->vram, MIN (header.blockLength, VRAM_16K), 1,
    snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  vramBlock->vramSize = header.blockLength;

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeVramBlock (struct SnssVramBlock *vramBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;
  SNSS_RETURN_CODE returnCode;

  strcpy (header.tag, "VRAM");
  header.blockVersion = SNSS_BLOCK_VERSION;
  header.blockLength = vramBlock->vramSize;
  if ((returnCode = writeSnssBlockHeader (&header, snssFile)) != SNSS_OK)
    {
      return returnCode;
    }

  if (fwrite (vramBlock->vram, vramBlock->vramSize, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing SRAM blocks */
/**************************************************************************/

SNSS_RETURN_CODE 
readSramBlock (struct SnssSramBlock *sramBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;

  if (readSnssBlockHeader (&header, snssFile) != SNSS_OK)
    {
      return SNSS_READ_FAILED;
    }

  if (fread (&sramBlock->sramEnabled, 1, 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  /* read blockLength - 1 bytes to get all of the SRAM */
  if (fread (&sramBlock->sram, 
    MIN (header.blockLength - 1, SRAM_8K), 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  /* SRAM size is the size of the block - 1 (SRAM enabled byte) */
  sramBlock->sramSize = header.blockLength - 1;

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeSramBlock (struct SnssSramBlock *sramBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;
  SNSS_RETURN_CODE returnCode;

  strcpy (header.tag, "SRAM");
  header.blockVersion = SNSS_BLOCK_VERSION;
  /* length of block is size of SRAM plus SRAM enabled byte */
  header.blockLength = sramBlock->sramSize + 1;
  if ((returnCode = writeSnssBlockHeader (&header, snssFile)) != SNSS_OK)
    {
      return returnCode;
    }

  if (fwrite (&sramBlock->sramEnabled, 1, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  if (fwrite (sramBlock->sram, sramBlock->sramSize, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing mapper data blocks */
/**************************************************************************/

SNSS_RETURN_CODE 
readMapperBlock (struct SnssMapperBlock *mapperBlock, SNSS_FILE *snssFile)
{
  char *blockBytes;
  int i;
  struct SnssBlockHeader header;

  if (readSnssBlockHeader (&header, snssFile) != SNSS_OK)
    {
      return SNSS_READ_FAILED;
    }

  if ((blockBytes = (char *) malloc (0x8 + 0x10 + 0x80)) == NULL)
    {
      return SNSS_OUT_OF_MEMORY;
    }

  if (fread (blockBytes, 
    MIN (0x8 + 0x10 + 0x80, header.blockLength), 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  for (i = 0; i < 4; i++)
    {
      mapperBlock->prgPages[i] = *((unsigned short *) &blockBytes[i * 2]);
      mapperBlock->prgPages[i] = swap16 (mapperBlock->prgPages[i]);
    }

  for (i = 0; i < 8; i++)
    {
      mapperBlock->chrPages[i] = *((unsigned short *) &blockBytes[0x8 + i * 2]);
      mapperBlock->chrPages[i] = swap16 (mapperBlock->chrPages[i]);
    }

  memcpy (&mapperBlock->extraData.mapperData, &blockBytes[0x18], 0x80);

  free (blockBytes);

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeMapperBlock (struct SnssMapperBlock *mapperBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;
  char blockBytes[MAPPER_BLOCK_LENGTH];
  unsigned short tempShort;
  int i;
  SNSS_RETURN_CODE returnCode;

  strcpy (header.tag, "MPRD");
  header.blockVersion = SNSS_BLOCK_VERSION;
  header.blockLength = MAPPER_BLOCK_LENGTH;
  if ((returnCode = writeSnssBlockHeader (&header, snssFile)) != SNSS_OK)
    {
      return returnCode;
    }

  for (i = 0; i < 4; i++)
    {
      tempShort = swap16 (mapperBlock->prgPages[i]);
      blockBytes[i * 2 + 0] = ((char *) &tempShort)[0];
      blockBytes[i * 2 + 1] = ((char *) &tempShort)[1];
    }

  for (i = 0; i < 8; i++)
    {
      tempShort = swap16 (mapperBlock->chrPages[i]);
      blockBytes[0x8 + i * 2 + 0] = ((char *) &tempShort)[0];
      blockBytes[0x8 + i * 2 + 1] = ((char *) &tempShort)[1];
    }

  memcpy (&blockBytes[0x18], &mapperBlock->extraData.mapperData, 0x80);

  if (fwrite (blockBytes, MAPPER_BLOCK_LENGTH, 1, snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing controller data blocks */
/**************************************************************************/

SNSS_RETURN_CODE 
readControllersBlock (struct SnssControllersBlock *controllersBlock, 
  SNSS_FILE *snssFile)
{
  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeControllersBlock (struct SnssControllersBlock *controllersBlock, 
  SNSS_FILE *snssFile)
{
  return SNSS_OK;
}

/**************************************************************************/
/* functions for reading and writing sound blocks */
/**************************************************************************/

SNSS_RETURN_CODE 
readSoundBlock (struct SnssSoundBlock *soundBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;

  if (readSnssBlockHeader (&header, snssFile) != SNSS_OK)
    {
      return SNSS_READ_FAILED;
    }

  if (fread (soundBlock->soundRegisters, 
    MIN (header.blockLength, 0x16), 1, snssFile) != 1)
    {
      return SNSS_READ_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/

SNSS_RETURN_CODE 
writeSoundBlock (struct SnssSoundBlock *soundBlock, SNSS_FILE *snssFile)
{
  struct SnssBlockHeader header;
  SNSS_RETURN_CODE returnCode;

  strcpy (header.tag, "SOUN");
  header.blockVersion = SNSS_BLOCK_VERSION;
  header.blockLength = SOUND_BLOCK_LENGTH;
  if ((returnCode = writeSnssBlockHeader (&header, snssFile)) != SNSS_OK)
    {
      return returnCode;
    }

  if (fwrite (soundBlock->soundRegisters, SOUND_BLOCK_LENGTH, 1, 
    snssFile) != 1)
    {
      return SNSS_WRITE_FAILED;
    }

  return SNSS_OK;
}

/**************************************************************************/
