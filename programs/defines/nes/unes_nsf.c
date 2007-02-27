/*************************************
  Little John New Generation
  File : unes_nsf.c
  Authors : Yoyo.
  Version : 0.1
  Last update : 12th May 2000
**************************************/

#include "unes.h"
#include "ljng.h"
#include "unes_apu.h"

extern byte fileisnsf,nsfver,songnb,firstsong,songchg,cursong;
extern word load_addr,init_addr,start_addr;
extern char songname[32],artistname[32],copyrightnsf[32];
extern word ntscspeed,palspeed;
extern byte nsfmode;
extern byte *nsfram;
extern byte bankswitch;

extern Uint8 ProcessKey();

extern Uint8 *keys;

byte quitnsf;

void NSFPlay()
{
  long synchro,freq,frame,j;
  byte min,sec,b;
  int i,k;
  int timing;
  SDL_ShowCursor(1);

  printf("load %04X init %04X play %04X\n",load_addr,init_addr,start_addr);

  quitnsf=0;
  frame=0;
  if (nsfmode&1) 
    {
      freq=1000000/palspeed/50;
      timing=1000/50;
    }
  else 
    {
      freq=1000000/ntscspeed/60;
      timing=1000/60;
    }

  printf("Freq=%d mode : %s\n",freq,(nsfmode&1?"PAL":"NTSC"));
  if (!freq)
    {

      return;
    }

  //init apu
  for (i=0;i<0x14;i++) APU_RegWrite(0x4000+i,0);
   APU_RegWrite(0x4015,0xf);

   //init the song
   a_reg=firstsong-1;
   x_reg=nsfmode&1;

   Vnes.CPUMemory[0x4100]=0x20;
   Vnes.CPUMemory[0x4101]=init_addr;
   Vnes.CPUMemory[0x4102]=init_addr>>8;
   call6502();

   printf("ok\n");

   cursong=firstsong;
   songchg=0;

   MessageBox(5,100,"\n NSF Player (beta)\n -----------------\n Song : xx/xx \n Use KP +,- to change\n");

   MessageBox(50,200,"Time elapsed 00:00");

   for (i=126;i<134;i++)
   for (j=55;j<87;j++) ((byte *)(screen->pixels))[(i<<(8+pixsize))+j]=23;
   Yo_Printc(55,126,48+(cursong/10),0);
   Yo_Printc(61,126,48+(cursong%10),0);
   Yo_Printc(67,126,'/',0);

   Yo_Printc(73,126,48+(songnb/10),0);
   Yo_Printc(79,126,48+(songnb%10),0);

   //set up play
   Vnes.CPUMemory[0x4101]=start_addr;
   Vnes.CPUMemory[0x4102]=start_addr>>8;

   SDL_UpdateRect(screen,0,0,0,0);
   SDL_PauseAudio(0);
  
   synchro=SDL_GetTicks();
   min=sec=0;

  while (!quitnsf)
    {
      if (ProcessKey()) quitnsf=1;

      if (keys[SDLK_TAB]==SDL_PRESSED) skipframe=1;
      else skipframe=0;
      if (keys[SDLK_ESCAPE]==SDL_PRESSED) quitnsf=1;


      i=0;
      if (!skipframe)
	{
	  while (SDL_GetTicks()-synchro<timing) i=1;      
	  synchro+=timing;
	  if (!i)
	    {
	      synchro=SDL_GetTicks();
	    }
	}   

      if (songchg)
	{
	  songchg=0;
	  a_reg=cursong-1;
	  x_reg=nsfmode&1;
	  for (i=0;i<0x14;i++) APU_RegWrite(0x4000+i,0);
	  APU_RegWrite(0x4015,0xf);
	  memset(Vnes.CPUMemory,0,0x800);

	  // reinit
	  Vnes.CPUMemory[0x4101]=init_addr&0xff;
	  Vnes.CPUMemory[0x4102]=init_addr>>8;
	  call6502();
	  for (i=126;i<134;i++)
	    for (j=55;j<67;j++) ((byte *)(screen->pixels))[(i<<(8+pixsize))+j]=23;
	  Yo_Printc(55,126,48+(cursong/10),0);
	  Yo_Printc(61,126,48+(cursong%10),0);

	  SDL_UpdateRect(screen,55,126,12,10);

	  // restore play
	  Vnes.CPUMemory[0x4101]=start_addr&0xff;
	  Vnes.CPUMemory[0x4102]=start_addr>>8;
	}

      j=0;
      for (i=0;i<freq;i++) 
	{
	  call6502();
	  for (j=0;j<(263/freq);j++) Sound_Update(skipframe);
	}
     
      for (i=201;i<208;i++)
	for (j=128;j<158;j++) ((byte *)(screen->pixels))[(i<<(8+pixsize))+j]=23;
      Yo_Printc(128,201,(min/10)+48,0);
      Yo_Printc(134,201,(min%10)+48,0);
      Yo_Printc(140,201,':',0);
      Yo_Printc(146,201,(sec/10)+48,0);
      Yo_Printc(152,201,(sec%10)+48,0);
      SDL_UpdateRect(screen,128,201,30,8);
      if ((frame%60)==0) 
	{
	  sec++;
	  if (sec==60)
	    {
	      sec=0;
	      min++;
	    }
	}

      for (i=100-8;i<(100+20*6+8);i++)
	memset(screen->pixels+(i<<(8+pixsize))+180,13,256-180);

      for (i=0,k=(100*256<<pixsize)+180;i<6;i++,k+=(20*256<<pixsize))
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
	  for (j=0;j<64;j++)
	    {
	      ((byte *)(screen->pixels))[k+j+((audio_buffer[i][(buffposn-1-(j<<2))%SndBuffSize]>>5)<<(8+pixsize))]=b;
	    }
	}

      SDL_UpdateRect(screen,180,50,256-180,240-50);
      frame++;

    }
  
}
