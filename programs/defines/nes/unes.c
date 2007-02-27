/*************************************
  Little John New Generation.
  File : unes.c
  Authors : Yoyo.
  Version : 0.1.1
  Last update : 3rd June 2000
**************************************/

#include "snss.h"

#include "font.h"

#include "unes.h"

#include "unes_ppu.h"
#include "unes_mapper.h"
#include "unes_io.h"
#include "unes_pal.h"
#include "unes_apu.h"

#include "ljng.h"


/***********************************************/
/*      NSF Player variables   */
/**/
byte fileisnsf;
byte nsfver;
byte songnb;
byte songchg;
byte firstsong;
byte cursong;
word load_addr,init_addr,start_addr;
char songname[32],artistname[32],copyrightnsf[32];
word ntscspeed,palspeed;
byte nsfmode;
byte *nsfram;
byte bankswitch;

#define ShadeIT 0x40404040

byte debugmode;

char tvmodeispal;

//SDL_Color normcolors[256],menucolors[256];

void helper();
#ifndef __marat__
byte Loop6502();
#endif

int InitSound();
void InitDisplay();
void InitGUI();


//Error function
void SendErrorMsg(char *errormsg)
{
	printf("\n%s\n",errormsg);
}

void Yo_Printc(Uint16 x,Uint16 y,char c,char s)
{
  int indice=c-32; //index in the font.h array
  int indicex,indicey;
  void *scr=(s?screen2->pixels:screen->pixels);
  Uint16 i,j;
  byte b=(s?(128+32):(128+32));


  if (indice>=96) indice=0;
  if (indice<0) indice=0;
  indicey=(indice>>4)*9;
  indicex=(indice&0xf)<<3;

  if (doubleres)
    {
      for (i=0;i<8;i++)
	for (j=0;j<9;j++) if (font[j+indicey][i+indicex]=='#') ((Uint8 *)(scr))[((y+j)<<9)+i+x]=b;
    }
  else
    {
      for (i=0;i<8;i++)
	for (j=0;j<9;j++) if (font[j+indicey][i+indicex]=='#') ((Uint8 *)(scr))[((y+j)<<8)+i+x]=b;
    }
}

void Message (const char *message,byte b)
{
  int l;
  int k;
  SDL_SetColors(screen, menucolors, 0, 256);
  if (viewsample) return;
  if (screen2)
    {
      l=strlen(message);
      k=0;
      NewMSG=30*(1+(l>>4));
      while (k<l)
	{
	  if (message[k]!='\n') 
	    {
	      Yo_Printc(msg_x,msg_y,message[k],b);
	      msg_x+=6;
	    }
	  else 
	    {
	      msg_y+=8;
	      msg_x=0;
	    }

	  if (msg_x>=(doubleres?512-6:256-6))
	    {
	      msg_x=0;
	      msg_y+=8;
	    }
	  if (msg_y>=(doubleres?480-15:240-8))
	    {
	      msg_x=0;
	      msg_y=0;
	      memset((char *)(screen2->pixels),0,(doubleres?512*480:256*240));
	    }
	  k++;
	}
    }
  else
    {
      printf("%s\n",message);
    }
}

void Init_Sound()
{
  int i,j,k;

  apu_ctrl=0;
  Pulse1_ctrl1=Pulse1_ctrl2=Pulse1_reg3=Pulse1_reg4=Pulse1_vol=Pulse1_env_phase=Pulse1_env_delay=Pulse1_freq=Pulse1_samplepos=Pulse1_sweep_phase=Pulse1_sweep_delay=Pulse1_length=0;
  Pulse2_ctrl1=Pulse2_ctrl2=Pulse2_reg3=Pulse2_reg4=Pulse2_vol=Pulse2_env_phase=Pulse2_env_delay=Pulse2_freq=Pulse2_samplepos=Pulse2_sweep_phase=Pulse2_sweep_delay=Pulse2_length=0;
  
  Pulse3_ctrl1=Pulse3_ctrl2=Pulse3_reg3=Pulse3_reg4=Pulse3_freq=Pulse3_samplepos=Pulse3_length=Pulse3_linear_length=Pulse3_write_latency=Pulse3_cnt_started=0;
  
  Noise_ctrl1=Noise_ctrl2=Noise_reg3=Noise_reg4=Noise_vol=Noise_env_phase=Noise_env_delay=Noise_freq=Noise_samplepos=Noise_length=0;
  
  noise_phaseacc=dpcm_phaseacc=0;
  apu_len2mix=0;  
  buffer_filled=0;
  
  //init the noise buffer
  
  j=0x0011;
  for (i = 0; i < 0x4000; i++)
    {
      k = j & 1;
      j >>= 1;
      k ^= (j & 1);
      j |= k << (13 - 1);
      noise_lut[i] = j;
    }
  //
  
  channel1=channel2=channel3=channel4=channel5=channel6=1;
  
  buffposn=0;
  
  viewsample=0;
  
  dpcm_reg1=dpcm_reg2=dpcm_reg3=dpcm_reg4=dpcm_freq=dpcm_length=dpcm_bits_len=dpcm_irq=dpcm_active=dpcm_address=0;
  
  audio_buffer_pos=0; 
}

char Init_NES(char *RomName)
{
  int i,j,k;
  long l;
  byte b;
  char sramfile[256],result[256];
  FILE *f;

  Init_Sound();


  memset(Vnes.PPUMemory,0,16384);
  memset(Vnes.SPRMemory,0,256);

  if (fileisnsf) return 0;


  memset(Vnes.CPUMemory,0,65536);



  
  //and the mirror, according to the method choosen, 4screen not implemented yet!!!!
  if (Vnes.ROM_Header.rom_ctrl1&1)
    {
      //Vertical Vnes.var.mirroring : name table 0,1 are orginals, 2,3 are mirrors
      Vnes.var.mirroring=1;
      
    }
  else
    {
      //Horizontal Vnes.var.mirroring : name table 0,2 are orginals, 1,3 are mirrors
      Vnes.var.mirroring=2;
    }

  if (Vnes.ROM_Header.rom_ctrl1&8) Vnes.var.mirroring=5;

  if (forcemirror) Vnes.var.mirroring=forcemirror;
  
  //set vram 
  Vnes.var.vrom=0;

  Vnes.CPUPageIndex[0]=Vnes.CPUMemory;
  Vnes.CPUPageIndex[1]=Vnes.CPUMemory+8192;
  Vnes.CPUPageIndex[2]=Vnes.CPUMemory+16384;
  Vnes.CPUPageIndex[3]=Vnes.CPUMemory+16384+8192;
  

  Vnes.PPUPageIndex[12]=Vnes.PPUMemory+0x3000;
  Vnes.PPUPageIndex[13]=Vnes.PPUMemory+0x3400;
  Vnes.PPUPageIndex[14]=Vnes.PPUMemory+0x3800;
  Vnes.PPUPageIndex[15]=Vnes.PPUMemory+0x3C00;


	printf("  * MMC Init...");

	if (forcemapper!=-1)
	  {
	    printf(" forcing to : %d",forcemapper);
	    Vnes.Mapper_used=forcemapper;
	  }

	for (i=0;i<8;i++) PPUConvertedMemoryIndex[i]=PPUConvertedMemory+i*4096;


	MMC_Latch=NULL;
	switch (Vnes.Mapper_used)
	  {
	  case 0:
	    {
	      switch (Vnes.ROM_Header.prg_rom_pages_nb)
		{
		case 1:
		  {
		    Vnes.CPUPageIndex[4]=Vnes.rom+16;
		    Vnes.CPUPageIndex[5]=Vnes.rom+16+8192;
		    Vnes.CPUPageIndex[6]=Vnes.rom+16;
		    Vnes.CPUPageIndex[7]=Vnes.rom+16+8192;
		    break;
		  }
		case 2:
		  {
		    Vnes.CPUPageIndex[4]=Vnes.rom+16;
		    Vnes.CPUPageIndex[5]=Vnes.rom+16+8192;
		    Vnes.CPUPageIndex[6]=Vnes.rom+16+16384;
		    Vnes.CPUPageIndex[7]=Vnes.rom+16+16384+8192;
		    break;
		  }

		default:
		  {
		    printf("no MMC and 0 or >2 pages!  //aborting");
		    Close_ROM(0);
		    return(6);
		  }
		}

	      //no MMC so only 2 Vnes.var.vrom page at max
			
	      switch (Vnes.ROM_Header.chr_rom_pages_nb)
		{
		case 1:
		  {
			
		    for (i=0;i<8;i++)
		      {
			Vnes.PPUPageIndex[i]=Vnes.rom+16+Vnes.ROM_Header.prg_rom_pages_nb*16384+i*1024;
		      }

				
		    Vnes.var.vrom=1;	
		    break;
		  }
		default:
		  {
		    for (i=0;i<8;i++)
		      {
			Vnes.PPUPageIndex[i]=Vnes.PPUMemory+i*1024;
		      }
		  }
			
		}

			

	      break;
	    }
	  case 1: //MMC1
	    {
	      printf(" MMC1 ");
			
	      //if VROM, then swap it
	      if (Vnes.ROM_Header.chr_rom_pages_nb)
		{
		  for (i=0;i<8;i++)
		    {
		      Vnes.PPUPageIndex[i]=chr_beg+i*1024;
		    }
		  Vnes.var.vrom=1;
		}
	      else
		{
		  for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=Vnes.PPUMemory+i*1024;
		}
	      MMC1_Reg[0]=MMC1_Reg[1]=MMC1_Reg[2]=MMC1_Reg[3]=0;
	      MMC1_low=0;
	      MMC1_high=0xf%Vnes.ROM_Header.prg_rom_pages_nb;
	      MMC1_Access(0x8000,0x80);
			
	      break;
	    }
	  case 2:
	    {
	      //copy the first page to 0x8000 and the last to 0xC000 (last cannot be modified : hardwired)
	      Vnes.CPUPageIndex[4]=Vnes.rom+16;
	      Vnes.CPUPageIndex[5]=Vnes.rom+16+8192;
	      Vnes.CPUPageIndex[6]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384;
	      Vnes.CPUPageIndex[7]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384+8192;
	      for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=Vnes.PPUMemory+i*1024;
	      break;
	    }
	  case 3:
	    {
	      switch (Vnes.ROM_Header.prg_rom_pages_nb)
		{
		case 1:
		  {
		    Vnes.CPUPageIndex[4]=Vnes.rom+16;
		    Vnes.CPUPageIndex[5]=Vnes.rom+16+8192;
		    Vnes.CPUPageIndex[6]=Vnes.rom+16;
		    Vnes.CPUPageIndex[7]=Vnes.rom+16+8192;
		    break;
		  }
		case 2:
		  {
		    Vnes.CPUPageIndex[4]=Vnes.rom+16;
		    Vnes.CPUPageIndex[5]=Vnes.rom+16+8192;
		    Vnes.CPUPageIndex[6]=Vnes.rom+16+16384;
		    Vnes.CPUPageIndex[7]=Vnes.rom+16+16384+8192;
		    break;
		  }

		default:
		  {
		    printf("CNROM and 0 or >2 ROM pages!  //aborting");
		    Close_ROM(0);
		    return(6);
		  }
		}
	      Vnes.var.vrom=1;
	      for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=chr_beg+i*1024;
	      break;
	    }
	  case 4:  //MMC3
	    {	
	      Vnes.CPUPageIndex[4]=Vnes.rom+16;
	      Vnes.CPUPageIndex[5]=Vnes.rom+16+8192;
	      Vnes.CPUPageIndex[6]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384;
	      Vnes.CPUPageIndex[7]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384+8192;

			
	      MMC3_ROMPage[0]=Vnes.rom+16;
	      MMC3_ROMPage[1]=Vnes.rom+16+8192;

	      //if VROM, then swap it
	      if (Vnes.ROM_Header.chr_rom_pages_nb)
		{
		  for (i=0;i<8;i++) 
		    {
		      Vnes.PPUPageIndex[i]=chr_beg+i*1024;
		      MMC3_VROMPage[i]=chr_beg+i*1024;
		    }
		  Vnes.var.vrom=1;
				
		}
	      else
		{
		  for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=Vnes.PPUMemory+i*1024;
		}

			
	      break;
	    }
	  case 7:
	    {
	      for (i=0;i<4;i++) Vnes.CPUPageIndex[4+i]=Vnes.rom+16+i*8192;
	      
	      for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=Vnes.PPUMemory+i*1024;

	      break;
	    }
	  case 9: //MMC2
	    MMC2_Latch1_state=0xFD;
	    MMC2_Latch2_state=0xFD;
	    MMC2_Latch1_lo=MMC2_Latch1_hi=0;
	    MMC2_Latch2_lo=MMC2_Latch2_hi=0;
	    
	    MMC_Latch=MMC2_Access_Refresh;
	    Vnes.CPUPageIndex[4]=Vnes.rom+16;
	    Vnes.CPUPageIndex[5]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384-8192;
	    Vnes.CPUPageIndex[6]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384;
	    Vnes.CPUPageIndex[7]=Vnes.rom+16+(Vnes.ROM_Header.prg_rom_pages_nb-1)*16384+8192;
	    Vnes.var.vrom=1;
	    for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=chr_beg+i*1024;
	    break;
	  default:
	    {
	      printf("not supported yet/aborting\n");
	      Close_ROM(0);
	      return(6);
	    }
	  }

	InitMirroring();
	
	printf("ok\n");
	
	

	// Init the CPU
#ifdef __marat__
	Vnes.CPU->IPeriod=cpucycle;	       
	Vnes.CPU->TrapBadOps=debugmode;
	Vnes.CPU->Trace=debugmode;
	Reset6502(Vnes.CPU);
#else
	reset6502();
#endif


	Vnes.var.DrawCframe=1;
	frameskipped=0;
	
	ShowBackSprite=1;
	ShowBackGround=1;
	ShowFrontSprite=1;
	
	usezapper=0;
	
	Vnes.var.Reg2005_FirstAccess=1;
	Vnes.var.Reg2006_VRAMAddress=0;
	Vnes.var.Reg2006_FirstAccess=1;
	Vnes.var.BGDScroll_H=0;
	Vnes.var.BGDScroll_V=0;
	Vnes.var.CurrentScanLine=0;
	
	Vnes.var.SubScanLine=0;
	
	
	Vnes.var.JoyPad1_BitIndex=0;
	Vnes.var.JoyPad2_BitIndex=0;
	
	Vnes.var.hitsprite0=0;
	
	IRQ_Reset=0;
	IRQ_Counter=0;
	IRQ_Counter_Initvalue=0;
	IRQ_Enable=0;
	SaveRAM=Vnes.ROM_Header.rom_ctrl1&2;
	
	framecounter=0;
	skipframe=0;
	
	toggles=0;
	
	refreshaddress2000=refreshaddress2000tmp=0;

	
	printf("ok\n");
	
	//	if (Vnes.ROM_Header.rom_ctrl1&2)
	{
	  //load the sram
	  i=0;
	  j=255;
	  k=-1;
	  while (RomName[i]!=0) 
	    {
	      if (RomName[i]=='.') j=i;
	      if (RomName[i]=='/') k=i;
	      if (RomName[i]=='\\') k=i;
	      i++;
	    }
	  if (j==255) j=i;  //pas d'extension
	  
	  memcpy(sramfile,RomName+k+1,j+1-k-1);
	  sramfile[j+1-k-1]=0;
	  strcat(sramfile,"srm");
	  
	  memcpy(result,path_srm,strlen(path_srm)+1);
	  strcat(result,sramfile);
	  
	  f=fopen(result,"rb");
	  if (f) 
	    {
	      printf("  * SRAM Present, loading data from %s ...",sramfile);
	      fread(&(Vnes.CPUMemory[0x6000]),0x2000,1,f);
	      printf("ok\n");
	      fclose(f);
	      SaveRAM=1;
	    }
	  
	}
	return 0;
}

char Load_ROM(byte *RomName)
{
	FILE *f;
	fpos_t filesize;
	int i,j,k;
	char ext[4];

  	printf(" * Name : %s\n",RomName);

	
	i=strlen(RomName);
	strcpy(ext,RomName+i-3);

	if (!strcasecmp(ext,"ZIP")) return(Load_ZIPROM(RomName));

	f=fopen(RomName,"rb");
	if (!f)
	{
		SendErrorMsg("Could not find romfile\n\n");
		helper();
		Vnes.rom=NULL;
		return(1);
	}
	fseek(f,0,SEEK_END);
	fgetpos(f,&filesize);
	fseek(f,0,SEEK_SET);

	romsize=filesize;


	Vnes.rom=(byte *)malloc(romsize);
	if (!Vnes.rom)
	  {
	    SendErrorMsg("Could not allocate memory for the rom");
	    fclose(f);
	    return(2);
	  }



	if (!fread(Vnes.rom,romsize,1,f))
	  {
	    SendErrorMsg("Error while reading, file corrupted");
	    fclose(f);
	    free(Vnes.rom);
	    return(3);
	  }
	// Now make some verification and initialization
	fclose(f);

}

char Open_ROM()
{
  int i,j,x,y;
  word addr;

  if (Vnes.rom[3]!=0x1a) 
    {//test for a .nsf file
      if ((Vnes.rom[4]!=0x1a)||(!strcmp(Vnes.rom,"NESM")))
	{
	  SendErrorMsg("Rom file corrupted");
	  free(Vnes.rom);
	  return(4);
	}
      else
	{ //file is nsf
	  PPUConvertedMemory=NULL;
	  SaveRAM=0;
	  fileisnsf=1;
	  nsfver=Vnes.rom[5];
	  songnb=Vnes.rom[6];
	  firstsong=Vnes.rom[7];
	  load_addr=Vnes.rom[8]|(Vnes.rom[9]<<8);
	  init_addr=Vnes.rom[10]|(Vnes.rom[11]<<8);
	  start_addr=Vnes.rom[12]|(Vnes.rom[13]<<8);
	  memcpy(songname,Vnes.rom+14,32);
	  memcpy(artistname,Vnes.rom+0x2e,32);
	  memcpy(copyrightnsf,Vnes.rom+0x4e,32);
	  ntscspeed=Vnes.rom[0x6e]|(Vnes.rom[0x6f]<<8);
	  palspeed=Vnes.rom[0x78]|(Vnes.rom[0x79]<<8);
	  nsfmode=Vnes.rom[0x7a];
	  tvmodeispal=nsfmode&1;
	  
	  Vnes.CPUMemory=(byte *)malloc(65536);
	  Vnes.PPUMemory=(byte *)malloc(16384);
	  Vnes.SPRMemory=(byte *)malloc(256);

	  for (i=0;i<8;i++) Vnes.CPUPageIndex[i]=Vnes.CPUMemory+8192*i;
	  for (i=0;i<16;i++) Vnes.PPUPageIndex[i]=Vnes.PPUMemory+1024*i;

	  memset(Vnes.CPUMemory,0,65536);

	  printf("\n  * Init the CPU...");
#ifdef __marat__
	  Vnes.CPU=(M6502 *)malloc(sizeof(M6502));
#else
	  init6502();
#endif

	  for (i=0;i<6;i++) 
	    {
	      audio_buffer[i]=malloc(SndBuffSize);
	      memset(audio_buffer[i],128,SndBuffSize);
	    }

	  audio_buffer_final=malloc(SndBuffSize);
	  memset(audio_buffer_final,128,SndBuffSize);
	  
	  i=0;
	  for (j=0x70;j<=0x78;j++) i+=Vnes.rom[j];

	  if (!i)
	    {
	      //no bank switching
	      memcpy(Vnes.CPUMemory+load_addr,Vnes.rom+0x80,romsize-0x80);
	      bankswitch=0;
	      return 0;
	    }
	  else
	    {
	      addr=load_addr&0xfff;
	      nsfram=(byte *)malloc(romsize-0x80+addr);
	      memcpy(nsfram+addr,Vnes.rom+0x80,romsize-0x80);
	      memcpy(Vnes.CPUMemory+0x5ff8,Vnes.rom+0x70,8);

	      for (i=0;i<8;i++) memcpy(Vnes.CPUMemory+0x8000+i*4096,nsfram+Vnes.CPUMemory[0x5ff8+i]*4096,4096);

	      bankswitch=1;
	      return 0;
	    }
	}
    }
   
  fileisnsf=0;
  Vnes.rom[3]=0;
  if (strcmp(Vnes.rom,"NES"))
    {
      SendErrorMsg("Rom file corrupted");
      free(Vnes.rom);
      return(4);
    }
  
  memcpy(&Vnes.ROM_Header,Vnes.rom,16);
  Vnes.romsize=romsize;


  if (romsize<(16+Vnes.ROM_Header.prg_rom_pages_nb*16384+Vnes.ROM_Header.chr_rom_pages_nb*8192))
    {
      printf(" *!! Warning, romsize is incorrect, ROM MAY NOT RUN or HAVE CORRUPTED GRAPHICS !!\n");
      printf(".. Press [Enter] ...\n");
      getchar();
    }

  if (romsize>(16+Vnes.ROM_Header.prg_rom_pages_nb*16384+Vnes.ROM_Header.chr_rom_pages_nb*8192))
    {
      i=romsize-(16+Vnes.ROM_Header.prg_rom_pages_nb*16384+Vnes.ROM_Header.chr_rom_pages_nb*8192);
      if (i>40) i=40;
      gamename=Vnes.rom+16+Vnes.ROM_Header.prg_rom_pages_nb*16384+Vnes.ROM_Header.chr_rom_pages_nb*8192;
      gamename[i]=0;
      printf("i %d\n",i);
    }
  else gamename=CurrentROMFile;

	//init stuff
	//
	printf("\n[Initialization]\n\n");
	
	printf("  * Reading ROM property :\n");
	if (Vnes.rom[12]==0)
	  //ext mapper
	  Vnes.Mapper_used=(Vnes.rom[6]>>4)+(Vnes.rom[7]&0xf0);
	else
	  Vnes.Mapper_used=(Vnes.rom[6]>>4);//+(Vnes.rom[7]&0xf0);
	    
	printf("     + Mapper used         : %d\n",Vnes.Mapper_used);
	printf("     + ROM 16K pages       : %d\n",Vnes.ROM_Header.prg_rom_pages_nb);
	printf("     + Vnes.var.vrom 8K pages       : %d\n",Vnes.ROM_Header.chr_rom_pages_nb);
	printf("     + Vnes.var.mirroring           : %s\n",(Vnes.ROM_Header.rom_ctrl1&1)?"Vertical":"Horizontal");
	printf("     + SRAM                : %s\n",(Vnes.ROM_Header.rom_ctrl1&2)?"Enabled":"Disabled");
	printf("     + Trainer             : %s\n",(Vnes.ROM_Header.rom_ctrl1&4)?"Present":"Not available");
	printf("     + 4-screen VRAM layout: %s\n",(Vnes.ROM_Header.rom_ctrl1&8)?"Enabled":"Disabled");
	
	if (Vnes.ROM_Header.rom_ctrl1&4)
	  {
	    printf("\n Trainer not supported yet\n");
	    exit(1);
	  }
	
	Vnes.var.cart_size=(romsize-16)/1024;
	printf("\n  * Rom size : %dKB\n",Vnes.var.cart_size);

	printf("\n  * Allocating memory...");
	Vnes.CPUMemory=(byte *)malloc(65536);
	Vnes.PPUMemory=(byte *)malloc(16384);
	Vnes.SPRMemory=(byte *)malloc(256);


	printf("\n  * Init the CPU...");
#ifdef __marat__
	Vnes.CPU=(M6502 *)malloc(sizeof(M6502));
#else
	init6502();
#endif

	for (i=0;i<6;i++) 
	  {
	    audio_buffer[i]=malloc(SndBuffSize);
	    memset(audio_buffer[i],128,SndBuffSize);
	  }
	audio_buffer_final=malloc(SndBuffSize);
	memset(audio_buffer_final,128,SndBuffSize);


	if ( (!Vnes.CPUMemory)||(!Vnes.PPUMemory)||(!Vnes.SPRMemory))
	  {
	    SendErrorMsg("Could not allocate memory for Virtual Nes");
	    return(5);
	  }

	chr_end=Vnes.rom+16+Vnes.ROM_Header.prg_rom_pages_nb*16384+Vnes.ROM_Header.chr_rom_pages_nb*8192; 
	chr_beg=Vnes.rom+16+Vnes.ROM_Header.prg_rom_pages_nb*16384;


	if (Vnes.ROM_Header.chr_rom_pages_nb)
	  {
	    PPUConvertedMemory=(byte *)malloc(Vnes.ROM_Header.chr_rom_pages_nb*8192*4);
	    for (i=0;i<Vnes.ROM_Header.chr_rom_pages_nb;i++)
	      for (j=0;j<512;j++)
		for (y=0;y<8;y++)
		  for (x=0;x<8;x++)
		    PPUConvertedMemory[i*8192*4+j*64+y*8+x]=(((((byte *)(chr_beg))[i*8192+j*16+y])>>(x))&1)|((((((byte *)(chr_beg))[i*8192+j*16+y+8])>>(x))&1)<<1);
	  }
	else
	  {
	    PPUConvertedMemory=(byte *)malloc(2*8192*4);
	    memset(PPUConvertedMemory,0,2*8192*4);
	  }

        


	
	printf("ok\n");



	return(0);
}

char Close_ROM(int b)
{
  FILE *f;
  int i,j,k;
  char sramfile[256],result[256];
  if (b)
    {
      if (SaveRAM)
	{
	  //save the sram
	  i=0;
	  j=255;
	  k=-1;
	  while (CurrentROMFile[i]!=0) 
	    {
	      if (CurrentROMFile[i]=='.') j=i;
	      if (CurrentROMFile[i]=='/') k=i;
	      if (CurrentROMFile[i]=='\\') k=i;
	      i++;
	    }
	  if (j==255) j=i;  //pas d'extension
	  memcpy(sramfile,CurrentROMFile+k+1,j+1-k-1);
	  sramfile[j+1-k-1]=0;
	  strcat(sramfile,"srm");
	  printf("Saving SRAM to %s\n",sramfile);
	  
	  memcpy(result,path_srm,strlen(path_srm)+1);
	  strcat(result,sramfile);
	  f=fopen(result,"wb");
	  if (f) 
	    {
	      fwrite(&(Vnes.CPUMemory[0x6000]),0x2000,1,f);
	      fclose(f);
	    }
	  else printf("!!Cannot write to file!!\n");
	}
    }



  if (Vnes.rom) free(Vnes.rom);
  if (Vnes.CPUMemory) free(Vnes.CPUMemory);
  if (Vnes.PPUMemory) free(Vnes.PPUMemory);
  if (Vnes.SPRMemory) free(Vnes.SPRMemory);
#ifdef __marat__
  if (Vnes.CPU) free(Vnes.CPU);
#endif
  for (i=0;i<6;i++) if (audio_buffer[i]) free(audio_buffer[i]);
  if (audio_buffer_final) free(audio_buffer_final);

  if (PPUConvertedMemory) free(PPUConvertedMemory);

  return(0);
}

void LaunchEmu()
{
  printf("\n*+-\n");
  printf("*m Emulating %s\n",gamename);
  printf("*+-\n\n");

  starttime=0;

	
  if (enablesound) SDL_PauseAudio(0);
#ifdef __marat__
  Run6502(Vnes.CPU);
#else
  while (1)
    {
      exec6502(cpucycle);
      switch (Loop6502())
	{
	case INT_NMI:
	  nmi6502();
	  break;
	case INT_IRQ:
	  irq6502();
	  break;
	case INT_QUIT:
	  return;
	  break;
	}
    }
  
#endif
}

long JoyPad1_State()
{
  long state;
  //  Uint8 *keys;
  Sint16 pos;
  state=0;
  //  keys=SDL_GetKeyState(NULL);

  if (keys[ljng_keysym[A_1]]==SDL_PRESSED) state|=1;         //A
  if (keys[ljng_keysym[B_1]]==SDL_PRESSED) state|=2;		//B
  if (keys[ljng_keysym[SELECT_1]]==SDL_PRESSED) state|=4;	//Select
  if (keys[ljng_keysym[START_1]]==SDL_PRESSED) state|=8;	//Start
  if ((keys[ljng_keysym[UP_1]]==SDL_PRESSED)||(keys[SDLK_o]==SDL_PRESSED)) state|=0x10;		 //up
  if ((keys[ljng_keysym[DOWN_1]]==SDL_PRESSED)||(keys[SDLK_l]==SDL_PRESSED)) state|=0x20;	//down
  if ((keys[ljng_keysym[LEFT_1]]==SDL_PRESSED)||(keys[SDLK_k]==SDL_PRESSED)) state|=0x40;	//left
  if ((keys[ljng_keysym[RIGHT_1]]==SDL_PRESSED)||(keys[SDLK_m]==SDL_PRESSED)) state|=0x80;	//right

  if (UseJoystickPl1)
    {
      SDL_JoystickUpdate();
      pos=SDL_JoystickGetAxis(joy1,0);
      if (pos<-8192) state|=0x40;
      if (pos>8192) state|=0x80;
      pos=SDL_JoystickGetAxis(joy1,1);
      if (pos>8192) state|=0x20;
      if (pos<-8192) state|=0x10;
      switch (joy1nb)
	{
	case 4:
	  if (SDL_JoystickGetButton(joy1,3)) state|=4;
	case 3:
	  if (SDL_JoystickGetButton(joy1,2)) state|=8;
	case 2:
	  if (SDL_JoystickGetButton(joy1,1)) state|=2;
	case 1:
	  if (SDL_JoystickGetButton(joy1,0)) state|=1;
	  
	}
    }
  
  state|=0x10000;			//1player signature
  return(state);

}

long JoyPad2_State()
{
  long state;
  //  Uint8 *keys;
  Sint16 pos;
  state=0;
  //  keys=SDL_GetKeyState(NULL);

  if (keys[ljng_keysym[A_2]]==SDL_PRESSED) state|=1;         //A
  if (keys[ljng_keysym[B_2]]==SDL_PRESSED) state|=2;		//B
  if (keys[ljng_keysym[SELECT_2]]==SDL_PRESSED) state|=4;	//Select
  if (keys[ljng_keysym[START_2]]==SDL_PRESSED) state|=8;	//Start
  if ((keys[ljng_keysym[UP_2]]==SDL_PRESSED)||(keys[SDLK_o]==SDL_PRESSED)) state|=0x10;		 //up
  if ((keys[ljng_keysym[DOWN_2]]==SDL_PRESSED)||(keys[SDLK_l]==SDL_PRESSED)) state|=0x20;	//down
  if ((keys[ljng_keysym[LEFT_2]]==SDL_PRESSED)||(keys[SDLK_k]==SDL_PRESSED)) state|=0x40;	//left
  if ((keys[ljng_keysym[RIGHT_2]]==SDL_PRESSED)||(keys[SDLK_m]==SDL_PRESSED)) state|=0x80;	//right

  if (UseJoystickPl2)
    {
      SDL_JoystickUpdate();
      pos=SDL_JoystickGetAxis(joy2,0);
      if (pos<-8192) state|=0x40;
      if (pos>8192) state|=0x80;
      pos=SDL_JoystickGetAxis(joy2,1);
      if (pos>8192) state|=0x20;
      if (pos<-8192) state|=0x10;
      switch (joy2nb)
	{
	case 4:
	  if (SDL_JoystickGetButton(joy2,3)) state|=4;
	case 3:
	  if (SDL_JoystickGetButton(joy2,2)) state|=8;
	case 2:
	  if (SDL_JoystickGetButton(joy2,1)) state|=2;
	case 1:
	  if (SDL_JoystickGetButton(joy2,0)) state|=1;
	  
	}
    }
  state|=0x20000;			//2player signature
  return(state);
}

void SaveStateSnss(byte b)
{
  FILE *f;
  char savename[255],result[256];
  byte i=0,j=255,k=255,l=255;
  char chaine[256];

  SNSS_RETURN_CODE code;
  SNSS_BLOCK_TYPE blockType;
 
  struct SnssFileHeader fileHeader;
  struct SnssBaseBlock baseBlock;
  struct SnssVramBlock vramBlock;
  struct SnssSramBlock sramBlock;
  struct SnssMapperBlock mapperBlock;
  struct SnssControllersBlock controllersBlock;
  struct SnssSoundBlock soundBlock;

  while (CurrentROMFile[i]) 
    {
      if (CurrentROMFile[i]==92) j=i;  //92="\" 
      if (CurrentROMFile[i]==46) k=i;  //46="."
      if (CurrentROMFile[i]=='/') l=i; 
      i++;
    }
	
  CurrentROMFile[k]=0;
  if (l==255) strcpy(savename,&CurrentROMFile[(byte)(j+1)]);  
  else strcpy(savename,&CurrentROMFile[(byte)(l+1)]);  
  CurrentROMFile[k]=46;
  i=strlen(savename);
  savename[i]=46;
  savename[i+1]='s';
  savename[i+2]='s';
  savename[i+3]=b%10+48;
  savename[i+4]=0;
  sprintf(chaine,"Saving state to file : %s\n",savename);
  Message(chaine,1);

  memcpy(result,path_save,strlen(path_save)+1);
  strcat(result,savename);
	
  f=fopen(result,"wb");
  if (f==NULL)
    {
      Message("Cannot save state\n",1);
      return;
    }

  fileHeader.numberOfBlocks=5;
  //  printf ("Writing SNSS file header\n");
  if ((code = writeFileHeader (&fileHeader, f)) != SNSS_OK)
    {
      printf ("Error: %s\n", getSnssErrorString (code));
      return;
    }



#ifdef __marat__
  baseBlock.regA=Vnes.CPU->A;
  baseBlock.regX=Vnes.CPU->X;
  baseBlock.regY=Vnes.CPU->Y;
  baseBlock.regFlags=Vnes.CPU->P;
  baseBlock.regStack=Vnes.CPU->S;
  baseBlock.regPc=Vnes.CPU->PC.W;
#else
  baseBlock.regA=a_reg;
  baseBlock.regX=x_reg;
  baseBlock.regY=y_reg;
  baseBlock.regFlags=flag_reg;
  baseBlock.regStack=s_reg;
  baseBlock.regPc=pc_reg;
#endif
  baseBlock.reg2000=Vnes.CPUMemory[0x2000];
  baseBlock.reg2001=Vnes.CPUMemory[0x2001];
  baseBlock.vramAddress=Vnes.var.Reg2006_VRAMAddress;
  baseBlock.spriteRamAddress=Vnes.CPUMemory[0x2003];
  //??????
  //	  Vnes.var.BGDScroll_H=baseBlock.tileXOffset&0xff;
  //	  Vnes.var.BGDScroll_V=baseBlock.tileXOffset>>8;
  baseBlock.tileXOffset=0;
  memcpy(baseBlock.cpuRam,Vnes.CPUMemory,0x800);
  memcpy(baseBlock.spriteRam,Vnes.SPRMemory,0x100);
  memcpy(baseBlock.ppuRam,Vnes.PPUMemory+0x2000,0x1000);
  memcpy(baseBlock.palette,Vnes.PPUMemory+0x3F00,0x20);

  for (i=0;i<0x20;i++)
    {
      baseBlock.palette[i]&=0x3f;
    }
	  
  switch (Vnes.var.mirroring)
    {
    case 1:
      baseBlock.mirrorState[0]=0;
      baseBlock.mirrorState[1]=1;
      baseBlock.mirrorState[2]=0;
      baseBlock.mirrorState[3]=1;
      break;
    case 2:
      baseBlock.mirrorState[0]=0;
      baseBlock.mirrorState[1]=0;
      baseBlock.mirrorState[2]=1;
      baseBlock.mirrorState[3]=1;
      break;
    case 3:
      baseBlock.mirrorState[0]=0;
      baseBlock.mirrorState[1]=0;
      baseBlock.mirrorState[2]=0;
      baseBlock.mirrorState[3]=0;
      break;
    case 4:
      baseBlock.mirrorState[0]=1;
      baseBlock.mirrorState[1]=1;
      baseBlock.mirrorState[2]=1;
      baseBlock.mirrorState[3]=1;
      break;
    case 5:
      baseBlock.mirrorState[0]=0;
      baseBlock.mirrorState[1]=1;
      baseBlock.mirrorState[2]=2;
      baseBlock.mirrorState[3]=3;
      break;
    }

  //  printf ("writing Base block\n");	  	  
  if ((code = writeBaseBlock (&baseBlock, f)) != SNSS_OK)
    {
      printf ("Error: %s\n", getSnssErrorString (code));
      return;
    }


  vramBlock.vramSize=0x2000;
  memcpy(vramBlock.vram,Vnes.PPUMemory,vramBlock.vramSize);
  //  printf ("writing VRAM block\n");
  if ((code = writeVramBlock (&vramBlock, f)) != SNSS_OK)
    {
      printf ("Error: %s\n", getSnssErrorString (code));
      return;
    }

  sramBlock.sramSize=0x2000;
  memcpy(sramBlock.sram,Vnes.CPUMemory+0x6000,sramBlock.sramSize);
  //  printf ("writing SRAM block\n");
  if ((code = writeSramBlock (&sramBlock, f)) != SNSS_OK)
    {
      printf ("Error: %s\n", getSnssErrorString (code));
      return;
    }


   for (i = 0; i < 4; i ++)
     {
       mapperBlock.prgPages[i]=(Vnes.CPUPageIndex[i+4]-Vnes.rom-16)/8192;
     }

   for (i = 0; i < 8; i ++)
     {
       mapperBlock.chrPages[i]=(Vnes.PPUPageIndex[i]-chr_beg)/1024;
     }

   switch (Vnes.Mapper_used)
     {
     case 1:
       for (i=0;i<4;i++) mapperBlock.extraData.mapper1.registers[i]=MMC1_Reg[i];
       mapperBlock.extraData.mapper1.latch=MMC1_RegValue;
       mapperBlock.extraData.mapper1.numberOfBits=MMC1_Cnt;
       break;
     case 4:
       mapperBlock.extraData.mapper4.irqCounter=IRQ_Counter;
       mapperBlock.extraData.mapper4.irqLatchCounter=IRQ_Counter_Initvalue;
       mapperBlock.extraData.mapper4.irqCounterEnabled=IRQ_Enable;
       mapperBlock.extraData.mapper4.last8000Write=MMC3_R8000;
       break;
     case 9:
       mapperBlock.extraData.mapper9.latch[0]=MMC2_Latch1_state;
       mapperBlock.extraData.mapper9.latch[1]=MMC2_Latch2_state;
       mapperBlock.extraData.mapper9.lastB000Write=MMC2_Latch1_lo;
       mapperBlock.extraData.mapper9.lastC000Write=MMC2_Latch1_hi;
       mapperBlock.extraData.mapper9.lastD000Write=MMC2_Latch2_lo;
       mapperBlock.extraData.mapper9.lastE000Write=MMC2_Latch2_hi;
       break;
     }

   //   printf ("writing mapper data block\n");
   if ((code = writeMapperBlock (&mapperBlock, f)) != SNSS_OK)
     {
       printf ("Error: %s\n", getSnssErrorString (code));
       return;
     }


   if (enablesound)
     {
       soundBlock.soundRegisters[0]=Pulse1_ctrl1;
       soundBlock.soundRegisters[1]=Pulse1_ctrl2;
       soundBlock.soundRegisters[2]=Pulse1_reg3;
       soundBlock.soundRegisters[3]=Pulse1_reg4;
       soundBlock.soundRegisters[4]=Pulse2_ctrl1;
       soundBlock.soundRegisters[5]=Pulse2_ctrl2;
       soundBlock.soundRegisters[6]=Pulse2_reg3;
       soundBlock.soundRegisters[7]=Pulse2_reg4;
       soundBlock.soundRegisters[8]=Pulse3_ctrl1;
       soundBlock.soundRegisters[9]=Pulse3_ctrl2;
       soundBlock.soundRegisters[0xA]=Pulse3_reg3;
       soundBlock.soundRegisters[0xB]=Pulse3_reg4;
       soundBlock.soundRegisters[0xC]=Noise_ctrl1;
       soundBlock.soundRegisters[0xD]=Noise_ctrl2;
       soundBlock.soundRegisters[0xE]=Noise_reg3;
       soundBlock.soundRegisters[0xF]=Noise_reg4;
       soundBlock.soundRegisters[0x15]=apu_ctrl;
       
       if ((code = writeSoundBlock (&soundBlock, f)) != SNSS_OK)
	 {
	   printf ("Error: %s\n", getSnssErrorString (code));
	   return;
	 }
     }

   /* close the files */
   if ((code = closeSnssFile (f)) != SNSS_OK)
     {
       printf ("Error: %s\n", getSnssErrorString (code));
       return;
     }
}

void LoadStateSTA(byte b)
{
  FILE *f;
  char savename[255],result[256];
  byte i=0,j=255,k=255,l=255;
  int x,y,jj;
  char chaine[256];
  byte mmcstate[32];

  while (CurrentROMFile[i]) 
    {
      if (CurrentROMFile[i]==92) j=i;  //92="\" 
      if (CurrentROMFile[i]==46) k=i;  //46="."
      if (CurrentROMFile[i]=='/') l=i; 
      i++;
    }
	
  CurrentROMFile[k]=0;
  if (l==255) strcpy(savename,&CurrentROMFile[(byte)(j+1)]);  
  else strcpy(savename,&CurrentROMFile[(byte)(l+1)]);  
  CurrentROMFile[k]=46;
  i=strlen(savename);
  savename[i]=46;
  savename[i+1]='s';
  savename[i+2]='t';
  
  if (b)
    savename[i+3]=b%10+48;
  else
    savename[i+3]='a';

  savename[i+4]=0;
      
	
  sprintf(chaine,"Loading NESTICLE state from file : %s\n",savename);
  Message(chaine,1);

  memcpy(result,path_save,strlen(path_save)+1);
  strcat(result,savename);
	
  f=fopen(result,"rb");
  if (f==NULL)
    {
      Message("Cannot load state\n",1);
      return;
    }

  fread(Vnes.CPUMemory+0x6000,0x2000,1,f); //sram
  fread(Vnes.CPUMemory,0x800,1,f);  //ram
#ifdef __marat__
  fread(&Vnes.CPU->PC.W,2,1,f);  //Regs
  fread(&Vnes.CPU->A,1,1,f);
  fread(&Vnes.CPU->P,1,1,f);  
  fread(&Vnes.CPU->X,1,1,f);  
  fread(&Vnes.CPU->Y,1,1,f);  
  fread(&Vnes.CPU->S,1,1,f);  
#else
  fread(&pc_reg,2,1,f);
  fread(&a_reg,1,1,f);
  fread(&flag_reg,1,1,f);
  fread(&x_reg,1,1,f);
  fread(&y_reg,1,1,f);
  fread(&s_reg,1,1,f);
#endif

  fread(Vnes.SPRMemory,0x100,1,f); //sprite ram
  fread(Vnes.PPUMemory,0x2000,1,f); //pattern table
  fread(Vnes.PPUMemory+0x2000,0x1000,1,f); //name/attribute table

  if (!Vnes.var.vrom)
    {
      for (jj=0;jj<512;jj++)
	for (y=0;y<8;y++)
	  for (x=0;x<8;x++)
	    PPUConvertedMemory[jj*64+y*8+x]=(((Vnes.PPUMemory[jj*16+y])>>x)&1)|((((Vnes.PPUMemory[jj*16+y+8])>>x)&1)<<1);
    }
  

  fread(Vnes.PPUMemory+0x3f00,0x10,1,f); //bgnd palette
  fread(Vnes.PPUMemory+0x3f10,0x10,1,f); //sprite palette

  for (i=0;i<0x10;i++)
    {
      Vnes.PPUMemory[0x3f00+i]&=0x3f;
      Vnes.PPUMemory[0x3f10+i]&=0x3f;
    }

  fread(Vnes.CPUMemory+0x2000,0x10,1,f);
  fread(Vnes.CPUMemory+0x4000,0x20,1,f);
  for (i=0;i<0x16;i++) 
    if (i!=0x14) APU_Access(0x4000+i,Vnes.CPUMemory[0x4000+i]);

  fread(mmcstate,0x20,1,f);

  switch (Vnes.Mapper_used)
    {
    case 1:
      for (i=0;i<4;i++) MMC1_Reg[i]=mmcstate[i];
      MMC1_RegValue=MMC1_Cnt=0;
      break;
    case 2:
#ifdef __marat__
      Wr6502(0x9000,mmcstate[0]);
#else
      put6502memory(0x9000,mmcstate[0]);
#endif
      break;
    case 3:
#ifdef __marat__
      Wr6502(0x9000,mmcstate[0]);
#else
      put6502memory(0x9000,mmcstate[0]);
#endif      
      break;
    case 4:
      for (i=0;i<4;i++) Vnes.CPUPageIndex[i]=Vnes.rom+16+(mmcstate[i]*16384);
      if (Vnes.var.vrom)
      for (i=0;i<8;i++) Vnes.PPUPageIndex[i]=chr_beg+(mmcstate[i+4]*1024);
      break;
    case 7:
      break;
    case 9:
      break;
    }

  fseek(f,7,SEEK_CUR);
  fread(&i,1,1,f); //mirroring
  switch (i)
    {
    case 1:
      Vnes.var.mirroring=1;
      break;
    case 0:
      Vnes.var.mirroring=2;
      break;
    case 8:
      Vnes.var.mirroring=5;
    default:
      printf("NEsticle mirroring : %d\n",i);    
    }
  InitMirroring();

  fclose(f);

}

void LoadStateSnss(byte b)
{
  FILE *f;
  char savename[255],result[256];
  byte i=0,j=255,k=255,l=255;
  int x,y,jj;
  char chaine[256];

  SNSS_RETURN_CODE code;
  SNSS_BLOCK_TYPE blockType;
 
  struct SnssFileHeader fileHeader;
  struct SnssBaseBlock baseBlock;
  struct SnssVramBlock vramBlock;
  struct SnssSramBlock sramBlock;
  struct SnssMapperBlock mapperBlock;
  struct SnssControllersBlock controllersBlock;
  struct SnssSoundBlock soundBlock;

  while (CurrentROMFile[i]) 
    {
      if (CurrentROMFile[i]==92) j=i;  //92="\" 
      if (CurrentROMFile[i]==46) k=i;  //46="."
      if (CurrentROMFile[i]=='/') l=i; 
      i++;
    }
	
  CurrentROMFile[k]=0;
  if (l==255) strcpy(savename,&CurrentROMFile[(byte)(j+1)]);  
  else strcpy(savename,&CurrentROMFile[(byte)(l+1)]);  
  CurrentROMFile[k]=46;
  i=strlen(savename);
  savename[i]=46;
  savename[i+1]='s';
  savename[i+2]='s';
  savename[i+3]=b%10+48;
  savename[i+4]=0;
	
  sprintf(chaine,"Loading state from file : %s\n",savename);
  Message(chaine,1);

  memcpy(result,path_save,strlen(path_save)+1);
  strcat(result,savename);
	
  f=fopen(result,"rb");
  if (f==NULL)
    {
      Message("Cannot load state\n",1);
      return;
    }

  //  printf ("reading SNSS file header\n");
  if ((code = readFileHeader (&fileHeader, f)) != SNSS_OK)
    {
      printf ("Error: %s\n", getSnssErrorString (code));
      return;
    }

 /* iterate through each block in the file */
  if (fileHeader.numberOfBlocks>255) {printf("Load overflow\n"); return;}
  for (j = 0; j < fileHeader.numberOfBlocks; j++)
    {
      if ((code = getNextBlockType (&blockType, f)) != SNSS_OK)
        {
          printf ("Error: %s\n", getSnssErrorString (code));
          return;
        }

      switch (blockType)
        {
        case SNSS_BASR:
	  //          printf ("reading base registers block\n");
          if ((code = readBaseBlock (&baseBlock, f)) != SNSS_OK)
            {
              printf ("Error: %s\n", getSnssErrorString (code));
              return;
            }
#ifdef __marat__
	  Vnes.CPU->A=baseBlock.regA;
	  Vnes.CPU->X=baseBlock.regX;
	  Vnes.CPU->Y=baseBlock.regY;
	  Vnes.CPU->P=baseBlock.regFlags;
	  Vnes.CPU->S=baseBlock.regStack;
	  Vnes.CPU->PC.W=baseBlock.regPc;
#else
	   a_reg=baseBlock.regA;
	   x_reg=baseBlock.regX;
	   y_reg=baseBlock.regY;
	   flag_reg=baseBlock.regFlags;
	   s_reg=baseBlock.regStack;
	   pc_reg=baseBlock.regPc;
#endif
	  Vnes.CPUMemory[0x2000]=baseBlock.reg2000;
	  Vnes.CPUMemory[0x2001]=baseBlock.reg2001;
	  Vnes.CPUMemory[0x2006]=Vnes.var.Reg2006_VRAMAddress=baseBlock.vramAddress;
	  Vnes.CPUMemory[0x2003]=baseBlock.spriteRamAddress;
	  //??????
	  //	  Vnes.var.BGDScroll_H=baseBlock.tileXOffset&0xff;
	  //	  Vnes.var.BGDScroll_V=baseBlock.tileXOffset>>8;
	  //	  printf("tile X ofs : %d\n",baseBlock.tileXOffset);


	  memcpy(Vnes.CPUMemory,baseBlock.cpuRam,0x800);
	  memcpy(Vnes.SPRMemory,baseBlock.spriteRam,0x100);
	  memcpy(Vnes.PPUMemory+0x2000,baseBlock.ppuRam,0x1000);
	  memcpy(Vnes.PPUMemory+0x3F00,baseBlock.palette,0x20);

	  for (i=0;i<0x10;i++)
	    {
	      Vnes.PPUMemory[0x3f00+i]&=0x3f;
	      Vnes.PPUMemory[0x3f10+i]&=0x3f;
	    }

	  if ((baseBlock.mirrorState[0]==0)&&(baseBlock.mirrorState[1]==0)&&(baseBlock.mirrorState[2]==0)&&(baseBlock.mirrorState[3]==0)) Vnes.var.mirroring=3;
	  else
	    if ((baseBlock.mirrorState[0]==1)&&(baseBlock.mirrorState[1]==1)&&(baseBlock.mirrorState[2]==1)&&(baseBlock.mirrorState[3]==1)) Vnes.var.mirroring=4;
	    else
	      if ((baseBlock.mirrorState[0]==0)&&(baseBlock.mirrorState[1]==1)&&(baseBlock.mirrorState[2]==0)&&(baseBlock.mirrorState[3]==1)) Vnes.var.mirroring=1;
	      else
		if ((baseBlock.mirrorState[0]==0)&&(baseBlock.mirrorState[1]==0)&&(baseBlock.mirrorState[2]==1)&&(baseBlock.mirrorState[3]==1)) Vnes.var.mirroring=2;
		else
if ((baseBlock.mirrorState[0]==0)&&(baseBlock.mirrorState[1]==1)&&(baseBlock.mirrorState[2]==2)&&(baseBlock.mirrorState[3]==3)) Vnes.var.mirroring=5;
	  InitMirroring();

          break;

        case SNSS_VRAM:
	  //          printf ("reading VRAM block\n");
          if ((code = readVramBlock (&vramBlock, f)) != SNSS_OK)
            {
              printf ("Error: %s\n", getSnssErrorString (code));
              return;
            }

	  memcpy(Vnes.PPUMemory,vramBlock.vram,vramBlock.vramSize);
	  if (!Vnes.var.vrom)
	    {
	      for (jj=0;jj<512;jj++)
		for (y=0;y<8;y++)
		  for (x=0;x<8;x++)
		    PPUConvertedMemory[jj*64+y*8+x]=(((Vnes.PPUMemory[jj*16+y])>>x)&1)|((((Vnes.PPUMemory[jj*16+y+8])>>x)&1)<<1);
	    }


          break;

        case SNSS_SRAM:
	  //          printf ("reading SRAM block\n");
          if ((code = readSramBlock (&sramBlock, f)) != SNSS_OK)
            {
              printf ("Error: %s\n", getSnssErrorString (code));
              return;
            }


	  memcpy(Vnes.CPUMemory+0x6000,sramBlock.sram,sramBlock.sramSize);


          break;

        case SNSS_MPRD:
	  //          printf ("reading mapper data block\n");
          if ((code = readMapperBlock (&mapperBlock, f)) != SNSS_OK)
            {
              printf ("Error: %s\n", getSnssErrorString (code));
              return;
            }
          for (i = 0; i < 4; i ++)
            {
	      Vnes.CPUPageIndex[i+4]=Vnes.rom+16+mapperBlock.prgPages[i]*8192;
            }

	  if (Vnes.ROM_Header.chr_rom_pages_nb)
          for (i = 0; i < 8; i ++)
            {
	      Vnes.PPUPageIndex[i]=chr_beg+mapperBlock.chrPages[i]*1024;
	      PPUConvertedMemoryIndex[i]=PPUConvertedMemory+mapperBlock.chrPages[i]*4096;
	    }

	  switch (Vnes.Mapper_used)
	    {
	    case 1:
	      for (i=0;i<4;i++) MMC1_Reg[i]=mapperBlock.extraData.mapper1.registers[i];
	      MMC1_RegValue=mapperBlock.extraData.mapper1.latch;
	      MMC1_Cnt=mapperBlock.extraData.mapper1.numberOfBits;

	      break;
	    case 4:
	      IRQ_Counter=mapperBlock.extraData.mapper4.irqCounter;
	      IRQ_Counter_Initvalue=mapperBlock.extraData.mapper4.irqLatchCounter;
	      IRQ_Enable=mapperBlock.extraData.mapper4.irqCounterEnabled;
	      MMC3_R8000=mapperBlock.extraData.mapper4.last8000Write;
	      if (MMC3_R8000&0x40)
		{
		  MMC3_ROMPage[1]=Vnes.CPUPageIndex[5];
		  MMC3_ROMPage[0]=Vnes.CPUPageIndex[6];
		}
	      else
		{
		  MMC3_ROMPage[0]=Vnes.CPUPageIndex[4];
		  MMC3_ROMPage[1]=Vnes.CPUPageIndex[5];
		}
	      if (MMC3_R8000&0x80)
		{
		  for (i=0;i<8;i++) MMC3_VROMPage[i^4]=Vnes.PPUPageIndex[i];
		}
	      else
		{
		  for (i=0;i<8;i++) MMC3_VROMPage[i]=Vnes.PPUPageIndex[i];
		  }
	      break;
	    case 9:
	      {
		MMC2_Latch1_state=mapperBlock.extraData.mapper9.latch[0];
		MMC2_Latch2_state=mapperBlock.extraData.mapper9.latch[1];
		MMC2_Latch1_lo=mapperBlock.extraData.mapper9.lastB000Write;
		MMC2_Latch1_hi=mapperBlock.extraData.mapper9.lastC000Write;
		MMC2_Latch2_lo=mapperBlock.extraData.mapper9.lastD000Write;
		MMC2_Latch2_hi=mapperBlock.extraData.mapper9.lastE000Write;
		break;
	      }
	    }

	  printf ("\n");
          break;

        case SNSS_CNTR:
	  //          printf ("reading controllers block\n");
          if ((code = readControllersBlock (&controllersBlock, f)) != 
            SNSS_OK)
            {
              printf ("Error: %s\n", getSnssErrorString (code));
              return;
            }
          printf ("\n");
          break;

        case SNSS_SOUN:
	  //          printf ("reading sound block\n");
          if ((code = readSoundBlock (&soundBlock, f)) != SNSS_OK)
            {
              printf ("Error: %s\n", getSnssErrorString (code));
              return;
            }
          for (i = 0; i < 0x16; i += 2)
            {
	      /*              printf ("$40%02X = %02X, $40%02X = %02X,\n",
                i, soundBlock.soundRegisters[i],
                i + 1, soundBlock.soundRegisters[i + 1]);*/
	      if (i!=0x14) APU_Access(0x4000+i,soundBlock.soundRegisters[i]);
	      APU_Access(0x4000+i+1,soundBlock.soundRegisters[i+1]);
            }
	  
	  printf ("\n");
          break;

        default:
          printf ("invalid block type\n");
          break;
        }
    }

  /* close the files */
  if ((code = closeSnssFile (f)) != SNSS_OK)
    {
      printf ("Error: %s\n", getSnssErrorString (code));
      return;
    }
}

void Reset()
{
  Init_NES(CurrentROMFile);
}

void InlineHelp()
{
  char chaine[256*2];
  viewsample=0;
  toggles=(toggles+1)%4;
  msg_x=0; msg_y=16;
  memset(screen2->pixels,0,(doubleres?512*480:256*240));
  Message("[Little John New Generation 0.1.1]\n",1);
  Message("(c) 2000 Yoyo\n",1);

  if (toggles==1)
    {
      Message("[Active keys]\n",1);
      Message(" F1         -> switch infoscreen\n",1);
      Message(" F9         -> toggle fullscreen\n",1);
      Message(" F10        -> reset\n",1);
      Message(" F11        -> toggle sound\n",1);
      Message(" F12        -> show frame rate\n",1);
      Message(" TAB        -> fast forward\n",1);
      Message(" ESC        -> quit message\n",1);
      Message(" Shft+ESC\n",1);
      Message(" Long ESC   -> exit emulation\n",1);
      Message(" P          -> pause/unpause emulation\n",1);
      Message(" F2-F8      -> load state\n",1);
      Message(" Shft+F2-F8 -> save state\n",1);
      Message(" Ctrl+F2-F8 -> load Nesticle state\n",1);
      Message(" Shft+F12   -> save snapshot\n",1);
      Message(" azerty     -> snd channels\n",1);
      Message(" u          -> toggle zapper mode\n",1);
      Message(" f,g,h      -> show/hide layers\n",1);
      return;
    }
  if (toggles==2)
    {
      Message("[Credits]\n\n",1);
      Message(" --> 6502 CPU Core : Neil Corlett\n",1);
      Message("                     Marat Fayzullin\n",1);
      Message("   * adapted versions *\n\n",1);
      Message(" --> Nes emulation : Yohann Magnien\n",1);
      Message("   * sound info & code heavily inspired\n",1);
      Message("     by Matthew Conte, Nosefart author\n\n",1);
      Message(" --> GUI : Yohann Magnien\n\n\n",1);
      Message(" --> Build 0.1.1, 3rd June 2000\n",1);
      return;
    }
  if (toggles==0)
    {
      sprintf(chaine,"%dx%d 8bpp\n%dHz %dbits %s\n%s buffer %dBytes\n",(doubleres?512:256),(doubleres?480:240),snd_frequency,8<<Sound16bits,(SoundStereo?"stereo":"mono"),(InterPolateSnd?(InterPolateSnd==1?"low filter":"high filter"):"no filter"),buffsize<<(Sound16bits+SoundStereo));
      Message(chaine,1);
      sprintf(chaine,"Emulating : %s\nSize : %dKo\nMapper : %d\n",gamename,Vnes.var.cart_size,Vnes.Mapper_used);
      Message(chaine,1);
      return;
    }
  if (toggles==3)
    {
      Message("\nChannels\n--------> Press ESC to quit\n\n <Square #1>\n\n\n <Square #2>\n\n\n <Triangle>\n\n\n <Noise>\n\n\n <DMC>\n\n\n <Extra>\n",1);
      viewsample=1;

    }
  
}

char *GetSnapName()
{
  static char chaine[256];
  char romname[100];
  byte i,j,k,l;

  strcpy(chaine,path_snap);

  i=0;
  j=k=l=255;
  while (CurrentROMFile[i]) 
    {
      if (CurrentROMFile[i]==92) j=i;  //92="\" 
      if (CurrentROMFile[i]==46) k=i;  //46="."
      if (CurrentROMFile[i]=='/') l=i; 
      i++;
    }
  
  CurrentROMFile[k]=0;
  if (l==255) strcpy(romname,&CurrentROMFile[(byte)(j+1)]);  
  else strcpy(romname,&CurrentROMFile[(byte)(l+1)]);

  if (k=255)
    {
      strcat(chaine,romname);
      strcat(chaine,".bmp");
    }
  else
    {
      romname[k+1]=0;
      strcat(chaine,".bmp");
    }

  return(chaine);
}





Uint8 ProcessKey()
{
  //  Uint8 *keys;
  int x,y;
  byte bouton;
 
  if (LJNG_Process()==0) return(INT_QUIT);
  
  if (keys[ljng_keysym[ACCEL]]==SDL_PRESSED) 
    {
      if (!skipframe)
	{
	  oldfskiprate=fskiprate;		
	  skipframe=1;
	  fskiprate=30;
	}
    }
  else 
    {
      if (skipframe)
	{
	  fskiprate=oldfskiprate;
	  skipframe=0;
	  buffposn=audio_buffer_pos=buffer_filled=0;
	}
    }

  //rajouter le delay avec tamyo
  if (!tamyo)
    {

      if (keys[ljng_keysym[QUIT]]==SDL_PRESSED)
	{
	  SDL_SetColors(screen, normcolors, 0, 256);
	  tamyo=15;
	  viewsample=0;
	  NewMSG=0;
	  memset(screen2->pixels,0,(doubleres?512*480:256*240));
	  if (doubleres)
	    {
	      memset((byte *)(screen->pixels)+512*13,13,512*18);
	    }
	  msg_x=0; msg_y=16;	  
	  return(((keys[ljng_keysym[SHIFTL]]==SDL_PRESSED)||(keys[ljng_keysym[SHIFTR]]==SDL_PRESSED)));
	}


      if ((keys[ljng_keysym[SHIFTL]]==SDL_PRESSED)||(keys[ljng_keysym[SHIFTR]]==SDL_PRESSED))
	{
	  if (keys[SDLK_F2]==SDL_PRESSED) {SaveStateSnss(0);tamyo=15;}
	  if (keys[SDLK_F3]==SDL_PRESSED) {SaveStateSnss(1);tamyo=15;}
	  if (keys[SDLK_F4]==SDL_PRESSED) {SaveStateSnss(2);tamyo=15;}
	  if (keys[SDLK_F5]==SDL_PRESSED) {SaveStateSnss(3);tamyo=15;}
	  if (keys[SDLK_F6]==SDL_PRESSED) {SaveStateSnss(4);tamyo=15;}
	  if (keys[SDLK_F7]==SDL_PRESSED) {SaveStateSnss(5);tamyo=15;}
	  if (keys[SDLK_F8]==SDL_PRESSED) {SaveStateSnss(6);tamyo=15;}
	  if (keys[SDLK_F12]==SDL_PRESSED) {SDL_SaveBMP(screen,GetSnapName()); tamyo=15; Message("Saving snapshot\n",1);}
	}
      else
	{
	  if ((keys[ljng_keysym[CTRLL]]==SDL_PRESSED)||(keys[ljng_keysym[CTRLR]]==SDL_PRESSED))
	    {
	      if (keys[SDLK_F2]==SDL_PRESSED) {LoadStateSTA(0);tamyo=15;}
	      if (keys[SDLK_F3]==SDL_PRESSED) {LoadStateSTA(1);tamyo=15;}
	      if (keys[SDLK_F4]==SDL_PRESSED) {LoadStateSTA(2);tamyo=15;}
	      if (keys[SDLK_F5]==SDL_PRESSED) {LoadStateSTA(3);tamyo=15;}
	      if (keys[SDLK_F6]==SDL_PRESSED) {LoadStateSTA(4);tamyo=15;}
	      if (keys[SDLK_F7]==SDL_PRESSED) {LoadStateSTA(5);tamyo=15;}
	      if (keys[SDLK_F8]==SDL_PRESSED) {LoadStateSTA(6);tamyo=15;}
	    }
	  else
	    {
	      if (keys[SDLK_F2]==SDL_PRESSED) {LoadStateSnss(0);tamyo=15;}
	      if (keys[SDLK_F3]==SDL_PRESSED) {LoadStateSnss(1);tamyo=15;}
	      if (keys[SDLK_F4]==SDL_PRESSED) {LoadStateSnss(2);tamyo=15;}
	      if (keys[SDLK_F5]==SDL_PRESSED) {LoadStateSnss(3);tamyo=15;}
	      if (keys[SDLK_F6]==SDL_PRESSED) {LoadStateSnss(4);tamyo=15;}
	      if (keys[SDLK_F7]==SDL_PRESSED) {LoadStateSnss(5);tamyo=15;}
	      if (keys[SDLK_F8]==SDL_PRESSED) {LoadStateSnss(6);tamyo=15;}
	    }
	}
      if (keys[SDLK_a]==SDL_PRESSED) {channel1^=1; Message((channel1?"Square channel 1 on\n":"Square channel 1 off\n"),1); tamyo=10;}
      if (keys[SDLK_z]==SDL_PRESSED) {channel2^=1; Message((channel2?"Square channel 2 on\n":"Square channel 2 off\n"),1); tamyo=10;}
      if (keys[SDLK_e]==SDL_PRESSED) {channel3^=1; Message((channel3?"Triangle channel on\n":"Triangle channel off\n"),1); tamyo=10;}
      if (keys[SDLK_r]==SDL_PRESSED) {channel4^=1; Message((channel4?"Noise channel on\n":"Noise channel off\n"),1); tamyo=10;}
      if (keys[SDLK_t]==SDL_PRESSED) {channel5^=1; Message((channel5?"DMC channel on\n":"DMC channel off\n"),1); tamyo=10;}
      if (keys[SDLK_y]==SDL_PRESSED) {channel6^=1; Message((channel6?"Extra channel on\n":"Extra channel off\n"),1); tamyo=10;}

      if (keys[SDLK_f]==SDL_PRESSED) {ShowBackGround^=1; tamyo=10; Message((ShowBackGround?"background on\n":"background off\n"),1);}
      if (keys[SDLK_g]==SDL_PRESSED) {ShowBackSprite^=1; tamyo=10;Message((ShowBackSprite?"backsprite on\n":"backsprite off\n"),1);}
      if (keys[SDLK_h]==SDL_PRESSED) {ShowFrontSprite^=1; tamyo=10;Message((ShowFrontSprite?"fontsprite on\n":"frontsprite off\n"),1);}

      if (keys[SDLK_u]==SDL_PRESSED)
	{
	  usezapper^=1;
	  SDL_ShowCursor(usezapper);
	  tamyo=15;
	  Message((usezapper?"Zapper enabled\nWarning, low frame rate causes zapper detection problem\n":"Zapper disabled\n"),1);
	}
      
      if (keys[ljng_keysym[HELP]]==SDL_PRESSED) {InlineHelp();tamyo=15;}
      if (keys[SDLK_F9]==SDL_PRESSED) 
	  { 
		if (SDL_WM_ToggleFullScreen(SDL_GetVideoSurface())) 
			fullscreen^=1; 
			tamyo=15;
	  }
      if (keys[SDLK_F10]==SDL_PRESSED) { Reset(); tamyo=15;}
      if (keys[SDLK_F11]==SDL_PRESSED) 
	{ 
	  tamyo=15;
	  if (!enablesound)
	    {
	      Message("Sound disabled!\n",1);
	    }
	  else
	    {
	      SoundMode=(SoundMode+1)&3;
	      switch (SoundMode)
		{
		case 0:Message("Sound on, raw output\n",1);
		  SDL_PauseAudio(0);
		  InterPolateSnd=0;
		  break;
		case 1:Message("Sound on, low filtered output\n",1);
		  InterPolateSnd=1;
		  break;
		case 2:Message("Sound on, high filtered output\n",1);
		  InterPolateSnd=2;
		  break;
		case 3:Message("Sound off, no output\n",1);
		  SDL_PauseAudio(1);
		  break;
		}
	    }
	}
      if (keys[SDLK_F12]==SDL_PRESSED) { ShowFR^=1;tamyo=15;}

      if (keys[SDLK_p]==SDL_PRESSED) {Message((emupause?"  ** Emulation unpaused **\n":"\n   ** Emulation paused **\n\n     Press P to continue\n\n"),1); 
      emupause^=1; tamyo=15;}

      if (keys[SDLK_KP_PLUS]==SDL_PRESSED)
	{
	  tamyo=15;
	  if (cursong<songnb) cursong++;
	  songchg=1;
	}
      if (keys[SDLK_KP_MINUS]==SDL_PRESSED)
	{
	  tamyo=15;
	  if (cursong>1) cursong--;
	  songchg=1;
	}
    }
  else 
    {
      tamyo--;
    }
  
  if (NewMSG&&(!toggles))
    {
      NewMSG--;
      if (!NewMSG)
	{
	  SDL_SetColors(screen, normcolors, 0, 256);
	  memset(screen2->pixels,0,(doubleres?512*480:256*240));
	  if (doubleres)
	    {
	      memset((byte *)(screen->pixels)+512*13,13,512*18);
	    }
	  msg_x=0; msg_y=16;
	}
    }

  return(0);
}

#ifdef __marat__
byte Loop6502(register M6502 *R)
#else
byte Loop6502()
#endif
{
  int i,j,k;
  byte b,sndok;
  int key;

  framecounter++;

  if (enablesound)
    {
      if (enablesound==2)
	{
	  SDL_PauseAudio(1);
	  enablesound=0;
	}
      else
	{
	  if (skipframe||(SoundMode==3)) Sound_Update(1);
	  else Sound_Update(0);
	}
    }

  if (Vnes.var.CurrentScanLine==0)
    {

      // Emulator test for exit
      if (ProcessKey()) return(INT_QUIT);

      Vnes.CPUMemory[0x2002]&=0x3f;  // OUT VBLANK sprite #0 hit reseted
      Vnes.var.Reg2005_FirstAccess=1;
      // reset the sprite0 stuff
      Vnes.var.spr0ok=1;
      Vnes.var.hitsprite0=0;

      Vnes.var.vscroll=Vnes.var.BGDScroll_V;
      Vnes.var.SubScanLine=0;
      //what?

            
      refreshaddress2000tmp=Vnes.CPUMemory[0x2000]&2;
      //oh ok

      vbl=1;

    }
	

  if (Vnes.var.CurrentScanLine<VblankScanLine)
    {	

      refreshaddress2000=Vnes.CPUMemory[0x2000]&0xfd;  //bit 1 in refreshaddress2000tmp
      refreshaddress2000|=refreshaddress2000tmp;

      refresh(Vnes.var.CurrentScanLine);

      BgndColor=Vnes.CPUMemory[0x2001]>>5;
      //      printf("%d\n",BgndColor);
      switch (BgndColor)
	{
	  case 1:
	    for (i=0,j=Vnes.var.CurrentScanLine<<8;i<256;i++,j++) Vbuffer[j]|=128;
	    break;
	  case 2:
	    for (i=0,j=Vnes.var.CurrentScanLine<<8;i<256;i++,j++) Vbuffer[j]|=64;
	    break;
	  case 4:
	    for (i=0,j=Vnes.var.CurrentScanLine<<8;i<256;i++,j++) Vbuffer[j]|=128+64;
	    break;
	}

      if ((Vnes.var.hitsprite0)&&(Vnes.var.spr0ok))
	{
	  Vnes.var.hitsprite0=0;
	  Vnes.var.spr0ok=0;
	  Vnes.CPUMemory[0x2002]|=0x40;  //sprite 0 hitted	
	}

      Vnes.var.CurrentScanLine++;


      if ((Vnes.CPUMemory[0x2001]&0x8)&&(IRQ_Enable)&&(IRQ_Counter--==0))
	{
	  IRQ_Counter=IRQ_Counter_Initvalue;        
	  //	  printf("IRQ on %d\n",Vnes.var.CurrentScanLine);
	  return(INT_IRQ);
	}


      return(INT_NONE);
    }

  if (Vnes.var.CurrentScanLine==VblankScanLine)
    {
      IRQ_Counter=IRQ_Counter_Initvalue;        

      if (Vnes.var.DrawCframe)
	{ //draw the frame
	  if (doubleres)
	    {
	      for (i=8*256,k=8*512;i<232*256;i+=256,k+=512)
		{
		  for (j=0;j<256;j++)
		    ((word *)(screen->pixels))[k+j]=(((word)(Vbuffer[i|j]))<<8)|Vbuffer[i|j];
		  memcpy((byte *)(screen->pixels)+((k<<1)|512),(byte *)(screen->pixels)+(k<<1),512);
		}
	    }
	  else memcpy(screen->pixels+8*256,Vbuffer+8*256,224*256);
	  if (NewMSG)
	    {
	      if (doubleres)
		for (i=16*512;i<(msg_y)*512;i+=512)
		  for (j=0;j<(512);j++) 
		    {
		      if (i<32*512) 
			k=((((i>>11)+(j>>2))+(framecounter>>10))&7);
		      else 
			k=0;
		      if (((Uint8 *)(screen2->pixels))[i+j]) 
			((Uint8 *)(screen->pixels))[i+j]=32+k;
		      else
			((Uint8 *)(screen->pixels))[i+j]|=128+64;
		    }
	      else
		for (i=16*256;i<(msg_y)*256;i+=256)
		  for (j=0;j<(256);j++) 
		    {
		      if (i<32*256) 
			k=((((i>>10)+(j>>2))+(framecounter>>10))&7);
		      else 
			k=0;
		      if (((Uint8 *)(screen2->pixels))[i+j]) 
			((Uint8 *)(screen->pixels))[i+j]=32+k;
		      else
			((Uint8 *)(screen->pixels))[i+j]|=128+64;
		    }
	    }

	  if ((ShowFR)&&(FRcnt))
	    {
	      if (doubleres)
		{
		  for (i=480-15;i<480;i++)
		    for (j=0;j<20;j++)
		    ((Uint8 *)(screen->pixels))[(i<<9)+j]=13;
		  Yo_Printc(0,480-15,(FRcnt/10)+48,0);
		  Yo_Printc(10,480-15,(FRcnt%10)+48,0);
		}
	      else
		{
		  for (i=240-15;i<240;i++)
		    for (j=0;j<20;j++)
		    ((Uint8 *)(screen->pixels))[(i<<8)+j]=13;
		  Yo_Printc(0,240-15,(FRcnt/10)+48,0);
		  Yo_Printc(10,240-15,(FRcnt%10)+48,0);
		}
	      

	    }

	  if (viewsample&&enablesound)
	    {
	      lastsamplepos=buffposn-1;
	      for (i=0,k=64;i<6;i++,k+=24)
		{
		  b=128;
		  switch (i)
		    {
		    case 0:b+=Pulse1_active<<5; b|=!channel1; break;
		    case 1:b+=Pulse2_active<<5; b|=!channel2; break;
		    case 2:b+=Pulse3_active<<5; b|=!channel3; break;
		    case 3:b+=Noise_active<<5; b|=!channel4; break;
		    case 4:b+=dpcm_active<<5; b|=!channel5; break;
		    case 5:b+=extra_active<<2; b|=!channel6; break;
		    }
		  if (doubleres)
		    for (j=0;j<64;j++)
		      ((byte *)(screen->pixels))[((k+(audio_buffer[i][(lastsamplepos-((j^63)<<2))%SndBuffSize]>>5))<<9)+j+128]=b;
		  else
		    for (j=0;j<64;j++)
		      ((byte *)(screen->pixels))[((k+(audio_buffer[i][(lastsamplepos-((j^63)<<2))%SndBuffSize]>>5))<<8)+j+128]=b;
		}
	    }
	  
	  
	  SDL_UpdateRect(screen, 0, viewGUI<<4, 0, 0);

      if (emupause)
	{
	  if (enablesound) SDL_PauseAudio(1);
	  while (emupause) 
	    {
	      i=SDL_GetTicks();		  
	      ProcessKey();
	      while ((SDL_GetTicks()-i)<(1000/60)) rand();
	    }

	  viewsample=1;
	  while (keys[SDLK_p]==SDL_PRESSED) ProcessKey();
	  viewsample=0;

	  emupause=0;
	  if (enablesound) SDL_PauseAudio(0);
	}
      





	}
      Vnes.CPUMemory[0x2002]|=0x80;  // IN VBLANK

    }

  if (Vnes.var.CurrentScanLine==LastScanLine)
    {
      /* Timing loop */
      i=0;
      if (!skipframe)
	{
	  if (starttime==0) starttime=SDL_GetTicks();
	  else starttime+=(tvmodeispal?(1000/50):(1000/60));
	  
	  //	  j=starttime-SDL_GetTicks();

	  while (SDL_GetTicks()<=starttime) i=1;
	  //	  if (i) printf("%d\n",j);
	}

      if (fskiprate==AUTO_FSKIP)
	{
	  if (i) 
	    {
	      Vnes.var.DrawCframe=1;
	      frameskipped=0;
	    }
	  else
	    {
	      frameskipped++;
	      if (frameskipped>30)  //reset if skipped frames > maxframes
		{
		  Vnes.var.DrawCframe=1;
		  frameskipped=1;
		  starttime=SDL_GetTicks();
		}
	      else
		Vnes.var.DrawCframe=0;

	    }
	}
      else
	{
	  if ((++frameskipped)>=fskiprate) 
	    {
	      Vnes.var.DrawCframe=1;
	      frameskipped=0;
	    }
	  else 
	    Vnes.var.DrawCframe=0;
	}
	
      if (Vnes.var.DrawCframe) FR++;
      frame++;

      if (frame==(tvmodeispal?50:60))
	{
	  FRcnt=FR;
	  FR=0;
	  frame=0;
	}

      Vnes.var.CurrentScanLine=0;

      return(INT_NONE);
    }

  Vnes.var.CurrentScanLine++;

  if ((Vnes.CPUMemory[0x2000]&0x80)&&vbl&&(Vnes.var.CurrentScanLine>=VblankScanLineB))
    {
      vbl=0;
      return(INT_NMI);
    }
  else return(INT_NONE);	
}

int InitSound()
{
  SDL_AudioSpec wanted,obt[1];

  /* Set the audio format */
  wanted.freq = snd_frequency;
  wanted.format = (Sound16bits?AUDIO_S16:AUDIO_U8);
  wanted.channels = (SoundStereo?2:1);    /* 1 = mono, 2 = stereo */
  wanted.samples = buffsize;  /* Good low-latency value for callback */
  wanted.callback = Sound_CallBack;
  wanted.userdata = NULL;

  /* Open the audio device, forcing the desired format */
  if ( SDL_OpenAudio(&wanted, obt) < 0 ) {
    printf("Couldn't open audio: %s\n", SDL_GetError());
    enablesound=0;
    return(-1);
  }


  if (obt)
    {
      printf("Obtained : freq %d format %d channels %d samples %d\n",obt->freq,obt->format,obt->channels,obt->samples);
      buffsize=obt->samples;
      snd_frequency=obt->freq;
      SoundStereo=obt->channels-1;
      Sound16bits=(obt->format==AUDIO_S16);
    }

  apu_buf_size=snd_frequency/(tvmodeispal?50:60);
  apu_inc_size=(3579545.454 / 2) / snd_frequency;
  FrequencyMagicSquare=apu_inc_size*65536*2;
  FrequencyMagicTriangle=FrequencyMagicSquare>>1;
  FrequencyMagicNoise=FrequencyMagicTriangle;



  //  printf("ok\n");
  return(0);
}

void InitDisplay()
{
  int i;

  SDL_Cursor *mycursor;
  Uint8 cursor_data[32]=
  {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
  };
  Uint8 cursor_mask[32]=
  {
    0x01,0xC0,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x80,0x81,
    0xff,0xff,
    0x80,0x81,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x00,0x80,
    0x01,0xC0
  };


  /* Initialize the display in a 8-bit palettized mode */
  bppix=0;

  if (doubleres==0) 
    {
      screen = SDL_SetVideoMode(256, 240, 8, SDL_SWSURFACE|SDL_HWPALETTE|(fullscreen?SDL_FULLSCREEN:0));
      screen2 = SDL_CreateRGBSurface(SDL_SWSURFACE,256, 240, 8, 0,0,0,0);
      pixsize=0;
    }
  else 
    {
      screen = SDL_SetVideoMode(512, 480, 8, SDL_SWSURFACE|SDL_HWPALETTE|(fullscreen?SDL_FULLSCREEN:0));
      screen2 = SDL_CreateRGBSurface(SDL_SWSURFACE,512, 480, 8, 0,0,0,0);
      pixsize=1;
    }
	
	
  if (( screen == NULL )||(screen2 == NULL)) {
    printf("Couldn't set video mode: %s\n",SDL_GetError());
    exit(1);
  }
	 
  //makes the palette
  //menucolors  = (SDL_Color *)malloc(256*sizeof(SDL_Color));
  //normcolors  = (SDL_Color *)malloc(256*sizeof(SDL_Color));

#if FALSE	
  for (i=0;i<64;i++)
    {
      normcolors[i].r=(palette[i].r);
      normcolors[i].g=(palette[i].g);
      normcolors[i].b=(palette[i].b);
      normcolors[i+64].r=(palette[i].r)|255;
      normcolors[i+64].g=(palette[i].g);
      normcolors[i+64].b=(palette[i].b);
      normcolors[i+128].r=(palette[i].r);
      normcolors[i+128].g=(palette[i].g)|255;
      normcolors[i+128].b=(palette[i].b);
      normcolors[i+64+128].r=(palette[i].r);
      normcolors[i+64+128].g=(palette[i].g);
      normcolors[i+64+128].b=(palette[i].b)|255;

      menucolors[i+64+128].r=(palette[i].r)>>2;
      menucolors[i+64+128].g=(palette[i].g)>>2;
      menucolors[i+64+128].b=(palette[i].b)>>2;

      menucolors[i]=normcolors[i];
      menucolors[i+64]=normcolors[i+64];
      menucolors[i+128]=normcolors[i+128];
    }
  
  SDL_SetColors(screen, normcolors, 0, 256);
#endif
	
  Vbuffer=(byte *)malloc(256*240);
  memset(Vbuffer,0,256*240);

  memset(screen->pixels,13,256*240<<pixsize);


  //  SDL_WarpMouse(128,128);
  SDL_ShowCursor(0);
  SDL_WM_SetCaption("Little John New Generation V0.1.1","LJohn");

  for (i=0;i<32;i++) cursor_data[i]=0;

  mycursor=SDL_CreateCursor(cursor_data,cursor_mask,16,16,8,8);
  SDL_SetCursor(mycursor);

  printf("ok\n");

}

void helper()
{
  printf("+- Usage +-------> ljng.exe [romfile] [Options]\n\n");
  printf("[Options]\n");
  printf("          -cpu n    : number of cycle/hline (default : 114)\n");
  printf(" * some games (radracer) need 113 to get ride of gfx bugs *\n");
  printf("          -mapper n : force mapper to mapper #n\n");
  printf("          -mh       : force horizontal mirroring\n");
  printf("          -mv       : force vertical mirroring\n");
  printf("          -m1l      : force one screen low mirroring\n");
  printf("          -m1h      : force one screen high mirroring\n");
  printf("          -m4       : force four screens mirroring\n\n");
  printf("          -dres     : zoom 1:2\n");
  printf("          -nres     : zoom 1:1\n");
  printf("          -full     : select fullscreen mode\n");
  printf("          -win      : select windowed mode\n");
  printf("          -fskip n  : draw 1/n frames (default : AUTO)\n");
  printf("          -nosound  : disable sound\n");
  printf("          -sound    : enable sound\n");
  printf("          -freq n   : select frequency (default : 22050)\n");
  printf("          -buff n   : select buffer length (size is n^2 : 512,1024,...,8192)\n");

  printf("          -lf       : low sound filter\n");
  printf("          -hf       : high sound filter\n");
  printf("          -8        : 8bits sound mode (default)\n");
  printf("          -16       : 16bits sound mode\n");
  printf("          -mono     : mono sound mode (default)\n");
  printf("          -stereo   : stereo sound mode\n\n");
  printf("          -joy1     : enable joystick for player #1\n");
  printf("          -joy2     : enable joystick for player #2\n");
}

void nes_ParseArgs(int argc,char **argv)
{
  int i;
  if (argc>1)
    {
      i=1;
      while (i<argc)
	{
	  
	  if (strcmp("-debug",argv[i])==0) debugmode=1;

	  if (strcmp("-mapper",argv[i])==0)
	    {
	      i++;
	      if (i==argc) helper();
	      forcemapper=atoi(argv[i]);
	    }

	  if (strcmp("-cpu",argv[i])==0)
	    {
	      i++;
	      if (i==argc) helper();
	      cpucycle=atoi(argv[i]);
	    }
			
			
	 
	  

	  if (strcmp("-lf",argv[i])==0) {InterPolateSnd=1; SoundMode=1;}
	  if (strcmp("-hf",argv[i])==0) {InterPolateSnd=2; SoundMode=2;}

	 

	  if (strcmp("-mv",argv[i])==0) forcemirror=1;
	  if (strcmp("-mh",argv[i])==0) forcemirror=2;
	  if (strcmp("-m1l",argv[i])==0) forcemirror=3;
	  if (strcmp("-m1h",argv[i])==0) forcemirror=4;
	  if (strcmp("-m4",argv[i])==0) forcemirror=5;

	  i++;
			
	}
    }


}



void nes_init()
{
  forcemapper=-1;
  cpucycle=114;
  debugmode=0;
  forcemirror=0;
}

byte nes_main (int argc,char *argv[])
{
  char i;
  char chaine[256*2];

  fskiprate=AUTO_FSKIP;  
  InterPolateSnd=0;
  emupause=0;
  SoundMode=0;
  tvmodeispal=0;

  nes_init();
  nes_ParseArgs(argc,argv);

  if ((!Load_ROM(CurrentROMFile))&&(!Open_ROM())&&(!Init_NES(CurrentROMFile)))
    {
      printf("  * Opening display ...");  
      InitDisplay();      
      if (enablesound)
	{
	  printf("  * Opening sound device ...");
	  InitSound();
	  printf("ok\n");
	}

      msg_y=16;
      msg_x=0;

      if (fileisnsf)
	{
#ifdef __marat__
	  printf("NSF player does not work with marat cpu core for now\n");
#else
	  Message("[Little John New Generation 0.1.1]\n",0);
	  Message("(c) 2000 Yoyo\n",0);
	  sprintf(chaine,"Playing : %s\nSize : %dKo\nSong number : %d\nArtist : %s\n\n[ENJOY! ^=^]\n",songname,romsize>>10,songnb,artistname);
	  Message(chaine,0);
	  NewMSG=100;
	  NSFPlay();
	  if (bankswitch) free(nsfram);
#endif

	}
      else
	{
	  Message("[Little John New Generation 0.1.1]\n",1);
	  Message("(c) 2000 Yoyo\n",1);
	  sprintf(chaine,"Emulating : %s\nSize : %dKo\nMapper : %d\nPress F1 for help\n\n[ENJOY! ^=^]\n",gamename,Vnes.var.cart_size,Vnes.Mapper_used);
	  Message(chaine,1);
	  NewMSG=100;		
	  LaunchEmu();
	}
	  Close_ROM(1);
      
	  if (enablesound) {SDL_PauseAudio(1); SDL_CloseAudio();}
	  SDL_FreeSurface(screen2);
	  SDL_FreeSurface(screen);

      //free(normcolors);
      //      free(menucolors);



      return(0);
    }
  else 
    {
      printf("Not a nes file\n");
      return(1);
    }
}
