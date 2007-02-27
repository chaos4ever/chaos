/*
  Little John New Generation
  File : unes_apu.c
  Authors : Yoyo.
  Version : 0.1
  Last update : 12th May 2000
*/

#include "SDL_audio.h"
#include "unes.h"
#include "unes_apu.h"
#include "ljng.h"

extern char tvmodeispal;

unsigned char nes_psg_atl[0x20] = {
  5, 127, 10, 1, 19,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
  6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};
/* frequency limit of square channels */
static const int freq_limit[8] =
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0,
};

/* table of noise frequencies */
static const int noise_freq[16] =
{
   4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 2046
};

/* dpcm transfer freqs */
const int dpcm_clocks[16] =
{
   428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 85, 72, 54
};

/* ratios of pos/neg pulse for square waves */
/* 2/16 = 12.5%, 4/16 = 25%, 8/16 = 50%, 12/16 = 75% */
static const int duty_lut[4] =
{
   2, 4, 8, 12
};


byte pulse_25[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

byte pulse_50[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

byte pulse_75[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

byte pulse_87[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
};

byte *pulse_waves[4] = {
  pulse_87, pulse_75, pulse_50, pulse_25,
};

unsigned char triangle_50[0x20] = {
  0x00, 0x10, 0x20, 0x30,
  0x40, 0x50, 0x60, 0x70,
  0x80, 0x90, 0xa0, 0xb0,
  0xc0, 0xd0, 0xe0, 0xf0,
  0xff, 0xef, 0xdf, 0xcf,
  0xbf, 0xaf, 0x9f, 0x8f,
  0x7f, 0x6f, 0x5f, 0x4f,
  0x3f, 0x2f, 0x1f, 0x0f,
};

void apu_resetdpcm()
{
  dpcm_address=0xC000 + (((word)dpcm_reg3) << 6);
  dpcm_length=(((word)dpcm_reg4)<<4)+1;
  dpcm_bits_len=dpcm_length<<3;
  dpcm_irq=0;
  dpcm_phaseacc=0;
  //  printf("rdpcm addr %04X len %d blen %d freq %d\n",dpcm_address,dpcm_length,dpcm_bits_len,dpcm_clocks[dpcm_reg1&15]);
}

void APU_Access(word Addr,byte Value)
{
    if (!enablesound) return;

    apu_event_log[apu_event_cnt].addr=Addr;
    apu_event_log[apu_event_cnt].cycle=/*114-(Vnes.CPU->IPeriod>=0?Vnes.CPU->IPeriod:0)+*/CurrentCycle;
    apu_event_log[apu_event_cnt++].data=Value;
}

void APU_RegWrite(word Addr,byte Value)
{
  switch (Addr&0x1f)
    {
    case 0:
      Pulse1_ctrl1=Value;
      return;
    case 1:
      Pulse1_ctrl2=Value;
      return;
    case 2:
      Pulse1_reg3=Value;
      if (apu_ctrl&1)
	Pulse1_freq=(((Pulse1_reg4&0x7)<<8)|Pulse1_reg3)+1;
      return;
    case 3:
      Pulse1_reg4=Value;
      if (apu_ctrl&1)
	{
	  Pulse1_freq=(((Pulse1_reg4&7)<<8)|Pulse1_reg3)+1;
	  Pulse1_length=nes_psg_atl[Pulse1_reg4>>3]*apu_buf_size;
	  Pulse1_vol=0;
	}
      return;
    case 4:
      Pulse2_ctrl1=Value;
      return;
    case 5:
      Pulse2_ctrl2=Value;
      return;
    case 6:
      Pulse2_reg3=Value;
      if (apu_ctrl&2)
	Pulse2_freq=(((Pulse2_reg4&0x7)<<8)|Pulse2_reg3)+1;
      return;
    case 7:
      Pulse2_reg4=Value;
      if (apu_ctrl&2)
	{
	  Pulse2_freq=(((Pulse2_reg4&7)<<8)|Pulse2_reg3)+1;
	  Pulse2_length=nes_psg_atl[Pulse2_reg4>>3]*apu_buf_size;
	  Pulse2_vol=0;
	}
      
      return;
    case 8:
      Pulse3_ctrl1=Value;
      if (apu_ctrl&4)
	{
	  if (!Pulse3_cnt_started)
	    Pulse3_linear_length=((Pulse3_ctrl1&0x7)*apu_buf_size)>> 2;
	}
      
      //	  printf("1\n");
      return;
    case 9:
      Pulse3_ctrl2=Value;
      
      //	  printf("2\n");
      return;
    case 0xA:
      Pulse3_reg3=Value;
      
      //	  printf("3\n");
      
      return;
    case 0xB:
      Pulse3_reg4=Value;
      Pulse3_write_latency=3;
      if (apu_ctrl&4)
	{
	  Pulse3_cnt_started=0;
	  Pulse3_length=(nes_psg_atl[Pulse3_reg4>>3]*apu_buf_size) >> 2;
	}
      //	  printf("4\n");
      return;				
      
    case 0x0C:
      Noise_ctrl1=Value;
      return;
    case 0x0D:
      Noise_ctrl2=Value;
      return;
    case 0x0E:
      Noise_reg3=Value;
      return;
    case 0X0F:
      Noise_reg4=Value;
      if (apu_ctrl&8)
	{
	  Noise_length=nes_psg_atl[Noise_reg4>>3]*apu_buf_size;
	  Noise_vol=0;
	}
      return;
    case 0x10:
      dpcm_reg1=Value;
      if (!(Value&0x80))
	dpcm_irq=0;
      return;
    case 0x11:
      dpcm_reg2=Value;
      return;
    case 0x12:
      dpcm_reg3=Value;      
      return;
    case 0x13:
      dpcm_reg4=Value;      
      return;
    case 0x15:
      apu_ctrl=Value;

      if (!(apu_ctrl&1))
	{
	  Pulse1_length=0;
	  Pulse1_active=0;
	}
      else 
	Pulse1_active=1;

      if (!(apu_ctrl&2))
	{
	  Pulse2_length=0;
	  Pulse2_active=0;
	}
      else
	Pulse2_active=1;

      if (!(apu_ctrl&4))
	{
	  Pulse3_length=0;
	  Pulse3_linear_length=0;
	  Pulse3_cnt_started=0;
	  Pulse3_write_latency=0;
	  Pulse3_active=0;
	}
      else
	Pulse3_active=1;

      if (!(apu_ctrl&8))
	{
	  Noise_length=0;
	  Noise_active=0;
	}
      else
	Noise_active=1;

      if (apu_ctrl&0x10)
	{
	  if (!dpcm_active) //DMA finished
	    {
	      dpcm_active=1;
	      apu_resetdpcm();
	    }
	}
      else
	dpcm_active=0;

      dpcm_irq=0;

      //	  printf("ctrl %d\n",ctrl);
      
      return;
    }
}

void FillAudio(Uint8 *stream,int len)
{
  long i,j;
  //  if (WaitSoundUpdate) printf("missed sound synchro!\n");
  //  WaitSoundUpdate=1;

  if ( (i=(buffposn<<(SoundStereo+Sound16bits))-audio_buffer_pos+(SndBuffSize*buffer_filled))<len)
    {
      memset(stream,0,len);
      return;
    }
  if ((audio_buffer_pos+len)<=SndBuffSize)      
    memcpy(stream,audio_buffer_final+audio_buffer_pos,len);
  else
    {
      j=SndBuffSize-audio_buffer_pos;
      memcpy(stream,audio_buffer_final+audio_buffer_pos,j);
      memcpy(stream+j,audio_buffer_final,len-j);
    }

  audio_buffer_pos+=len;
  if (audio_buffer_pos>=SndBuffSize)
    {
      audio_buffer_pos-=SndBuffSize;
      if (buffer_filled) buffer_filled--;
    }

  
}

void Sound_CallBack(void *udata, Uint8 *stream, int len)
{
  if ((len!=(buffsize<<(Sound16bits+SoundStereo)))&&(!badsoundbuff))
    {
      printf("Bad sound buffer size, you should try the -buff n options...\n");
      badsoundbuff=1;
    }
  FillAudio(stream,len);
}



void Sound_Update(int synchro)
{
  byte b;
  int i,l,r;
  int event_cnt,event_pos;
  byte Pulse1_zero,Pulse2_zero,Pulse3_zero,Noise_zero;
  long lo,lb;

  if (synchro)
    {
      audio_buffer_pos=buffer_filled=0;
      buffposn=0;
    }
  else
    {
  
      lo=(buffposn<<(Sound16bits+SoundStereo))-audio_buffer_pos+(SndBuffSize*buffer_filled);
      lb=(buffsize<<(Sound16bits+SoundStereo));

      if (lo>(lb+(lb>>1))) return;  //if we go 50% too far, we wait
    }

  if (tvmodeispal) apu_len2mix+=(float)snd_frequency/50/263;
  else apu_len2mix+=(float)snd_frequency/60/263;


    event_pos=0;
  event_cnt=apu_event_cnt;

  while (apu_len2mix>0)
    { //generate sound wave
      apu_len2mix--;
      //      apu_cycle+=(float)((float)114/(float)snd_frequency)*60*263;


      while (event_cnt/*&&(apu_event_log[event_pos].cycle<=apu_cycle)*/)
	{
	  APU_RegWrite(apu_event_log[event_pos].addr,apu_event_log[event_pos].data);
	  event_pos++;
	  event_cnt--;
	}

      Pulse1_zero=!channel1;
      Pulse2_zero=!channel2;
      Pulse3_zero=!channel3;
      Noise_zero=!channel4;

      if (Pulse1_active)
	{
	  // enveloping
	  Pulse1_env_delay=((Pulse1_ctrl1&0xf)+1)*apu_buf_size;
	  
	  Pulse1_env_phase -= 4;
	  while (Pulse1_env_phase < 0)
	    {
	      Pulse1_env_phase += Pulse1_env_delay;
	      if (Pulse1_ctrl1&0x20)
		Pulse1_vol=(Pulse1_vol + 1)&15;
	      else
		if (Pulse1_vol<15) Pulse1_vol++;
	    }
	  // VBL length counter
	  if (Pulse1_length&&(!(Pulse1_ctrl1&0x20))) Pulse1_length--;
	  
	  if (!Pulse1_length) Pulse1_zero=1;
	  else
	    {
	      //frequency sweeps
	      if ((Pulse1_ctrl2&0x80)&&(Pulse1_ctrl2&7))
		{
		  Pulse1_sweep_delay=(((Pulse1_ctrl2>>4)&7)+1)*apu_buf_size;
		  Pulse1_sweep_phase-=2;
		  while (Pulse1_sweep_phase<0)
		    {
		      Pulse1_sweep_phase+=Pulse1_sweep_delay;
		      if (Pulse1_ctrl2&8)
			Pulse1_freq-=Pulse1_freq >> (Pulse1_ctrl2&7);
		      else
			Pulse1_freq+=Pulse1_freq >> (Pulse1_ctrl2&7);
		    }
		}
	      
	      if (( (!(Pulse1_ctrl2&8))&&(Pulse1_freq > freq_limit[Pulse1_ctrl2&7]))||(Pulse1_freq<4)) Pulse1_zero=1;
	      
	    }
	}
      
      if (Pulse2_active)
	{
	  // enveloping
	  Pulse2_env_delay=((Pulse2_ctrl1&0xf)+1)*apu_buf_size;
	  
	  Pulse2_env_phase -= 4;
	  while (Pulse2_env_phase < 0)
	    {
	      Pulse2_env_phase += Pulse2_env_delay;
	      if (Pulse2_ctrl1&0x20)
		Pulse2_vol=(Pulse2_vol + 1)&15;
	      else
		if (Pulse2_vol<15) Pulse2_vol++;
	    }
	  // VBL length counter
	  if (Pulse2_length&&(!(Pulse2_ctrl1&0x20))) Pulse2_length--;
	  
	  if (!Pulse2_length) Pulse2_zero=1;
	  else
	    {
	      //frequency sweeps
	      if ((Pulse2_ctrl2&0x80)&&(Pulse2_ctrl2&7))
		{
		  Pulse2_sweep_delay=(((Pulse2_ctrl2>>4)&7)+1)*apu_buf_size;
		  Pulse2_sweep_phase-=2;
		  while (Pulse2_sweep_phase<0)
		    {
		      Pulse2_sweep_phase+=Pulse2_sweep_delay;
		      if (Pulse2_ctrl2&8)
			Pulse2_freq-=Pulse2_freq >> (Pulse2_ctrl2&7);
		      else
			Pulse2_freq+=Pulse2_freq >> (Pulse2_ctrl2&7);
		    }
		}
	      
	      if (( (!(Pulse2_ctrl2&8))&&(Pulse2_freq > freq_limit[Pulse2_ctrl2&7]))||(Pulse2_freq<4)) Pulse2_zero=1;
	      
	    }
	}
      
      if (Pulse3_active)
	{
	  if ((!Pulse3_cnt_started)&&(!(Pulse3_ctrl1&0x80)))
	    {
	      if (Pulse3_write_latency) 
		Pulse3_write_latency--;
	      if (!Pulse3_write_latency) 
		Pulse3_cnt_started=1;
	    }
	  if (Pulse3_cnt_started) 
	    {
	      if (Pulse3_linear_length)
		Pulse3_linear_length--;
	      if (Pulse3_length&&(!(Pulse3_ctrl1&0x80))) Pulse3_length--;
	      if (!Pulse3_length) Pulse3_zero=1;
	    }
	  if (Pulse3_active)
	    {
	      if (!Pulse3_linear_length)
		Pulse3_zero=1;
	      else
		{
		  Pulse3_freq=(((Pulse3_reg4&7)<<8)|Pulse3_reg3)+1;
		  if (Pulse3_freq<4) Pulse3_active=0;
		}
	    }
	}
      
      if (Noise_active)
	{
	  // enveloping
	  Noise_env_delay=((Noise_ctrl1&0xf)+1)*apu_buf_size;
	  
	  Noise_env_phase -= 4;
	  while (Noise_env_phase < 0)
	    {
	      Noise_env_phase += Noise_env_delay;
	      if (Noise_ctrl1&0x20)
		Noise_vol=(Noise_vol + 1)&15;
	      else
		if (Noise_vol<15) Noise_vol++;
	    }
	  // VBL length counter
	  if (!(Noise_ctrl1&0x20)) 
	    if (Noise_length) Noise_length--;
	  
	  if (!Noise_length) Noise_zero=1;
	  else Noise_freq=noise_freq[Noise_reg3&0xf];
	}

      if (dpcm_active)
	{
	  dpcm_freq = dpcm_clocks[dpcm_reg1&15];
	  dpcm_phaseacc-=apu_inc_size;
	  while (dpcm_phaseacc<0)
	    {
	      dpcm_phaseacc+=dpcm_freq;
	      if (!dpcm_length)
		{
		  if (dpcm_reg1&0x40)
		    apu_resetdpcm();
		  else
		    {
		      if (dpcm_reg1&0x80)  //irq
			{
			  dpcm_irq=1;
			  //			  printf("dpcm irq\n");
#ifdef __marat__
			  Int6502(Vnes.CPU,INT_IRQ);
#else
			  irq6502();
#endif
			}
		      break;
		    }
		}

	      dpcm_bits_len--;
	      b=7-(dpcm_bits_len&7);
	      if (b==7)
		{
		  dpcm_cur_byte=Rd6502(dpcm_address);
		  dpcm_address++;
		  dpcm_length--;
		}
	      if (dpcm_cur_byte&(1<<b))
		dpcm_reg2+=4;
	      else
		dpcm_reg2-=4;
	    }
	}

      if ((Sint16)dpcm_reg2 > 127) 
	dpcm_reg2=127;
      if ((Sint16)dpcm_reg2 < -128) 
      dpcm_reg2=-128;

      if ((Pulse1_active)&&(!Pulse1_zero))
 	{
	  b=pulse_waves[Pulse1_ctrl1>>6][(Pulse1_samplepos>>16)&0x1f];
	  
	  if (Pulse1_freq) Pulse1_samplepos+=FrequencyMagicSquare/(Pulse1_freq);
	  if (Pulse1_ctrl1&0x10) audio_buffer[0][buffposn]=b*(Pulse1_ctrl1&15);
	  else audio_buffer[0][buffposn]=b*(15-Pulse1_vol);
	} 
      else audio_buffer[0][buffposn]=128;

      if ((Pulse2_active)&&(!Pulse2_zero))
 	{
	  b=pulse_waves[Pulse2_ctrl1>>6][(Pulse2_samplepos>>16)&0x1f];
	  
	  if (Pulse2_freq) Pulse2_samplepos+=FrequencyMagicSquare/(Pulse2_freq);
	  if (Pulse2_ctrl1&0x10) audio_buffer[1][buffposn]=b*(Pulse2_ctrl1&15);
	  else audio_buffer[1][buffposn]=b*(15-Pulse2_vol);
	} 
      else audio_buffer[1][buffposn]=128;

      if ((Pulse3_active)&&(!Pulse3_zero))
 	{
	  b=triangle_50[(Pulse3_samplepos>>16)&0x1f];
	  
	  if (Pulse3_freq) Pulse3_samplepos+=FrequencyMagicTriangle/(Pulse3_freq);
	  audio_buffer[2][buffposn]=b>>1;
	} 
      else audio_buffer[2][buffposn]=128;

      

      if ((Noise_active)&&(!Noise_zero))
	{

	  noise_phaseacc-=apu_inc_size;
	  while (noise_phaseacc<0)
	    {
	      noise_phaseacc+=Noise_freq;
	      Noise_samplepos++;

	      if ((Noise_reg3&0x80)&&(Noise_samplepos==93)) Noise_samplepos=0;
	      else if (Noise_samplepos==0x4000) Noise_samplepos=0;
	    }

	  if (Noise_ctrl1&0x10) b=(Noise_ctrl1&15)<<3;
	  else b=(15-Noise_vol)<<3;

	  if (b>noise_lut[Noise_samplepos]) b=noise_lut[Noise_samplepos];

	  if (noise_lut[Noise_samplepos]&0x80)
	    audio_buffer[3][buffposn]=((char)(-b))^128;
	  else
	    audio_buffer[3][buffposn]=b^128;
	  
	}
      else
	audio_buffer[3][buffposn]=128;

      if (channel5) 
	audio_buffer[4][buffposn]=((Sint8)dpcm_reg2)^128;
      else
	audio_buffer[4][buffposn]=128;




      if (SoundStereo)
	{
	  l=( (word)audio_buffer[0][buffposn]+((word)audio_buffer[1][buffposn]>>1)+((word)audio_buffer[2][buffposn]>>1)+(word)audio_buffer[3][buffposn]+((word)audio_buffer[4][buffposn]>>1))/4;
	  r=( ((word)audio_buffer[0][buffposn]>>1)+(word)audio_buffer[1][buffposn]+(word)audio_buffer[2][buffposn]+((word)audio_buffer[3][buffposn]>>1)+(word)audio_buffer[4][buffposn])/4;
	  r*=MasterVolume;
	  r>>=8;
	}
      else l=( (word)audio_buffer[0][buffposn]+(word)audio_buffer[1][buffposn]+(word)audio_buffer[2][buffposn]+(word)audio_buffer[3][buffposn]+(word)audio_buffer[4][buffposn])/5;

      l*=MasterVolume;
      l>>=8;

      
      if (InterPolateSnd)
	{
	  if (SoundStereo)
	    {
	      next_sampleL=l;
	      next_sampleR=r;
	      l+=prev_sampleL;
	      r+=prev_sampleR;
	      l>>=1;
	      r>>=1;

	      if (InterPolateSnd==1)
		{//soft
		   prev_sampleL=next_sampleL;
		   prev_sampleR=next_sampleR;
		}
	      else
		{//hard
		   prev_sampleL=l;
		   prev_sampleR=r;
		}
	    }
	  else
	    {
	      next_sampleL=l;
	      l+=prev_sampleL;
	      l>>=1;
	      if (InterPolateSnd==1)
		prev_sampleL=next_sampleL;
	      else
		prev_sampleL=l;
	    }
	}


      if (Sound16bits)
	{
	  if (SoundStereo)
	    {
	      ((word *)(audio_buffer_final))[buffposn<<1]=(l<<8)^32768;
	      ((word *)(audio_buffer_final))[(buffposn<<1)|1]=(r<<8)^32768;
	    }
	  else
	    ((word *)(audio_buffer_final))[buffposn]=(l<<8)^32768;
	}
      else
	{
	  if (SoundStereo)
	    {
	      audio_buffer_final[(buffposn<<1)]=l;
	      audio_buffer_final[(buffposn<<1)|1]=r;
	    }
	  else
	    audio_buffer_final[buffposn]=l;
	}

      buffposn++;



      if (buffposn==(SndBuffSize>>(SoundStereo+Sound16bits)))
	{
	  buffposn=0;
	  buffer_filled++;
	  //	  printf("bf %d bp %d abp %d\n",buffer_filled,buffposn,audio_buffer_pos);
	}
    }

  apu_event_cnt=event_cnt;
  
  //  printf("b %d\n",buffposn);
  return;
}

