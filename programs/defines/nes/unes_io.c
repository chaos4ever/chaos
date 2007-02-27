/*
  Little John, a NES emulator.
  File : unes_io.c
  Authors : Yoyo.
  Version : 0.3 
  Last update : 3rd Mai 2000
*/



#include "unes.h"
#include "unes_io.h"
#include "unes_ppu.h"
#include "unes_apu.h"
#include "unes_mapper.h"

#include "ljng.h"

extern byte fileisnsf;
extern byte *nsfram;

extern pal palette[64];

word vramlatch=0;

byte Op6502(register word A) { 

  return(Vnes.CPUPageIndex[(A>>13)][A&0x1FFF]); }

#ifdef __marat__
void Wr6502(register word Addr,register byte Value)
#else
void put6502memory(WORD Addr, BYTE Value)
#endif
{
  int tmp,i;

  //printf("wr %04X=%d\n",Addr,Value);


  switch (Addr&0xF000)
    {
    case 0x0:  //RAM access
    case 0x1000:
      {

	Vnes.CPUMemory[Addr&0x7FF]=Value;  //Vnes.var.mirroring
	return;
      }
    case 0x2000: //Registers 0x2000 access  
    case 0x3000:
      {
	//	printf("Wr %04X at %04X on scan %d\n",Value,Addr,Vnes.var.CurrentScanLine);

		
	switch (Addr&0x2007)
	  {
	  case 0x2000:
	    {
	      Vnes.CPUMemory[0x2000]=Value;


	      //printf("wr 2000 on %d with %02X\n",Vnes.var.CurrentScanLine,Value);
	      
	      return;
	    }
	  case 0x2001: Vnes.CPUMemory[Addr&0x2007]=Value; return;
	  case 0x2003: Vnes.CPUMemory[Addr&0x2007]=Value; return;
	  case 0x2004:          
	    {   
	      //	      	      printf("wr Spr mem on %d\n",Vnes.var.CurrentScanLine);
	      Vnes.SPRMemory[Vnes.CPUMemory[0x2003]++]=Value;	      
	      return;
	    }
	  case 0x2005:
	    {
	      //printf("2005 on %d val %d\n",Vnes.var.CurrentScanLine,Value);
	      
	      if (Vnes.var.Reg2005_FirstAccess^=1)
		{
		  Vnes.var.BGDScroll_V=Value;

		  vramlatch=(vramlatch&0x3c00)|(Value<<2);
		  //printf("vscroll %d scan %d\n",Vnes.var.BGDScroll_V,Vnes.var.CurrentScanLine);
		}
	      else
		{
		  Vnes.var.BGDScroll_H=Value;
		}

	      return;
	    }
	    
	  case 0x2006:
	    {
	      if (Vnes.var.Reg2005_FirstAccess^=1)
		vramlatch=(vramlatch&0xff00)|Value;
	      else
		vramlatch=(vramlatch&0xff)|(Value<<8);


	      if ((Vnes.var.CurrentScanLine<VblankScanLine)&&(Vnes.var.CurrentScanLine>3))
		{
		  //		  printf("2006 on %d val %d\n",Vnes.var.CurrentScanLine,Value);
		  if (Vnes.var.Reg2005_FirstAccess)
		    {		  
		      Vnes.var.SubScanLine=Vnes.var.CurrentScanLine-1;
		      Vnes.var.vscroll=(((vramlatch>>5)&31)<<3)+(((vramlatch>>12)&3));
		      Vnes.var.BGDScroll_H=(Vnes.var.BGDScroll_H&7)|((vramlatch&31)<<3);
		    }
		  else
		    {
		      Vnes.CPUMemory[0x2000]=(Vnes.CPUMemory[0x2000]&0xfc)|((vramlatch>>10)&2);
		    }

		  refreshaddress2000tmp=Vnes.CPUMemory[0x2000]&2;
		}

	      Vnes.var.Reg2006_VRAMAddress=vramlatch&0x3fff;

	      return;
	    }
	  case 0x2007:
	    {
	      if ((Vnes.var.CurrentScanLine<3)||(Vnes.var.CurrentScanLine>=VblankScanLine)||(Vnes.CPUMemory[0x2001]&0x8)==0)
		{
		  WritePPU(Value);
	      
		  if (Vnes.CPUMemory[0x2000]&4) Vnes.var.Reg2006_VRAMAddress+=32;
		  else Vnes.var.Reg2006_VRAMAddress++;
		}
	      else
		{
		  printf("wr PPU mem during refresh scna %d!\n",Vnes.var.CurrentScanLine);
		}
	      return;
	    }
	  default: 
	    {
	      printf("unknown write to  %04X\n",Addr);
	      return;
	    }
	  }
      }
    case 0x4000:  //Registers 0x4000 access  TO DO
      {
	if ((Addr<0x4014)||(Addr==0x4015)) 
	  {
	    APU_Access(Addr,Value);
	    return;
	  }
	switch (Addr)
	  {
	  case 0x4014:   //DMA transfer to SPRram
	    {
	      //	      printf("spr dma on %d\n",Vnes.var.CurrentScanLine);
	      for (tmp=0;tmp<256;tmp++) Vnes.SPRMemory[tmp]=Rd6502( 0x100*((word)(Value)) + tmp);
	      return;
	    }
	  case 0x4016:  //Joypad
	    {
	      if ((Vnes.CPUMemory[0x4016]==1)&&(Value==0) ) Vnes.var.JoyPad1_BitIndex=0;
	      Vnes.CPUMemory[Addr]=Value;
	      return;
	    }
	  case 0x4017:  //Joypad
	    {
	      if ((Vnes.CPUMemory[0x4017]==1)&&(Value==0) ) Vnes.var.JoyPad2_BitIndex=0;
	      Vnes.CPUMemory[Addr]=Value;
	      return;
	    }
	  default:
	    {
	      Vnes.CPUMemory[Addr]=Value;
	      return;
	    }
	  }
      }
    case 0x5000:

      //Expansion modules : dont know about them
      {
	//	printf("wr ext %d\n",Value);
	Vnes.CPUMemory[Addr]=Value;

	if ((Addr>0x5ff7)&&(fileisnsf))  //bank switching
	  {
	    memcpy(Vnes.CPUMemory+0x8000+(Addr&7)*4096,nsfram+Value*4096,4096);
	  }

	return;
      }
    case 0x6000:
    case 0x7000:
      //SRAM
      {
	//printf("Write...sram %d\n",Value);
	Vnes.CPUMemory[Addr]=Value;
	SaveRAM=1;
	return;
      }
    default:
      // PRG-ROM
      switch (Vnes.Mapper_used)
	{
	case 0:
	  {
	    return;
	  }
	case 1: //MMC1
	  {
	    MMC1_Access(Addr,Value);
	    return;
	  }
	  // MMC 2 : UNROM   select the 16k bank at 0x8000
	case 2:
	  {
	    Value=Value%Vnes.ROM_Header.prg_rom_pages_nb;
	    
	    Vnes.CPUPageIndex[4]=Vnes.rom+16+Value*16384;
	    Vnes.CPUPageIndex[5]=Vnes.rom+16+Value*16384+0x2000;
	    return;
	  }
	case 3:
	  {
	    
	    Value=(Value%Vnes.ROM_Header.chr_rom_pages_nb);
	    //	printf("Wr mapper %d\n",Value);
	    for (i=0;i<8;i++) 
	      {
		Vnes.PPUPageIndex[i]=chr_beg+Value*8192+i*1024;
		PPUConvertedMemoryIndex[i]=Value*8192*4+PPUConvertedMemory+i*4096;
	      }
	    return;
	  }
	case 4: //MMC3
	  {
	    
	    MMC3_Access(Addr,Value);
	    return;
	  }
	case 7:
	  {
	    Value&=0x1f;
	    if (Vnes.var.mirroring!=((Value>>4)+3))
	      {
		Vnes.var.mirroring=((Value>>4)+3);
		InitMirroring();
	      }

	    Value=Value%(Vnes.ROM_Header.prg_rom_pages_nb>>1);
	    Vnes.CPUPageIndex[4]=Vnes.rom+16+Value*32768;
	    Vnes.CPUPageIndex[5]=Vnes.CPUPageIndex[4]+8192;
	    Vnes.CPUPageIndex[6]=Vnes.CPUPageIndex[4]+16384;
	    Vnes.CPUPageIndex[7]=Vnes.CPUPageIndex[4]+8192+16384;
	    
	    
	    return;
	  }
	case 9: //MMC2
	  {
	    MMC2_Access(Addr,Value);
	    return;
	  }
	}
    }
}

#ifdef __marat__
byte Rd6502(register word Addr)
#else
byte get6502memory(WORD Addr)
#endif
{	
  int Value;
  int x,y;
  Uint8 bouton;
  int tmp;

  //  printf("rd at %04X\n",Addr);

  switch (Addr&0xF000)
    {
    case 0:
    case 0x1000:
      //RAM access
      {
	return( Vnes.CPUMemory[Addr&0x7FF] );  //Vnes.var.mirroring
      }
    case 0x2000:
    case 0x3000: //Registers 0x2000 access  
	{
	  
	  //	  printf("Rd at %04X\n",Addr);
	  
	  
	  switch (Addr&0x2007)
	    {
	    case 0x2007:
	      {
		if ((Vnes.var.CurrentScanLine>2)&&(Vnes.var.CurrentScanLine<VblankScanLine)&&(Vnes.CPUMemory[0x2001]&8))
		  {
		    //printf("rd 2007 on %d\n",Vnes.var.CurrentScanLine);
		    Vnes.var.SubScanLine=Vnes.var.CurrentScanLine-1;
		    Vnes.var.vscroll=(((vramlatch>>5)&31)<<3)+(((vramlatch>>12)&3));
		    Vnes.var.BGDScroll_H=(Vnes.var.BGDScroll_H&7)|((vramlatch&31)<<3);
		  }
		/*		if (Vnes.var.Reg2006_VRAMAddress>=0x3f00)
				{ 
				lastPPUread=ReadPPU(Vnes.var.Reg2006_VRAMAddress);
				return(lastPPUread);
				}*/
		
		tmp=lastPPUread;  
		lastPPUread=ReadPPU(Vnes.var.Reg2006_VRAMAddress);
		if (Vnes.CPUMemory[0x2000]&4) Vnes.var.Reg2006_VRAMAddress+=32;
		else Vnes.var.Reg2006_VRAMAddress++;
		return(tmp);
	      }
	    default:
	      {

	   //access the 2002 register
		if ((Addr&0x2007)!=0x2002) 
		  {
		    
		    //printf("rd %04X on %d\n",Addr&0x2007,Vnes.var.CurrentScanLine);
		    //		    return(0);
		  }
		tmp=Vnes.CPUMemory[0x2002];
		Vnes.CPUMemory[0x2002]&=0x7f;  //vblank is reseted
		Vnes.var.Reg2005_FirstAccess=1;
		if ((Vnes.var.CurrentScanLine>232/*VblankScanLine*/)&&(!(Vnes.CPUMemory[0x2000]&0x80)))
		  Vnes.CPUMemory[0x2000]&=0xfe;

		return(tmp);
	      }
	    }
	}
      
    case 0x4000:  //Registers 0x4000 access  TO DO
      {
	switch (Addr)
	  {
	    //	  case 0x4015: return(apu_ctrl); 
	  case 0x4016:
	    {
	      Vnes.CPUMemory[0x4016]=((JoyPad1_State()>>Vnes.var.JoyPad1_BitIndex)&1)|0x40;
	      Vnes.var.JoyPad1_BitIndex++;
	      if (Vnes.var.JoyPad1_BitIndex==8) Vnes.var.JoyPad1_BitIndex=0;
	      
	      return(Vnes.CPUMemory[0x4016]);
	    }
	  case 0x4017:
	    {
	      Vnes.CPUMemory[0x4017]=((JoyPad2_State()>>Vnes.var.JoyPad2_BitIndex)&1)|0x40;
	      Vnes.var.JoyPad2_BitIndex++;
	      if (Vnes.var.JoyPad2_BitIndex==8) Vnes.var.JoyPad2_BitIndex=0;

	      if (usezapper)
		{
		  bouton=SDL_GetMouseState(&x,&y);
		  
		  if ((doubleres&&(y>31))||((!doubleres)&&(y>15)))
		    {
		      if (bouton==1) Vnes.CPUMemory[0x4017]|=0x10;

		      if (doubleres)
			tmp=Vbuffer[(((y>>1)-8)<<8)+(x>>1)]&0x3f;
		      else
			tmp=Vbuffer[((y-8)<<8)+x]&0x3f;
		      tmp=(palette[tmp].r+palette[tmp].g+palette[tmp].b)/3;
		      if (tmp<200) Vnes.CPUMemory[0x4017]|=0x8;

		    }
		}
	      



	      return(Vnes.CPUMemory[0x4017]);
	    }
	  default:return(Vnes.CPUMemory[Addr]);
	  }
	
      }
      
      
    case 0x5000:  //Expansion modules : dont know about them
      {
	//	printf("rd ext\n");
	return(Vnes.CPUMemory[Addr]);
      }
    case 0x6000:
    case 0x7000:  //SRAM
      {
	//printf("Read...sram\n");
	return(Vnes.CPUMemory[Addr]);
      }
      
    default:
      //      printf("rd at %04X, returned : %d\n",Addr,Vnes.CPUPageIndex[(Addr>>13)][Addr&0x1fff]);
      return(Vnes.CPUPageIndex[(Addr>>13)][Addr&0x1fff]);
    }
}
