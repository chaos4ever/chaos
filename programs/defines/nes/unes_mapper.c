/*
  Little John, a NES emulator.
  File : unes_mapper.c
  Authors : Yoyo.
  Version : 0.3
  Last update : 3rd Mai 2000
*/

#include "unes.h"
#include "unes_mapper.h"
#include "unes_ppu.h"

void MMC3_UpdateROM()
{
	if (MMC3_R8000&0x40)
	{
		Vnes.CPUPageIndex[4]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384;
		Vnes.CPUPageIndex[5]=MMC3_ROMPage[1];
		Vnes.CPUPageIndex[6]=MMC3_ROMPage[0];
	
	}
	else
	{
		Vnes.CPUPageIndex[4]=MMC3_ROMPage[0];
		Vnes.CPUPageIndex[5]=MMC3_ROMPage[1];	
		Vnes.CPUPageIndex[6]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384;
	}
}

void MMC3_UpdateVROM()
{
	int i;
	if (!Vnes.var.vrom) return;
	
	if (MMC3_R8000&0x80)
	{
		for (i=0;i<8;i++) 
		  {
		    Vnes.PPUPageIndex[i]=MMC3_VROMPage[i^4];
		    PPUConvertedMemoryIndex[i]=(MMC3_VROMPage[i^4]-chr_beg)*4+PPUConvertedMemory;
		  }

		    
	}
	else
	{
		for (i=0;i<8;i++) 
		  {
		    Vnes.PPUPageIndex[i]=MMC3_VROMPage[i];
		    PPUConvertedMemoryIndex[i]=(MMC3_VROMPage[i]-chr_beg)*4+PPUConvertedMemory;
		  }
	}
	//VROM_Updated=1;

}

void MMC3_Access(word Addr,byte Value)
{
	char sramfile[256],result[256];
	int i,j,k;

	switch (Addr&0xE001)
	{
		case 0x8000:
		{
			MMC3_R8000=Value;
			MMC3_UpdateROM();
			MMC3_UpdateVROM();
			break;
		}
		case 0x8001:
		{
			if ( ((MMC3_R8000&7)<6)&&(!Vnes.var.vrom) ) break;
			switch (MMC3_R8000&7)
			{
				case 0:
				{
				  Value=Value%(Vnes.ROM_Header.chr_rom_pages_nb<<3);
					
					MMC3_VROMPage[0]=chr_beg+(Value<<10);							
					MMC3_VROMPage[1]=chr_beg+(Value<<10)+1024;
					MMC3_UpdateVROM();
					break;
				}
				case 1:
				{
				  Value=Value%(Vnes.ROM_Header.chr_rom_pages_nb<<3);
					MMC3_VROMPage[2]=chr_beg+(Value<<10);
					MMC3_VROMPage[3]=chr_beg+(Value<<10)+1024;
					MMC3_UpdateVROM();
					break;
				}
				case 2:
				{
				  Value=Value%(Vnes.ROM_Header.chr_rom_pages_nb<<3);
					MMC3_VROMPage[4]=chr_beg+(Value<<10);
					MMC3_UpdateVROM();
					break;
				}
				case 3:
				{
				  Value=Value%(Vnes.ROM_Header.chr_rom_pages_nb<<3);
					MMC3_VROMPage[5]=chr_beg+(Value<<10);
					MMC3_UpdateVROM();
					break;
				}
				case 4:
				{
					MMC3_VROMPage[6]=chr_beg+(Value<<10);
					MMC3_UpdateVROM();
					break;
				}
				case 5:
				{
				  Value=Value%(Vnes.ROM_Header.chr_rom_pages_nb<<3);
					MMC3_VROMPage[7]=chr_beg+(Value<<10);
					MMC3_UpdateVROM();
					break;
				}
				case 6:
				{

				  
				  Value=Value%(Vnes.ROM_Header.prg_rom_pages_nb<<1);
					
					MMC3_ROMPage[0]=Vnes.rom+16+(Value<<13);		
					MMC3_UpdateROM();
					break;
				}
				case 7:
				{

					Value=Value%(Vnes.ROM_Header.prg_rom_pages_nb<<1);
					MMC3_ROMPage[1]=Vnes.rom+16+(Value<<13);
					MMC3_UpdateROM();
					break;
				}
			}
			break;
		}
		case 0xA000:
		{
			//mirroring select
			if (Value&0x40) tmp=4;
			else tmp=(Value&1)+1;
			if (Vnes.var.mirroring!=tmp)
			{
				Vnes.var.mirroring=tmp;
				InitMirroring();
			}
			break;
		}
		case 0xA001:
		{

			SaveRAM=Value;
			//			printf("Change saveram to %d\n",Value);
			break;
		}
		case 0xC000:
		{
			IRQ_Counter=Value;
			
//			printf("C0 at %d\n",Vnes.var.CurrentScanLine);
			
			break;
		}
		case 0xC001:
		{
			
			IRQ_Counter_Initvalue=Value;

//			printf("C1 at %d\n",Vnes.var.CurrentScanLine);
			break;
		}
		case 0xE000:
		{
			IRQ_Enable=0;
//			printf("E0 at %d\n",Vnes.var.CurrentScanLine);
//       			IRQ_Counter=IRQ_Counter_Initvalue;
			
			break;
		}
		case 0xE001:
		{
	//		printf("E1 at %d\n",Vnes.var.CurrentScanLine);
			IRQ_Enable=1;
			break;
		}
		return;
	}
	return;
}


void MMC1_Access(word Addr,byte Value)
{
	int tmp;
	long ad1,ad2;
//	printf("Writing %d at %04X\n",Value,Addr);
	if (Value&0x80)
	{
	// Reset mapper 1
  
		MMC1_Reg[0]|=0xC;
		MMC1_RegValue=MMC1_Reg[(Addr>>13)&3];
		MMC1_Cnt=5;		

	}
	else
	{
	// Fill the 5bit buffer
		MMC1_RegValue|=((Value&1)<<(MMC1_Cnt));
		MMC1_Cnt++;
//		printf("%d\n",MMC1_Cnt);
	
	}	

	if (MMC1_Cnt==5)
	{
	// Fill the register with value of the 5bit array


	  MMC1_Reg[(Addr>>13)&3]=MMC1_RegValue&0x1f;
	  MMC1_Cnt=0;
	  MMC1_RegValue=0;

//printf("set reg %d to %d\n",(Addr>>13)&3,MMC1_Reg[(Addr>>13)&3]);
//getchar();
		// Do the update
	  switch (MMC1_Reg[0]&3)
	    {
	    case 0:
	      tmp=3;
	      break;
	    case 1:
	      tmp=4;
	      break;
	    case 2:
	      tmp=1;
	      break;
	    case 3:
	      tmp=2;
	      break;
	    }			
	  //		if (tmp!=Vnes.var.mirroring) 
	  {
	    Vnes.var.mirroring=tmp;
	    //			printf("mirror : %d\n",tmp);
	    InitMirroring();
	  }
	  
	  switch (Vnes.var.cart_size)
	    {
	    case 512:
	      MMC1_PageSelect=(MMC1_Reg[1]&0x10);
	      break;
	    case 1024:
	      if (MMC1_Reg[0]&0x10) MMC1_PageSelect=(MMC1_Reg[1]&0x10)|((MMC1_Reg[2]&0x10)<<1);
	      else MMC1_PageSelect=((MMC1_Reg[1]&0x10)? 0x30:0x00);  //thanks to darcnes
	      break;
	    default:MMC1_PageSelect=0;					
	    }
	  
	  if (!(MMC1_Reg[0]&8))
	    {
	      //switch 32k
	      //	      printf("MMC1 s 32k page=%d\n",(MMC1_Reg[3]&0x1e)+MMC1_PageSelect);
	      MMC1_low=(((MMC1_Reg[3]>>1)<<1)+MMC1_PageSelect)%Vnes.ROM_Header.prg_rom_pages_nb;
	      MMC1_high=(((MMC1_Reg[3]>>1)<<1)+MMC1_PageSelect+1)%Vnes.ROM_Header.prg_rom_pages_nb;
	    }
	  else
	    {
	      //switch 16k at location based on bit 2
	      //	      	      printf("MMC1 s 16k pos=%d page=%d\n",MMC1_Reg[0]&4,MMC1_Reg[3]+MMC1_PageSelect);

	      if (MMC1_Reg[0]&4)
		{
		  MMC1_low=(MMC1_Reg[3]+MMC1_PageSelect)%Vnes.ROM_Header.prg_rom_pages_nb;
		  MMC1_high=(0xf+MMC1_PageSelect)%Vnes.ROM_Header.prg_rom_pages_nb;
		}
	      else
		{
		  MMC1_low=0;
		  MMC1_high=(MMC1_Reg[3]+MMC1_PageSelect)%Vnes.ROM_Header.prg_rom_pages_nb;
		}
	    }
	  
	  Vnes.CPUPageIndex[4]=Vnes.rom+16+MMC1_low*16384;
	  Vnes.CPUPageIndex[5]=Vnes.CPUPageIndex[4]+8192;
	  Vnes.CPUPageIndex[6]=Vnes.rom+16+MMC1_high*16384;
	  Vnes.CPUPageIndex[7]=Vnes.CPUPageIndex[6]+8192;
	  
	  //on s occupe de la vrom
	  if (Vnes.var.vrom)
	    {
	      if (MMC1_Reg[0]&0x10)
		{
		  //		  printf("4k vrom switch %d %d\n",MMC1_Reg[1],MMC1_Reg[2]);
		  ad1=(MMC1_Reg[1])%(Vnes.ROM_Header.chr_rom_pages_nb<<1);
		  ad2=(MMC1_Reg[2])%(Vnes.ROM_Header.chr_rom_pages_nb<<1);
		  //swap 4k vrom at 0000 & 1000
		  Vnes.PPUPageIndex[0]=chr_beg+(ad1<<12);
		  Vnes.PPUPageIndex[1]=chr_beg+(ad1<<12)+1024;
		  Vnes.PPUPageIndex[2]=chr_beg+(ad1<<12)+2048;
		  Vnes.PPUPageIndex[3]=chr_beg+(ad1<<12)+3072;
		  Vnes.PPUPageIndex[4]=chr_beg+(ad2<<12);
		  Vnes.PPUPageIndex[5]=chr_beg+(ad2<<12)+1024;
		  Vnes.PPUPageIndex[6]=chr_beg+(ad2<<12)+2048;
		  Vnes.PPUPageIndex[7]=chr_beg+(ad2<<12)+3072;
		  
		  PPUConvertedMemoryIndex[0]=(ad1<<14)+PPUConvertedMemory;
		  PPUConvertedMemoryIndex[1]=PPUConvertedMemoryIndex[0]+4096;
		  PPUConvertedMemoryIndex[2]=PPUConvertedMemoryIndex[0]+4096*2;
		  PPUConvertedMemoryIndex[3]=PPUConvertedMemoryIndex[0]+4096*3;
		  PPUConvertedMemoryIndex[4]=(ad2<<14)+PPUConvertedMemory;
		  PPUConvertedMemoryIndex[5]=PPUConvertedMemoryIndex[4]+4096;
		  PPUConvertedMemoryIndex[6]=PPUConvertedMemoryIndex[4]+4096*2;
		  PPUConvertedMemoryIndex[7]=PPUConvertedMemoryIndex[4]+4096*3;
		  
		}
	      else
		{
		  //		  printf("8k vrom switch %d\n",MMC1_Reg[1]);
		  //swap 8k vrom at 0000
		  ad1=(MMC1_Reg[1]&31)%(Vnes.ROM_Header.chr_rom_pages_nb<<1);
		  
		  Vnes.PPUPageIndex[0]=chr_beg+(ad1<<12);
		  Vnes.PPUPageIndex[1]=chr_beg+(ad1<<12)+1024;
		  Vnes.PPUPageIndex[2]=chr_beg+(ad1<<12)+1024*2;
		  Vnes.PPUPageIndex[3]=chr_beg+(ad1<<12)+1024*3;
		  Vnes.PPUPageIndex[4]=chr_beg+(ad1<<12)+1024*4;
		  Vnes.PPUPageIndex[5]=chr_beg+(ad1<<12)+1024*5;
		  Vnes.PPUPageIndex[6]=chr_beg+(ad1<<12)+1024*6;
		  Vnes.PPUPageIndex[7]=chr_beg+(ad1<<12)+1024*7;
		  
		  PPUConvertedMemoryIndex[0]=(ad1<<14)+PPUConvertedMemory;
		  PPUConvertedMemoryIndex[1]=PPUConvertedMemoryIndex[0]+4096;
		  PPUConvertedMemoryIndex[2]=PPUConvertedMemoryIndex[0]+4096*2;
		  PPUConvertedMemoryIndex[3]=PPUConvertedMemoryIndex[0]+4096*3;
		  PPUConvertedMemoryIndex[4]=PPUConvertedMemoryIndex[0]+4096*4;
		  PPUConvertedMemoryIndex[5]=PPUConvertedMemoryIndex[0]+4096*5;
		  PPUConvertedMemoryIndex[6]=PPUConvertedMemoryIndex[0]+4096*6;
		  PPUConvertedMemoryIndex[7]=PPUConvertedMemoryIndex[0]+4096*7;
		  
		}
	    }
	}
	
}

void MMC2_Access_Refresh(word PatternTable,byte TileNb)
{
  if (TileNb==253)  //0xfd
    {
      if (PatternTable==0)
	{
	  MMC2_Latch1_state=0xfd;
	  MMC2_Access(0xB000,MMC2_Latch1_lo);
	}
      else
	{
	  MMC2_Latch2_state=0xfd;
	  MMC2_Access(0xD000,MMC2_Latch2_lo);
	}
    }
  else
    if (TileNb==254) //0xfe
      {
	if (PatternTable==0)
	  {
	    MMC2_Latch1_state=0xfe;
	    MMC2_Access(0xC000,MMC2_Latch1_hi);

	  }
	else
	  {
	    MMC2_Latch2_state=0xfe;
	    MMC2_Access(0xE000,MMC2_Latch2_hi);
	  }
      }
}

void MMC2_Access(word Addr,byte Value)
{

  switch (Addr&0xF000)
    {
    case 0xA000:
      Vnes.CPUPageIndex[4]=Vnes.rom+16+(Value%(Vnes.ROM_Header.prg_rom_pages_nb<<1))*8192;
      break;
    case 0xB000: 
      MMC2_Latch1_lo=(Value%(Vnes.ROM_Header.chr_rom_pages_nb<<1));
      if (MMC2_Latch1_state==0xFD)
	{
	  Vnes.PPUPageIndex[0]=chr_beg+MMC2_Latch1_lo*4096;
	  Vnes.PPUPageIndex[1]=Vnes.PPUPageIndex[0]+1024;
	  Vnes.PPUPageIndex[2]=Vnes.PPUPageIndex[0]+2048;
	  Vnes.PPUPageIndex[3]=Vnes.PPUPageIndex[0]+3072;
	  PPUConvertedMemoryIndex[0]=MMC2_Latch1_lo*4096*4+PPUConvertedMemory;
	  PPUConvertedMemoryIndex[1]=PPUConvertedMemoryIndex[0]+4096;
	  PPUConvertedMemoryIndex[2]=PPUConvertedMemoryIndex[0]+4096*2;
	  PPUConvertedMemoryIndex[3]=PPUConvertedMemoryIndex[0]+4096*3;
	}
      break;
    case 0xC000:
      MMC2_Latch1_hi=(Value%(Vnes.ROM_Header.chr_rom_pages_nb<<1));
      if (MMC2_Latch1_state==0xFE)
	{
	  Vnes.PPUPageIndex[0]=chr_beg+MMC2_Latch1_hi*4096;
	  Vnes.PPUPageIndex[1]=Vnes.PPUPageIndex[0]+1024;
	  Vnes.PPUPageIndex[2]=Vnes.PPUPageIndex[0]+2048;
	  Vnes.PPUPageIndex[3]=Vnes.PPUPageIndex[0]+3072;
	  PPUConvertedMemoryIndex[0]=MMC2_Latch1_hi*4096*4+PPUConvertedMemory;
	  PPUConvertedMemoryIndex[1]=PPUConvertedMemoryIndex[0]+4096;
	  PPUConvertedMemoryIndex[2]=PPUConvertedMemoryIndex[0]+4096*2;
	  PPUConvertedMemoryIndex[3]=PPUConvertedMemoryIndex[0]+4096*3;
	}
      

      break;
    case 0xD000:
      MMC2_Latch2_lo=(Value%(Vnes.ROM_Header.chr_rom_pages_nb<<1));
      if (MMC2_Latch2_state==0xFD)
	{
	  Vnes.PPUPageIndex[4]=chr_beg+MMC2_Latch2_lo*4096;
	  Vnes.PPUPageIndex[5]=Vnes.PPUPageIndex[4]+1024;
	  Vnes.PPUPageIndex[6]=Vnes.PPUPageIndex[4]+2048;
	  Vnes.PPUPageIndex[7]=Vnes.PPUPageIndex[4]+3072;		
	  PPUConvertedMemoryIndex[4]=MMC2_Latch2_lo*4096*4+PPUConvertedMemory;
	  PPUConvertedMemoryIndex[5]=PPUConvertedMemoryIndex[4]+4096;
	  PPUConvertedMemoryIndex[6]=PPUConvertedMemoryIndex[4]+4096*2;
	  PPUConvertedMemoryIndex[7]=PPUConvertedMemoryIndex[4]+4096*3;
	}
      break;
    case 0xE000:
      MMC2_Latch2_hi=(Value%(Vnes.ROM_Header.chr_rom_pages_nb<<1));
      if (MMC2_Latch2_state==0xFE)
	{
	  Vnes.PPUPageIndex[4]=chr_beg+MMC2_Latch2_hi*4096;
	  Vnes.PPUPageIndex[5]=Vnes.PPUPageIndex[4]+1024;
	  Vnes.PPUPageIndex[6]=Vnes.PPUPageIndex[4]+2048;
	  Vnes.PPUPageIndex[7]=Vnes.PPUPageIndex[4]+3072;	
	  PPUConvertedMemoryIndex[4]=MMC2_Latch2_hi*4096*4+PPUConvertedMemory;
	  PPUConvertedMemoryIndex[5]=PPUConvertedMemoryIndex[4]+4096;
	  PPUConvertedMemoryIndex[6]=PPUConvertedMemoryIndex[4]+4096*2;
	  PPUConvertedMemoryIndex[7]=PPUConvertedMemoryIndex[4]+4096*3;	
	}
      break;
      
    case 0xF000:
      if (((Value&1)+1)!=Vnes.var.mirroring)
	{
	  Vnes.var.mirroring=(Value&1)+1;
	  InitMirroring();
	  break;
	}
    }

}
