/*
  Little John, a NES emulator.
  File : unes_ppu.c
  Authors : Yoyo.
  Version : 0.3
  Last update : 3rd Mai 2000
*/
/*
	ReadPPU : read a byte in the PPU meory
*/

#define SpriteMax 16

#include "unes.h"
#include "unes_ppu.h"
#include "unes_mapper.h"

byte ReadPPU(register word Addr)
{
  Addr&=0x3fff;

  if (Vnes.Mapper_used==9) //MMC2
  if (Addr<0x2000) MMC2_Access_Refresh(Addr&0x1000,(Addr>>4)&0xff);


  if (Addr>0x3eff)
    {
      return(Vnes.PPUMemory[Addr&0x3f1f]);
    }
  
  return(Vnes.PPUPageIndex[Addr>>10][Addr&0x3ff]);
}

/*
  WritePPU : write a byte in the PPU memory
*/
void WritePPU(register byte Value)
{
  int Addr=Vnes.var.Reg2006_VRAMAddress&0x3fff;
  int i,j;

  
  if (Vnes.Mapper_used==9) //MMC2
  if (Addr<0x2000) MMC2_Access_Refresh(Addr&0x1000,(Addr>>4)&0xff);
  
  
  if (Addr<0x2000)
    {
      
      if (Vnes.var.vrom) return;			
      else
	{	
	  Vnes.PPUPageIndex[Addr>>10][Addr&0x3ff]=Value;
	  
	  //MAJ de PPUConvertedMemmory
	  if (Addr&8) //bit 1
	    {
	      Addr^=8;
	      i=((Addr&0x1ff0)<<2)|((Addr&7)<<3);
	      for (j=0;j<8;j++) PPUConvertedMemory[i|j]=(PPUConvertedMemory[i|j]&1)|(((Value>>j)&1)<<1);
	    }
	  else  //bit 0
	    {
	      i=((Addr&0x1ff0)<<2)|((Addr&7)<<3);
	      for (j=0;j<8;j++) PPUConvertedMemory[i|j]=(PPUConvertedMemory[i|j]&2)|((Value>>j)&1);
	    }
	  return;
	}
    }
  if (Addr<0x3000)
    {
      //    printf("wr %04X=%02X on scan %d rom1=%d rom2=%d\n",Addr,Value,Vnes.var.CurrentScanLine,MMC1_low,MMC1_high);
      Vnes.PPUPageIndex[Addr>>10][Addr&0x3ff]=Value;
      
      return;
    }
  
  if (Addr<0x3f00)
    {
      Vnes.PPUMemory[Addr]=Value;
      return;
    }
  
  // Palette access : handle Vnes.var.mirroring	
  Value=(Value&0x3f);
  switch (Addr&0x0ef)
    {
    case 0:
      {
	// do the palette mirroring
	Vnes.PPUMemory[0x3f00]=Vnes.PPUMemory[0x3f10]=Value;
	return;
      }
    default:
      {
	Vnes.PPUMemory[Addr&0x3f1f]=Value;
	return;
      }
      
    }
  
}


/*
	refresh : draw a scanline
*/
void refresh(long scanline)
{
  byte a,b,abit,y;
  word i,j,k,col,l,ii;
  long x,xbeg;
  long pos,PPUdep;
  long nametable,bgndpatterntable,SpritePatternTable,oldscanline;
  byte spr_cpt;
  byte _8pix[8];

  // met le scrolling horizontal à jour
  
  nametable=0x2000|((refreshaddress2000&3)<<10);

  if (Vnes.CPUMemory[0x2000]&0x10) bgndpatterntable=0x1000;
  else bgndpatterntable=0x0;
  
  if (Vnes.CPUMemory[0x2000]&0x8) SpritePatternTable=0x1000;
  else SpritePatternTable=0x0;

  //truc tres con  hit sprite #0
  
  if (Vnes.var.spr0ok)
    {
      if (!(Vnes.CPUMemory[0x2000]&0x20))
	{
	  //8x8 sprite
	  
	  if ( (Vnes.SPRMemory[0]+8>=scanline)&&(scanline>Vnes.SPRMemory[0]) )
	    {
	      y=scanline-Vnes.SPRMemory[0]-1;
	      y&=7;
	      if (Vnes.SPRMemory[2]&0x80) y^=7; //vertical flip
	      i=(Vnes.SPRMemory[1]<<4)|SpritePatternTable|(y&7);
	      x=Vnes.PPUPageIndex[i>>10][i&0x3ff]|Vnes.PPUPageIndex[i>>10][(i&0x3ff)|8];
	      if (x) Vnes.var.hitsprite0=1;

	      if (Vnes.Mapper_used==9) 
		{
		  MMC2_Access_Refresh(SpritePatternTable,Vnes.SPRMemory[1]);
		}

	    }
	}
      else
	{
	  //8x16 sprite
	  if ( (Vnes.SPRMemory[0]+16>=scanline)&&(scanline>Vnes.SPRMemory[0]) )
	    {
	      y=scanline-Vnes.SPRMemory[0]-1;
	      y&=15;
	      if (Vnes.SPRMemory[2]&0x80) y^=15; //vertical flip
	      i=(y&7)|( ((Vnes.SPRMemory[1]&0xfe)<<4)+((y&8)<<1) )|( (Vnes.SPRMemory[1]&1)<<12);  //if tile# is odd, 0x0000, else 0x1000
	      x=Vnes.PPUPageIndex[i>>10][i&0x3ff]|Vnes.PPUPageIndex[i>>10][(i&0x3ff)|8];
	      if (x) Vnes.var.hitsprite0=1;
	    }
	}
    }
  
  //info for the scrolling
  // layout is like this :
  //	Name table number :
  /*
    #2      #3
    0x2800  0x2c00
    #0      #1
    0x2000  0x2400
    
    Horizontal :  0....512    value : 0..255
    A  |  B
    
    Vertical :    0    A      value : 0..239  (if > then 0 ???(good??)
    |    -
    480   B
  */
  
  //  if (/*(scanline<MinScanLine)||*/(scanline>=MaxScanLine)) return;
  if (scanline>239) return;

  memset(Vbuffer+(scanline<<8),Vnes.PPUMemory[0x3f00],256);
		  
  if (!Vnes.var.DrawCframe) return;
  
  Vnes.var.hscroll=Vnes.var.BGDScroll_H;
  spr_cpt=0;
  
  // met la palette à jour	
  a=Vnes.PPUMemory[0x3f00];
  Vnes.PPUMemory[0x3f04]=a;
  Vnes.PPUMemory[0x3f08]=a;
  Vnes.PPUMemory[0x3f0c]=a;
  Vnes.PPUMemory[0x3f14]=a;
  Vnes.PPUMemory[0x3f18]=a;
  Vnes.PPUMemory[0x3f1c]=a;
  

  if (Vnes.CPUMemory[0x2001]&0x10)
    {      // search the sprite to draw
      
      Vnes.CPUMemory[0x2002]&=0xdf; //reset the sprite counter
      memset(Spr2draw,0,SpriteMax);
      SprCnt=0;
      for (i=63*4;i<64*4;i-=4)
	if (Vnes.CPUMemory[0x2000]&0x20) 
	  {
	    
	    //8x16
	    if ((Vnes.SPRMemory[i]<scanline)&&((Vnes.SPRMemory[i]+16)>=scanline)) 
	      {

		k=(Vnes.SPRMemory[i|2]&0x20)>>4;

		Spr2draw[SprCnt++]=i|k; //number*4 | priority
		if (SprCnt==SpriteMax) {Vnes.CPUMemory[0x2002]|=0x20; break;}
		
	      }
	  }
	else
	  {
	    
	    //8x8
	    if ((Vnes.SPRMemory[i]<scanline)&&((Vnes.SPRMemory[i]+8)>=scanline)) 
	      {

		k=(Vnes.SPRMemory[i|2]&0x20)>>4;
		
		Spr2draw[SprCnt++]=i|k; //number*4 | priority
		if (SprCnt==SpriteMax) {Vnes.CPUMemory[0x2002]|=0x20; break;}
		
	      }
	  }
      //sort sprites : if 2 are overlapping use tile number to determine priority 
      //seems incorrect with smb2 so disabled
      for (i=0;i<SprCnt;i++)
	if (!(Spr2draw[i]&2))   //sprite n°i in front
	for (j=(i+1);j<SprCnt;j++)
	  if (Spr2draw[j]&2)   //sprite n°j in back
	  {
	    x=Vnes.SPRMemory[(Spr2draw[i]&0xfc)|3]-Vnes.SPRMemory[(Spr2draw[j]&0xfc)|3];
	    if ((x>-8)&&(x<8))
	      {
		//2 sprites overlapping, go behind background
		Spr2draw[i]|=2;
		break;
		
		/* 		if ((Vnes.SPRMemory[(Spr2draw[i]&0xfc)|1])>(Vnes.SPRMemory[(Spr2draw[j]&0xfc)|1]))
		  {
		    k=Spr2draw[j];
		    Spr2draw[j]=Spr2draw[i];
		    Spr2draw[i]=k;
		    }*/
	      }

	  }
      
      
      //show sprite that are behind the background (full or partially)
      if (ShowBackSprite)
	{
	  for (ii=0;ii<SprCnt;ii++)
	    if (Spr2draw[ii]&2)
	      {
		i=Spr2draw[ii]&0xfc;
		if (Vnes.CPUMemory[0x2000]&0x20)
		  {
				//8x16 sprite
		    
		    
		    pos=(scanline<<8)|(Vnes.SPRMemory[i|3]);
		    a=(Vnes.SPRMemory[i|2]&3)<<2;
		    l=Vnes.SPRMemory[i|1];
		    b=scanline-Vnes.SPRMemory[i]-1;
		    
		    if (Vnes.SPRMemory[i|2]&0x80) b^=15;  //vertical flip

		    if (Vnes.Mapper_used==9) 
		      {
			//			MMC2_Access_Refresh(l&1,(l&0xfe)+(b>7));
			//MMC2_Access_Refresh(l&1,((l&0xfe)+(b>7))+1);
		      }



		    
		    j=((l&1)<<12) | ( ((l&0xfe)<<4) + ((b&8)<<1) ) | (b&7);
				// spritetable  tile address    for the down part      y pos
		    
		    y=Vnes.SPRMemory[i|3];
		    
		    if ((!(Vnes.CPUMemory[0x2001]&4))&&(Vnes.SPRMemory[i|3]<8)) 
		      {
			xbeg=8-Vnes.SPRMemory[i|3];
			pos&=0xff00;
			pos|=8;
			if (Vnes.SPRMemory[i|2]&0x40)
			  {	
			    for (x=xbeg;x<8;x++)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
			      }
			  }
			else
			  {
			    xbeg=7-xbeg;
			    for (x=xbeg;x>=0;x--)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
			      }
			  }
		      }
		    else
		      {
			if (Vnes.SPRMemory[i|2]&0x40)
			  {
			    for (x=0;x<8;x++)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
				if (!(pos&0xff)) break;
			      }
			  }
			else
			  {
			    for (x=7;x>=0;x--)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
				if (!(pos&0xff)) break;
			      }
			  }
		      }

		  }
		else
		  {	//8x8 sprites
		    pos=(scanline<<8)|(Vnes.SPRMemory[i|3]);
		    a=(Vnes.SPRMemory[i|2]&3)<<2;
		    l=Vnes.SPRMemory[i|1];
		    b=scanline-Vnes.SPRMemory[i]-1;
		    
		    if (Vnes.SPRMemory[i|2]&0x80) b^=7;  //vertical flip
		    
		    j=SpritePatternTable|(l<<4)|(b&7);

		    if (Vnes.Mapper_used==9)
		      {
			//			MMC2_Access_Refresh(SpritePatternTable,l);
			//MMC2_Access_Refresh(SpritePatternTable,l+1);
		      }


		    
		    y=Vnes.SPRMemory[i|3];

		    
		    if ((!(Vnes.CPUMemory[0x2001]&4))&&(Vnes.SPRMemory[i|3]<8) )
		      { //draw
			xbeg=8-Vnes.SPRMemory[i|3];
			pos&=0xff00;
			pos|=8;
			if (Vnes.SPRMemory[i|2]&0x40)
			  {	
			    for (x=xbeg;x<8;x++)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
			      }
			  }
			else
			  {
			    xbeg=7-xbeg;
			    for (x=xbeg;x>=0;x--)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
			      }
			  }
		      }
		    else
		      { //drawe
			if (Vnes.SPRMemory[i|2]&0x40)
			  {
			    for (x=0;x<8;x++)
			      {		
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];
				if (!(pos&0xff)) break;
			      }
			  }
			else
			  {
			    for (x=7;x>=0;x--)
			      {
				col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;			
				*(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];
				if (!(pos&0xff)) break;
				
			      }
			    
			  }
		      }

		  }
	      }
	}
    }


  //show the tiled-background
  if ((ShowBackGround )&&(Vnes.CPUMemory[0x2001]&8))
    {
      oldscanline=scanline;
      pos=scanline<<8;
      scanline-=Vnes.var.SubScanLine;


      if (Vnes.var.vscroll>239)
	{ 
	  scanline-=(Vnes.var.vscroll-240);
	  scanline+=16;
	  if (scanline<0)
	    {			
	      scanline+=240;
	      nametable^=0x800;
	    } 
	} 
      else scanline+=Vnes.var.vscroll;


      if (scanline>=240)
	{
	  scanline-=240;
	  nametable^=0x800;
	}
      
      j=7-(Vnes.var.hscroll&7); //bit index in the tile
      y=(scanline&7);  //y index in the tile
      
      abit=((Vnes.var.hscroll>>3)&2); 
      if ((scanline&31)>15) abit|=4;
      
      i=((scanline>>3)<<5)+(Vnes.var.hscroll>>3); //pos in the name table	
      a=((scanline>>5)<<3)+(Vnes.var.hscroll>>5);
      
      k=( (Vnes.PPUPageIndex[(i + nametable)>>10][(i+nametable)&0x3ff]<<6) | (y<<3) ) + (bgndpatterntable<<2);  // k=address of tile byte;
      b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;

      if (Vnes.Mapper_used==9) 
	{
	  MMC2_Access_Refresh(bgndpatterntable,Vnes.PPUPageIndex[(i + nametable)>>10][(i + nametable)&0x3ff]);
	}
      
      
      if (!(Vnes.CPUMemory[0x2001]&2))
	{
	  memset((  ((byte *)Vbuffer) + pos ),Vnes.PPUMemory[0x3f00],8);
	  for (x=0;x<8;x++)
	    {		//dx
	      if (!j)  //last bit readen
		{
		  i++;

		  if (Vnes.Mapper_used==9) 
		    {
		      MMC2_Access_Refresh(bgndpatterntable,Vnes.PPUPageIndex[(i + nametable)>>10][(i + nametable)&0x3ff]);
		    }



		  
		  if ( (Vnes.var.hscroll+x)==255 ) 
		    {
		      nametable^=0x400; //change the nametable (horizontal)
		      i=((scanline>>3)<<5);	
		      a=(scanline>>5)<<3;
		      if ((scanline&31)>15) abit=4;
		      else abit=0;
		      
		      b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;
		    }
		  else
		    {
		      if ( ((x+Vnes.var.hscroll)&15)==15 )
			{
			  if (abit&2)
			    {
			      abit^=2;
			      a++;
			    }
			  else abit|=2;
			  b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;
			}
		    }


		  k=( (Vnes.PPUPageIndex[(i + nametable)>>10][(i + nametable)&0x3ff]<<6) | (y<<3) ) | (bgndpatterntable<<2);  // k=address of tile byte;

		  j=7;	
		}
	      else j--;
	      
	    }
	  xbeg=8;
	  pos|=8;
	}
      else xbeg=0;

      x=xbeg;
      do
	{
	  col= PPUConvertedMemoryIndex[k>>12][(k|j)&0xfff]|b;
	  if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col|0x3f00];	
	  else pos++;
	  x++;

	  if (!j)
	    {//!j

	      i++;
	      if (Vnes.Mapper_used==9) 
		{
		  MMC2_Access_Refresh(bgndpatterntable,Vnes.PPUPageIndex[(i + nametable)>>10][(i + nametable)&0x3ff]);
		}

	      if ( (x+Vnes.var.hscroll)==256)
		{
		  nametable^=0x400; //change the nametable (horizontal)
		  i=((scanline>>3)<<5);	
		  a=(scanline>>5)<<3;
		  if ((scanline&31)>15) abit=4;
		  else abit=0;
		  
		  b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;
		}
	      else
		{
		  if ( ((x+Vnes.var.hscroll)&15)==0 )
		    {
		      if (abit&2)
			{
			  abit^=2;
			  a++;
			}
		      else abit|=2;
		      b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;
		    }
		}

	      k=( (Vnes.PPUPageIndex[(i + nametable)>>10][(i + nametable)&0x3ff]<<6) | (y<<3) )|(bgndpatterntable<<2);  // k=address of tile byte;

	    }
	  j--;
	    
	}
      while (j!=65535);

      
      while (x<256)
	{
	  memcpy(_8pix,&(PPUConvertedMemoryIndex[k>>12][k&0xfff]),8);
	  
	  if (x<256-8)
	    {
	      if (_8pix[7]) *(  ((byte *)Vbuffer) + pos )=Vnes.PPUMemory[_8pix[7]|b|0x3f00];
	      if (_8pix[6]) *(  ((byte *)Vbuffer) + pos +1)=Vnes.PPUMemory[_8pix[6]|b|0x3f00];
	      if (_8pix[5]) *(  ((byte *)Vbuffer) + pos +2)=Vnes.PPUMemory[_8pix[5]|b|0x3f00];
	      if (_8pix[4]) *(  ((byte *)Vbuffer) + pos +3)=Vnes.PPUMemory[_8pix[4]|b|0x3f00];
	      if (_8pix[3]) *(  ((byte *)Vbuffer) + pos +4)=Vnes.PPUMemory[_8pix[3]|b|0x3f00];
	      if (_8pix[2]) *(  ((byte *)Vbuffer) + pos +5)=Vnes.PPUMemory[_8pix[2]|b|0x3f00];
	      if (_8pix[1]) *(  ((byte *)Vbuffer) + pos +6)=Vnes.PPUMemory[_8pix[1]|b|0x3f00];
	      if (_8pix[0]) *(  ((byte *)Vbuffer) + pos +7)=Vnes.PPUMemory[_8pix[0]|b|0x3f00];
	      x+=8;
	      pos+=8;
	    }
	  else
	    {
	      i=256-x;
	      j=7;
	      while (i)
		{
		  if (_8pix[j])  *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[_8pix[j]|b|0x3f00];
		  else pos++;
		  x++;
		  j--;
		  i--;
		}
	      break;
	    }

	  i++;
	  if (Vnes.Mapper_used==9) 
	    {
	      MMC2_Access_Refresh(bgndpatterntable,Vnes.PPUPageIndex[(i + nametable)>>10][(i+nametable)&0x3ff]);
	    }
	  

	  if ( (x+Vnes.var.hscroll)==256)
	    {
	      nametable^=0x400; //change the nametable (horizontal)
	      i=((scanline>>3)<<5);	
	      a=(scanline>>5)<<3;
	      if ((scanline&31)>15) abit=4;
	      else abit=0;
	      
	      b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;
	    }
	  else
	    {
	      if ( ((x+Vnes.var.hscroll)&15)==0 )
		{
		  if (abit&2)
		    {
		      abit^=2;
		      a++;
		    }
		  else abit|=2;
		  b= ((Vnes.PPUPageIndex[(nametable+0x3c0+a)>>10][(nametable+0x3c0+a)&0x3ff]>>abit)&3)<<2;
		}
	    }


	  k=( (Vnes.PPUPageIndex[(i + nametable)>>10][(i + nametable)&0x3ff]<<6) | (y<<3) )|(bgndpatterntable<<2);  // k=address of tile byte;

	}
	              
      scanline=oldscanline;
      
    }

	
	
	if ((ShowFrontSprite)&&(Vnes.CPUMemory[0x2001]&0x10))
	//show sprite that are in front of the background
	{
		for (ii=0;ii<SprCnt;ii++) //i unsigned, so i<0 => i>64*4
		  if (!(Spr2draw[ii]&2))
		{
			i=Spr2draw[ii]&0xfc;
			if (Vnes.CPUMemory[0x2000]&0x20)
			  {
			
				pos=(scanline<<8)|(Vnes.SPRMemory[i|3]);
				a=(Vnes.SPRMemory[i|2]&3)<<2;
				l=Vnes.SPRMemory[i|1];
				b=scanline-Vnes.SPRMemory[i]-1;

				if (Vnes.SPRMemory[i|2]&0x80) b^=15;  //vertical flip

				j=((l&1)<<12) | ( ((l&0xfe)<<4) + ((b&8)<<1) ) | (b&7);
				// spritetable  tile address    for the down part      y pos

				if (Vnes.Mapper_used==9) 
				  {
				    //				    MMC2_Access_Refresh(l&1,(l&0xfe)+(b>7));
				    //MMC2_Access_Refresh(l&1,((l&0xfe)+(b>7))+1);
				  }


				if ((!(Vnes.CPUMemory[0x2001]&4))&&(Vnes.SPRMemory[i|3]<8) )
				{
					xbeg=8-Vnes.SPRMemory[i|3];
					pos&=0xff00;
					pos|=8;

					if (Vnes.SPRMemory[i|2]&0x40)
					{
						for (x=xbeg;x<8;x++)
						{
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;		
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
							else pos++;					
						}
					}
					else
					{
						xbeg=7-xbeg;
						for (x=xbeg;x>=0;x--)
						{		
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
							else pos++;
						}
					}
				}
				else
				{
					if (Vnes.SPRMemory[i|2]&0x40)
					{
						for (x=0;x<8;x++)
						{
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;		
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
							else pos++;					
							if (!(pos&0xff)) break;
						}
					}
					else
					{
						for (x=7;x>=0;x--)
						{		
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];	
							else pos++;
							if (!(pos&0xff)) break;
						}
					}
				}
			}
			else
			{
			//8x8 sprite
				pos=(scanline<<8)|(Vnes.SPRMemory[i|3]);
				a=(Vnes.SPRMemory[i|2]&3)<<2;
				l=Vnes.SPRMemory[i|1];
				b=scanline-Vnes.SPRMemory[i]-1;
				if (Vnes.SPRMemory[i|2]&0x80) b^=7;  //vertical flip
				j=SpritePatternTable|(l<<4)|(b&7);

				if (Vnes.Mapper_used==9) 
				  {
				    //MMC2_Access_Refresh(SpritePatternTable,l);
				    //MMC2_Access_Refresh(SpritePatternTable,l+1);
				  }



				if ((!(Vnes.CPUMemory[0x2001]&4))&&(Vnes.SPRMemory[i|3]<8) )
				{
					xbeg=8-Vnes.SPRMemory[i|3];
					pos&=0xff00;
					pos|=8;

					if (Vnes.SPRMemory[i|2]&0x40)
					{
						for (x=xbeg;x<8;x++)
						{
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;		
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];
							else pos++;					
						}
					}
					else
					{
						xbeg=7-xbeg;
						for (x=xbeg;x>=0;x--)
						{		
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];
							else pos++;
						}
					}
				}
				else
				  {//drawit
					if (Vnes.SPRMemory[i|2]&0x40)
					{
						for (x=0;x<8;x++)
						{											
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];
							else pos++;
							if (!(pos&0xff)) break;
						}
					}
					else
					{
						for (x=7;x>=0;x--)
						{										
							col= ((Vnes.PPUPageIndex[j>>10][j&0x3ff]>>x)&1)|(((Vnes.PPUPageIndex[j>>10][(j&0x3ff)|8]>>x)&1)<<1) | a;
							if (col&3) *(  ((byte *)Vbuffer) + pos++ )=Vnes.PPUMemory[col+0x3f10];
							else pos++;		
							if (!(pos&0xff)) break;
						}
					}
				  }

			}
		}			
	}
}

void InitMirroring()
{

	switch (Vnes.var.mirroring)
	{
		case 1:   //vertical org=0,1
		{
			Vnes.PPUPageIndex[8]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[9]=Vnes.PPUMemory+0x2400;
			Vnes.PPUPageIndex[10]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[11]=Vnes.PPUMemory+0x2400;
			break;
		}
		case 2:  //horizontal org=0,2
		{
			Vnes.PPUPageIndex[8]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[9]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[10]=Vnes.PPUMemory+0x2400;
			Vnes.PPUPageIndex[11]=Vnes.PPUMemory+0x2400;
			break;
		}
		case 3:  //one screen low
		{
			Vnes.PPUPageIndex[8]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[9]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[10]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[11]=Vnes.PPUMemory+0x2000;
			break;
		}
		case 4:  //one screen high
		{
			Vnes.PPUPageIndex[8]=Vnes.PPUMemory+0x2400;
			Vnes.PPUPageIndex[9]=Vnes.PPUMemory+0x2400;
			Vnes.PPUPageIndex[10]=Vnes.PPUMemory+0x2400;
			Vnes.PPUPageIndex[11]=Vnes.PPUMemory+0x2400;
			break;
		}
		case 5:  //no morriroring, 4screen
		{
			Vnes.PPUPageIndex[8]=Vnes.PPUMemory+0x2000;
			Vnes.PPUPageIndex[9]=Vnes.PPUMemory+0x2400;
			Vnes.PPUPageIndex[10]=Vnes.PPUMemory+0x2800;
			Vnes.PPUPageIndex[11]=Vnes.PPUMemory+0x2C00;
			break;
		}
	}
}

